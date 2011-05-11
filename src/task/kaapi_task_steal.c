/*
** kaapi_task_steal.c
** xkaapi
** 
** Created on Tue Mar 31 15:19:14 2009
** Copyright 2009 INRIA.
**
** Contributors :
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
#include <stdio.h>

#if defined(KAAPI_USE_CUDA)
# include "../machine/cuda/kaapi_cuda_execframe.h"
#endif


/**
*/
void kaapi_taskwrite_body( 
  void* taskarg, 
  kaapi_thread_t* thread  __attribute__((unused))
)
{
  unsigned int i;
  size_t count_params;

  const kaapi_format_t* fmt;
  void*                 orig_task_args;
  kaapi_access_t        access_param;

  kaapi_access_mode_t   mode_param;
  const kaapi_format_t* fmt_param;
  unsigned int          war_param;     /* */

  void*                 copy_task_args;
  void*                 copy_data_param;
  kaapi_access_t        copy_access_param;

  kaapi_tasksteal_arg_t* arg = (kaapi_tasksteal_arg_t*)taskarg;
  orig_task_args             = kaapi_task_getargs(arg->origin_task);
  copy_task_args             = arg->copy_task_args;
  war_param                  = arg->war_param;
  
  if (copy_task_args !=0)
  {
    /* for each parameter of the copy of the theft' task on mode:
       - V: we destroy the data
       - R,RW: do nothing
       - W,CW: set in field ->version of the original task args the field ->data of the copy args.
    */
    fmt         = arg->origin_fmt;
    count_params = kaapi_format_get_count_params( fmt, orig_task_args );
    for (i=0; i<count_params; ++i)
    {
      mode_param = KAAPI_ACCESS_GET_MODE( kaapi_format_get_mode_param(fmt, i, copy_task_args) );
      if (mode_param == KAAPI_ACCESS_MODE_V) 
      {
        fmt_param       = kaapi_format_get_fmt_param(fmt, i, orig_task_args);
        copy_data_param = (void*)kaapi_format_get_data_param(fmt, i, copy_task_args);
        (*fmt_param->dstor)(copy_data_param);
        continue;
      }

      if (KAAPI_ACCESS_IS_ONLYWRITE(mode_param) || KAAPI_ACCESS_IS_CUMULWRITE(mode_param))
      {
        //fmt_param         = kaapi_format_get_fmt_param(fmt, i, orig_task_args);
        access_param      = kaapi_format_get_access_param(fmt, i, orig_task_args); 
        copy_access_param = kaapi_format_get_access_param(fmt, i, copy_task_args); 

        /* write the value as the version */
        access_param.version = copy_access_param.data;
        kaapi_format_set_access_param(fmt, i, orig_task_args, &access_param );
      }
    }
  }

  /* if signaled thread was suspended, move it to the local queue */
#if 1
  kaapi_wsqueuectxt_cell_t* wcs = arg->origin_thread->wcs;
  if ((wcs != 0) && (arg->origin_thread->sfp->pc == arg->origin_task)) /* means thread has been suspended on this task */
  { 
    kaapi_readmem_barrier();
    kaapi_processor_t* kproc = arg->origin_thread->proc;
    //kaapi_processor_t* kproc = kaapi_get_current_processor();
    kaapi_assert_debug( kproc != 0);
    if (kaapi_cpuset_has(wcs->affinity, kproc->kid))
    //  /*kaapi_sched_readyempty(kproc) &&*/ kaapi_thread_hasaffinity(wcs->affinity, kproc->kid))
    {
      kaapi_thread_context_t* kthread = kaapi_wsqueuectxt_steal_cell( wcs );
      if (kthread !=0)
      {
        kaapi_wsqueuectxt_finish_steal_cell(wcs);
//    printf("Put thread %p, on myqueue: %li\n", (void*)thread, kproc->kid ); fflush(stdout);
        /* Ok, here we have theft the thread and no body else can steal it
           Signal the end of execution of forked task: 
           -if no war => mark the task as terminated 
           -if war and due to copy => mark the task as aftersteal in order to merge value
        */
        if (war_param ==0)
          kaapi_task_orstate( arg->origin_task, KAAPI_MASK_BODY_TERM );
        else
          kaapi_task_orstate( arg->origin_task, KAAPI_MASK_BODY_AFTER );

        kaapi_sched_lock(&kproc->lock);
        kaapi_sched_pushready(kproc, kthread );
        kaapi_sched_unlock(&kproc->lock);
        return;
      }
    }
  }
#endif

  /* signal the task : mark it as executed, the old returned body should have steal flag */
  kaapi_assert_debug( kaapi_task_state_issteal( kaapi_task_getstate( arg->origin_task) ) );
  uintptr_t oldstate __attribute__((unused));
  if (war_param ==0)
    oldstate = kaapi_task_orstate( arg->origin_task, KAAPI_MASK_BODY_TERM );
  else
    oldstate = kaapi_task_orstate( arg->origin_task, KAAPI_MASK_BODY_AFTER );
}


/* bound tasks
 */

#include <stdlib.h>
#include <string.h>

typedef struct bound_task
{
  kaapi_tasksteal_arg_t arg;
  struct bound_task* next;
} bound_task_t;

static bound_task_t* volatile bound_task_head = NULL;
static kaapi_atomic_t bound_task_lock = {0};

static inline void acquire_bound_task_lock(void)
{
  kaapi_atomic_t* const lock = &bound_task_lock;

  while (1)
  {
    if ((KAAPI_ATOMIC_READ(lock) == 0) && KAAPI_ATOMIC_CAS(lock, 0, 1))
      break ;
    kaapi_slowdown_cpu();
  }
}

static inline void release_bound_task_lock(void)
{
  KAAPI_ATOMIC_WRITE(&bound_task_lock, 0);
}

static void push_bound_task(kaapi_tasksteal_arg_t* arg)
{
  bound_task_t* const bt = malloc(sizeof(bound_task_t));
  bt->next = NULL;
  memcpy(&bt->arg, arg, sizeof(kaapi_tasksteal_arg_t));

  acquire_bound_task_lock();

  bt->next = bound_task_head;
  bound_task_head = bt;
  __sync_synchronize();

  release_bound_task_lock();
}

int pop_bound_task(kaapi_tasksteal_arg_t* arg)
{
  /* assume kproc locked */

  bound_task_t* head;

  acquire_bound_task_lock();

  head = bound_task_head;
  if (head == NULL)
  {
    release_bound_task_lock();
    return -1;
  }

  bound_task_head = head->next;
  release_bound_task_lock();

  memcpy(arg, &head->arg, sizeof(kaapi_tasksteal_arg_t));
  free(head);

  return 0;
}

/**
*/
void kaapi_tasksteal_body( void* taskarg, kaapi_thread_t* thread  )
{
#if defined(KAAPI_USE_CUDA)
  kaapi_thread_context_t* const self_thread = kaapi_self_thread_context();
  kaapi_processor_t* const self_proc = self_thread->proc;
#endif

  unsigned int           i;
  size_t                 count_params;
#if 0 /* remove unused warning */
  int                    push_write;
  int                    w_param;
#endif
  kaapi_task_t*          task;
  kaapi_tasksteal_arg_t* arg;
  kaapi_task_body_t      body;          /* format of the stolen task */
  kaapi_format_t*        fmt;
  unsigned int           war_param;     /* */

  void*                  orig_task_args;
  void*                  data_param;
  kaapi_access_t         access_param;

  kaapi_access_mode_t    mode_param;
  const kaapi_format_t*  fmt_param;

  void*                  copy_task_args;
  void*                  copy_data_param;
  kaapi_access_t         copy_access_param;

  /* get information of the task to execute */
  arg = (kaapi_tasksteal_arg_t*)taskarg;
  kaapi_assert_debug( kaapi_task_state_issteal( kaapi_task_getstate(arg->origin_task) ) );

  /* short the execution path. push this task in the bound
     task list instead and execute it on the affine core.
  */
  if (arg->origin_task->binding != (unsigned long)-1)
  {
    /* TODO: not bound to the local core */
    if (1)
    {
      printf("TODO: BINDING\n");
      push_bound_task(arg);
      return ;
    }
  }
  
  /* format of the original stolen task */  
  body            = kaapi_task_getbody(arg->origin_task);
  kaapi_assert_debug( kaapi_isvalid_body( body ) );

  fmt             = kaapi_format_resolvebybody( body );
  kaapi_assert_debug( fmt !=0 );
  
  /* the the original task arguments */
  orig_task_args  = kaapi_task_getargs(arg->origin_task);
  count_params    = kaapi_format_get_count_params(fmt, orig_task_args); 
  arg->copy_task_args = 0;
  
  /**/
  war_param = arg->war_param;
  
  if (!war_param)
  {
    /* Execute the orinal body function with the original args */
#if defined(KAAPI_USE_CUDA)
    if (self_proc->proc_type == KAAPI_PROC_TYPE_CUDA)
    {
      /* todo_remove */
      if (fmt->entrypoint[KAAPI_PROC_TYPE_CUDA] == 0)
        body(orig_task_args, thread);
      else
        /* todo_remove */
        kaapi_cuda_exectask(self_thread, orig_task_args, fmt);
    }
    else
#endif
    body(orig_task_args, thread);

    /* push task that will be executed after all created task by the user task */
    task = kaapi_thread_toptask( thread );
    kaapi_task_init( task, kaapi_taskwrite_body, arg );
    kaapi_thread_pushtask( thread );
  }
  else /* it exists at least one w parameter with war dependency */
  {
    copy_task_args       = kaapi_thread_pushdata( thread, fmt->size);
    arg->copy_task_args  = copy_task_args;
    arg->origin_fmt      = fmt;

    /* there are possibly non formated params  */
    memcpy(copy_task_args, orig_task_args, fmt->size);

    for (i=0; i<count_params; ++i)
    {
      mode_param      = KAAPI_ACCESS_GET_MODE( kaapi_format_get_mode_param(fmt, i, orig_task_args) ); 
      fmt_param       = kaapi_format_get_fmt_param(fmt, i, orig_task_args);
      
      if (mode_param == KAAPI_ACCESS_MODE_V) 
      {
        data_param      = kaapi_format_get_data_param(fmt, i, orig_task_args);
        copy_data_param = kaapi_format_get_data_param(fmt, i, copy_task_args);
        (*fmt_param->cstorcopy)(copy_data_param, data_param);
        continue;
      }

      /* initialize all parameters ... */
      access_param              = kaapi_format_get_access_param(fmt, i, orig_task_args);
      copy_access_param         = kaapi_format_get_access_param(fmt, i, copy_task_args);
      copy_access_param.data    = access_param.data;
      copy_access_param.version = 0; /*access_param->version; / * not required... * / */
      
      if (KAAPI_ACCESS_IS_ONLYWRITE(mode_param) || KAAPI_ACCESS_IS_CUMULWRITE(mode_param) )
      {
        if ((war_param & (1<<i)) !=0)
        { 
          /* allocate new data */
          kaapi_memory_view_t view = kaapi_format_get_view_param(fmt, i, orig_task_args);
#if defined(KAAPI_DEBUG)
          copy_access_param.data    = calloc(1, kaapi_memory_view_size(&view));
#else
          copy_access_param.data    = malloc(kaapi_memory_view_size(&view));
#endif
          if (fmt_param->cstor !=0) 
            (*fmt_param->cstor)(copy_access_param.data);
          
          /* set new data to copy of the data with new view */
          kaapi_format_set_access_param(fmt, i, copy_task_args, &copy_access_param );
          kaapi_memory_view_reallocated( &view );
          kaapi_format_set_view_param( fmt, i, copy_task_args, &view );
        }
      }
    }

    /* Execute the orinal body function with the copy of args: do not push it... ?
       WHY to push a nop ?
    */
    task = kaapi_thread_toptask( thread );
    kaapi_task_init( task, kaapi_nop_body, copy_task_args);
    kaapi_thread_pushtask( thread );

    /* call directly the stolen body function */
#if defined(KAAPI_USE_CUDA)
    if (self_proc->proc_type == KAAPI_PROC_TYPE_CUDA)
    {
      /* todo_remove */
      if (fmt->entrypoint[KAAPI_PROC_TYPE_CUDA] == 0)
        body(copy_task_args, thread);
      else
        /* todo_remove */
        kaapi_cuda_exectask(self_thread, copy_task_args, fmt);
    }
    else
#endif
    body( copy_task_args, thread);

    /* push task that will be executed after all created task by the user task */
    task = kaapi_thread_toptask( thread );
    kaapi_task_init( task, kaapi_taskwrite_body, arg );
    kaapi_thread_pushtask( thread );
  }
}
