/*
 ** kaapi_sched_stealstack.c
 ** xkaapi
 ** 
 ** Created on Tue Mar 31 15:18:04 2009
 ** Copyright 2009 INRIA.
 **
 ** Contributor :
 **
 ** thierry.gautier@inrialpes.fr
 ** 
 ** This software is a computer program whose purpose is to execute
 ** multithreaded computation with data flow synchronization between
 ** threads.
 ** 
 ** This software is governed by the CeCILL-C license under French law
 ** and abiding by the rules of distribution of free software.  You can
 ** use, modify and/ or redistribute the software under the terms of
 ** the CeCILL-C license as circulated by CEA, CNRS and INRIA at the
 ** following URL "http://www.cecill.info".
 ** 
 ** As a counterpart to the access to the source code and rights to
 ** copy, modify and redistribute granted by the license, users are
 ** provided only with a limited warranty and the software's author,
 ** the holder of the economic rights, and the successive licensors
 ** have only limited liability.
 ** 
 ** In this respect, the user's attention is drawn to the risks
 ** associated with loading, using, modifying and/or developing or
 ** reproducing the software by the user in light of its specific
 ** status of free software, that may mean that it is complicated to
 ** manipulate, and that also therefore means that it is reserved for
 ** developers and experienced professionals having in-depth computer
 ** knowledge. Users are therefore encouraged to load and test the
 ** software's suitability as regards their requirements in conditions
 ** enabling the security of their systems and/or data to be ensured
 ** and, more generally, to use and operate it in the same conditions
 ** as regards security.
 ** 
 ** The fact that you are presently reading this means that you have
 ** had knowledge of the CeCILL-C license and that you accept its
 ** terms.
 ** 
 */
#include "kaapi_impl.h"
#if defined(KAAPI_DEBUG_LOURD)
#include <unistd.h>
#endif

/* Compute if the task with arguments pointed by sp and with format task_fmt is ready
 Return the number of non ready data
 */
static int kaapi_task_computeready( 
  kaapi_task_t* task __attribute__((unused)),
  void* sp, 
  const kaapi_format_t* task_fmt, 
  unsigned int* war_param, 
  kaapi_hashmap_t* map 
)
{
  int i, wc, countparam;
  
  countparam = wc = task_fmt->count_params;
  for (i=0; i<countparam; ++i)
  {
    kaapi_access_mode_t m = KAAPI_ACCESS_GET_MODE(task_fmt->mode_params[i]);
    if (m == KAAPI_ACCESS_MODE_V) 
    {
      --wc;
      continue;
    }
    kaapi_access_t* access = (kaapi_access_t*)(task_fmt->off_params[i] + (char*)sp);
    
    /* */
    kaapi_gd_t* gd = &kaapi_hashmap_findinsert( map, access->data )->u.value;
    
    /* compute readyness of access */
    if (   KAAPI_ACCESS_IS_ONLYWRITE(m)
        || (gd->last_mode == KAAPI_ACCESS_MODE_VOID)
        || (KAAPI_ACCESS_IS_CONCURRENT(m, gd->last_mode))
       )
    {
      --wc;
      if (KAAPI_ACCESS_IS_ONLYWRITE(m) && KAAPI_ACCESS_IS_READ(gd->last_mode))
        *war_param |= 1<<i;
    }
    /* optimization: break from enclosest loop here */
    
    /* update map information for next access if no set */
    if (gd->last_mode == KAAPI_ACCESS_MODE_VOID)
      gd->last_mode = m;
    
    /* currently, datas produced by aftersteal_task are visible to thief in order to augment
     the parallelism by breaking chain of versions (W->R -> W->R ), the second W->R could
     be used (the middle R->W is splitted -renaming is also used in other context-).
     But we do not take into account of this extra parallelism.
     
     The problem that remains to solve:
     - if task_bot is kaapi_aftersteal_body task, then it corresponds to a stolen task already
     finished (before the victim thread is trying to execute it, else we will see an nop_body. 
     In this task:
     - each access->data points to the original data in the victim thread
     - each access->version points either to 0 (no new data produced) either to an heap allocated
     data value != access->data that may be used for next reader.
     This situation is only all the case W, CW access mode accesses.
     - this task_bot is considered as terminated and W, CW or RW data value 'access->version' may be consumed but
     - access->version (for W or CW) may be released if the victim thread executes the task 'aftersteal'
     - in the same time, this pointed data may be read by the stolen closure (if all its access are ready, 
     here we do not know about this fact: we need to wait after looking for all others parameters of the current task)
     
     A solution is to store references to accesses to last version in gd table:
     - if a closure is found to be ready:
     - we assume than gd map will store a pointer to the version to read (or write).
     - the victim will made a call to 'cas(access->version, access->version, 0)' to delete version
     1/ if ok -> the victim copy the data into the gd an set to 0 its version
     2/ if nok -> the victim only copy the version to the gd, 2 versions of the data will 
     be alive until the next aftersteal (that cannot be executed because the victim if currently
     under executing a first (previous) after steal).
     - the thief will made a call to 'cas(gd->access->version, gd->version, 0)' to keep the owner ship 
     on the data gd->access->version.
     1/ if ok -> the thief get the owner ship of data which will be deleted during the aftersteal
     of the under stealing closure
     2/ ifnok -> the victim has executed the aftersteal, then the victim may read gd->data as the correct version
     (this guarantee has to be written to ensure than reading gd->data is yet valid)
     (no other tasks have modified the shared between the 1rst aftersteal the task detected to be stolen)
     - seems good... with more details
     */
  }
  return wc;
}


/* Mark non ready data that are waiting.
   Only call in case of static scheduling.
   This is the same code as computeready except the mutation of r access mode.
*/
static int kaapi_task_markready_recv( kaapi_task_t* task, void* sp, kaapi_hashmap_t* map )
{
  int i, wc, countparam;
  const kaapi_format_t* task_fmt;
  const kaapi_taskrecv_arg_t* arg;

  arg = (kaapi_taskrecv_arg_t*)sp;
  task_fmt = kaapi_format_resolvebybody( arg->original_body );

  sp = arg->original_sp;

  countparam = wc = task_fmt->count_params;
  for (i=0; i<countparam; ++i)
  {
    kaapi_access_mode_t m = KAAPI_ACCESS_GET_MODE(task_fmt->mode_params[i]);
    if (m == KAAPI_ACCESS_MODE_V) 
    {
      --wc;
      continue;
    }
    /* mute r mode to be w, while the body is recv_task, the r or rw are not ready */
    if (KAAPI_ACCESS_IS_READ(m))
    {
      if (kaapi_threadgroup_paramiswait( task, i )) 
      {
        m = KAAPI_ACCESS_MODE_W;
/*        printf("->>> recv task: %p, mute r mode to w\n",task); */
      }
    }
    kaapi_access_t* access = (kaapi_access_t*)(task_fmt->off_params[i] + (char*)sp);
    
    /* */
    kaapi_gd_t* gd = &kaapi_hashmap_findinsert( map, access->data )->u.value;
    
    /* compute readyness of access */
    if (   KAAPI_ACCESS_IS_ONLYWRITE(m)
        || (gd->last_mode == KAAPI_ACCESS_MODE_VOID)
        || (KAAPI_ACCESS_IS_CONCURRENT(m, gd->last_mode))
       )
    {
      --wc;
    }
    /* optimization: break from enclosest loop here */
    
    /* update map information for next access if no set */
    if (gd->last_mode == KAAPI_ACCESS_MODE_VOID)
      gd->last_mode = m;
    
    /* currently, datas produced by aftersteal_task are visible to thief in order to augment
     the parallelism by breaking chain of versions (W->R -> W->R ), the second W->R could
     be used (the middle R->W is splitted -renaming is also used in other context-).
     But we do not take into account of this extra parallelism.
     
     The problem that remains to solve:
     - if task_bot is kaapi_aftersteal_body task, then it corresponds to a stolen task already
     finished (before the victim thread is trying to execute it, else we will see an nop_body. 
     In this task:
     - each access->data points to the original data in the victim thread
     - each access->version points either to 0 (no new data produced) either to an heap allocated
     data value != access->data that may be used for next reader.
     This situation is only all the case W, CW access mode accesses.
     - this task_bot is considered as terminated and W, CW or RW data value 'access->version' may be consumed but
     - access->version (for W or CW) may be released if the victim thread executes the task 'aftersteal'
     - in the same time, this pointed data may be read by the stolen closure (if all its access are ready, 
     here we do not know about this fact: we need to wait after looking for all others parameters of the current task)
     
     A solution is to store references to accesses to last version in gd table:
     - if a closure is found to be ready:
     - we assume than gd map will store a pointer to the version to read (or write).
     - the victim will made a call to 'cas(access->version, access->version, 0)' to delete version
     1/ if ok -> the victim copy the data into the gd an set to 0 its version
     2/ if nok -> the victim only copy the version to the gd, 2 versions of the data will 
     be alive until the next aftersteal (that cannot be executed because the victim if currently
     under executing a first (previous) after steal).
     - the thief will made a call to 'cas(gd->access->version, gd->version, 0)' to keep the owner ship 
     on the data gd->access->version.
     1/ if ok -> the thief get the owner ship of data which will be deleted during the aftersteal
     of the under stealing closure
     2/ ifnok -> the victim has executed the aftersteal, then the victim may read gd->data as the correct version
     (this guarantee has to be written to ensure than reading gd->data is yet valid)
     (no other tasks have modified the shared between the 1rst aftersteal the task detected to be stolen)
     - seems good... with more details
     */
  }
  return wc;
}


/** Steal task in the frame [frame->pc:frame->sp)
 */
static int kaapi_sched_stealframe
(
  kaapi_thread_context_t*       thread, 
  kaapi_frame_t*                frame, 
  kaapi_hashmap_t*              map, 
  kaapi_listrequest_t*          lrequests, 
  kaapi_listrequest_iterator_t* lrrange
)
{
  const kaapi_format_t* task_fmt;
  kaapi_task_body_t     task_body;
  kaapi_task_t*         task_top;
  kaapi_task_t*         task_exec;
  int                   replycount;
  kaapi_task_splitter_t splitter; 
  void*                 argsplitter;
    
  /* suppress history of the previous frame ! */
  kaapi_hashmap_clear( map );
  task_body  = kaapi_nop_body;
  task_top   = frame->pc;
  task_exec  = 0;
  replycount = 0;
  
  /* */
  while ( !kaapi_listrequest_iterator_empty(lrrange) && (task_top > frame->sp))
  {
    task_body = kaapi_task_getbody(task_top);
    
    /* its an adaptive task !!! */
    if (task_body == kaapi_adapt_body)
    {
      /* only steal into an correctly initialized steal context */
      kaapi_stealcontext_t* const sc = kaapi_task_getargst(task_top, kaapi_stealcontext_t);
      if (sc->header.flag & KAAPI_SC_INIT) 
      {
        /* should not be reorder before the barrier */
        splitter = sc->splitter;
        argsplitter = sc->argsplitter;
       
        if ( (splitter !=0) && (argsplitter !=0) )
        {
#if (KAAPI_USE_EXECTASK_METHOD == KAAPI_CAS_METHOD)
          const uintptr_t state = kaapi_task_orstate( task_top, KAAPI_MASK_BODY_STEAL );
          /* do not steal if terminated */
          if (likely( !kaapi_task_state_isterm(state) ) )
#else
          kaapi_assert_m(0, "Sequential execution cannot be used in parallel execution");
#endif        
          {
            /* steal sucess */

#if (KAAPI_USE_EXECTASK_METHOD == KAAPI_CAS_METHOD)
            /* possible race, reread the splitter */
            splitter = sc->splitter;
            argsplitter = sc->argsplitter;
            if ((splitter != 0) && (argsplitter != 0))
#endif
            kaapi_task_splitter_adapt(thread, task_top, splitter, argsplitter, lrequests, lrrange );

#if (KAAPI_USE_EXECTASK_METHOD == KAAPI_CAS_METHOD)
            /* here suspend bit was set: reset it */
            kaapi_task_andstate( task_top, ~KAAPI_MASK_BODY_STEAL );
#endif
          }
        }
      } /* end if init */
      --task_top;
      continue;
    }
    
    /* * weak symbol may also be used ? 
       * not that recv body is an empty function to serve as a mark. It could
       be put into the nonpartitioning code...
    */
    if (task_body == &kaapi_taskrecv_body)
    {
      kaapi_task_markready_recv( task_top, kaapi_task_getargs(task_top), map );
    }
    else 
    {
      task_fmt = kaapi_format_resolvebybody( task_body );
      if (task_fmt !=0)
      {
        unsigned int war_param = 0;
        int wc = kaapi_task_computeready( task_top, kaapi_task_getargs(task_top), task_fmt, &war_param, map );
        if ((wc ==0) && kaapi_task_isstealable(task_top))
        {
#if (KAAPI_USE_EXECTASK_METHOD == KAAPI_CAS_METHOD)
          uintptr_t state = kaapi_task_orstate( task_top, KAAPI_MASK_BODY_STEAL);
          if (likely( kaapi_task_state_isstealable(state) ) ) // means SUSPEND and EXEC was not set before
          {
#elif (KAAPI_USE_EXECTASK_METHOD == KAAPI_THE_METHOD)
#elif (KAAPI_USE_EXECTASK_METHOD == KAAPI_SEQ_METHOD)
              kaapi_assert_m(0,"Not implemented for work stealing"
#endif
              kaapi_task_splitter_dfg(thread, task_top, war_param, lrequests, lrrange );
#if (KAAPI_USE_EXECTASK_METHOD == KAAPI_THE_METHOD)
#elif (KAAPI_USE_EXECTASK_METHOD == KAAPI_CAS_METHOD)
          }
#endif
        }
      } /* if fmt != 0 */
    } /* else if != kaapi_taskrecv_body */
    --task_top;
  }
  
  return replycount;
}
  
  
/** Steal task in the stack from the bottom to the top.
 Do not steal curr if !=0 (current running adaptive task) in case of cooperative WS.
 This signature is the same as a splitter function.
 */
int kaapi_sched_stealstack  
( 
  kaapi_thread_context_t*       thread, 
  kaapi_task_t*                 curr __attribute__((unused)), 
  kaapi_listrequest_t*          lrequests, 
  kaapi_listrequest_iterator_t* lrrange
)
{
#if (KAAPI_USE_EXECTASK_METHOD == KAAPI_CAS_METHOD)
  kaapi_frame_t*           top_frame;
#endif

  int                      replycount;
  
  kaapi_hashmap_t          access_to_gd;
  kaapi_hashentries_bloc_t stackbloc;
  
  if ((thread ==0) || (thread->unstealable != 0) /*|| kaapi_frame_isempty( thread->sfp)*/) return 0;
  replycount = 0;
  
  /* be carrefull, the map should be clear before used */
  kaapi_hashmap_init( &access_to_gd, &stackbloc );
  
#if (KAAPI_USE_EXECTASK_METHOD == KAAPI_CAS_METHOD)  
  /* try to steal in each frame */
  for (top_frame =thread->stackframe; (top_frame <= thread->sfp) && !kaapi_listrequest_iterator_empty(lrrange); ++top_frame)
  {
    /* void frame ? */
    if (top_frame->pc == top_frame->sp) continue;
    kaapi_sched_stealframe( thread, top_frame, &access_to_gd, lrequests, lrrange );
  }
  
#elif (KAAPI_USE_EXECTASK_METHOD == KAAPI_THE_METHOD)
#else
#  error "Bad steal frame method"    
#endif
  
  kaapi_hashmap_destroy( &access_to_gd );
  
  return replycount;
}
