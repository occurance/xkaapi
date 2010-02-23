/*
** kaapi_task_exec.c
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


/** kaapi_stack_execchild
    Assumption: pc is the running task.
    Here the stack frame is organised like this:
          ----------
          | task1  |
     pc ->| task2  |
          | task3  |
          | retn   |
          ----------
          | task2.1|
          | task2.2|
          | task2.3|
     sp ->| ....   |
     
  
   Exec child will execute all tasks previously forked by pc task.
   Thus it first look for retn taks that mark begin of the next frame
   that contains the task.
*/
int kaapi_stack_execchild(kaapi_stack_t* stack, kaapi_task_t* pc)
{
  kaapi_uint32_t flag;
#if defined(KAAPI_USE_PERFCOUNTER)
  kaapi_uint32_t         cnt_tasks = 0;
#endif  
  kaapi_task_t*          saved_sp;
  char*                  saved_sp_data;
  int goto_redo_work;

  if (stack ==0) return EINVAL;
  if (kaapi_stack_isempty( stack ) ) return 0;

redo_work: 
  flag = pc->flag;
  if ( (flag >>KAAPI_TASK_BODY_SHIFT) == kaapi_retn_body ) 
  {
    /* inline retn body do not save stack frame before execution */
    kaapi_frame_t* frame = kaapi_task_getargst( pc, kaapi_frame_t);
    kaapi_task_setstate( frame->pc, KAAPI_TASK_S_TERM );
    kaapi_stack_restore_frame( stack, frame );
    /* read from memory */
    pc = stack->pc;
    --pc;
#if defined(KAAPI_USE_PERFCOUNTER)
    ++cnt_tasks;
#endif
    if (pc <= stack->sp) 
    {
      stack->pc = pc;
#if defined(KAAPI_USE_PERFCOUNTER)
      KAAPI_PERF_REG(stack->_proc, KAAPI_PERF_ID_TASKS) += cnt_tasks;
#endif
      return 0;
    }
    goto redo_work;
  }
  {
#if defined(KAAPI_CONCURRENT_WS)
    if (!kaapi_task_casstate(pc, KAAPI_TASK_S_INIT, KAAPI_TASK_S_EXEC )) 
#else
    if (pc->flag & KAAPI_TASK_S_STEAL)
#endif
    {
      /* rewrite pc into memory */
      stack->pc = pc;
#if defined(KAAPI_USE_PERFCOUNTER)
      KAAPI_PERF_REG(stack->_proc, KAAPI_PERF_ID_TASKS) += cnt_tasks;
#endif
      return EWOULDBLOCK;
    }
#if !defined(KAAPI_CONCURRENT_WS)
    kaapi_task_setstate( pc, KAAPI_TASK_S_EXEC );
#endif

    /* save the state of the stack */
    saved_sp      = stack->sp;
    saved_sp_data = stack->sp_data;
    stack->pc     = pc;

    kaapi_task_run( pc, stack );
#if defined(KAAPI_USE_PERFCOUNTER)
    ++cnt_tasks;
#endif
    goto_redo_work = (saved_sp != stack->sp);

    /* push restore_frame task if pushed tasks */
    if (goto_redo_work)
    {
      kaapi_frame_t* frame;
      /* inline version of kaapi_stack_pushretn in order to avoid to save all frame structure */
      kaapi_task_t* retn = kaapi_stack_toptask(stack);
      retn->flag  = KAAPI_TASK_STICKY | (kaapi_retn_body << KAAPI_TASK_BODY_SHIFT);

      frame = kaapi_stack_pushdata(stack, sizeof(kaapi_frame_t));
      retn->sp = (void*)frame;
      frame->pc = pc; /* <=> save pc, will mark this task as term after pop !! */
      frame->sp = saved_sp;
      frame->sp_data = saved_sp_data;
      kaapi_stack_pushtask(stack);

      /* update pc to the first forked task */
      pc = saved_sp;
    }

    /* process steal request 
       - here we always see the retn to split stack into frame.
    */
#if !defined(KAAPI_CONCURRENT_WS)
    if (stack->hasrequest !=0) 
    {
      stack->pc = pc;
#if defined(KAAPI_USE_PERFCOUNTER)
      KAAPI_PERF_REG(stack->_proc, KAAPI_PERF_ID_TASKS) += cnt_tasks;
      cnt_tasks = 0;
#endif
      kaapi_sched_advance(stack->_proc);
    }
#endif
    if (goto_redo_work) goto redo_work;

    kaapi_task_setstate( pc, KAAPI_TASK_S_TERM );    
  }

  /*next_task: */
  --pc;
  if (pc <= stack->sp) 
  {
    stack->pc = pc;
#if defined(KAAPI_USE_PERFCOUNTER)
    KAAPI_PERF_REG(stack->_proc, KAAPI_PERF_ID_TASKS) += cnt_tasks;
#endif
    return 0;
  }
  goto redo_work;
}
