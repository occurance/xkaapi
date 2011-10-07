/*
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

static int __kaapi_try_preempt( kaapi_stack_t* stack, kaapi_task_t* pc );

/** 
    Here the stack of task is organised like this, task1 is pointed by pc and
    it will be the first running task.

               | task0  | 
    thread->pc | task1  |< thread->sfp->pc
               | task2  |
               | task3  |
              -----------
               | task4  |< thread->sfp->sp
               | task5  |
               | task6  |
              -----------
               | task7  |
      stack->sp| ....   |
     
   Where task0 is only here to say that a possible task may exists before the frame.
   In case of whole stack execution, the caller should only call execframe with the
   first frame correctly set.
   Initialy thread->sfp store information about the frame the first frame [pc:sfp->sp). 
   
   On return, we leave the stack in the following state after execution of all tasks into the
   frame pointed by sfp, including all the child tasks.
   All task bodies are set to executed.
   
                     | task0  |
thread->pc=stack->sp | xxxxx  |< thread->sfp->pc = thread->sfp->sp
                     | xxxxx  |
                     | xxxxx  |
                    -----------
                     | xxxxx  | 
                     | xxxxx  |
                     | xxxxx  |
                    -----------
                     | xxxxx  |
                     | ....   |

  The method could returns EWOULDBLOCK in order to indicate that a task cannot be executed.
  In that case, thread->pc points to the tasks (body is marked as suspended) and thread->sfp
  points to the current frame where the suspend task has been tried to be executed.

  Where the task becomes ready, one may continue the execution simply by calling
  execframe with the state that has been set on return with EWOULDBLOCK.
*/

/*
*/
int kaapi_stack_execframe( kaapi_stack_t* stack )
{
  kaapi_task_t*              sp; /* cache */
  kaapi_task_t*              pc; /* cache */
  kaapi_frame_t*             fp; /* cache for stack->sfp */

//  kaapi_task_body_internal_t body;
  uintptr_t                  state;
  kaapi_frame_t*             eframe = stack->esfp;
#if defined(KAAPI_USE_JMP)
  jmp_buf jmpbuff;
  int retval;
#endif
  
#if defined(KAAPI_USE_PERFCOUNTER)
  uint32_t                   cnt_tasks = 0;
#endif

  kaapi_assert_debug(stack->sfp >= stack->stackframe);
  kaapi_assert_debug(stack->sfp < stack->stackframe+KAAPI_MAX_RECCALL);

#if defined(KAAPI_USE_JMP)
  jmp_buf* save_buff = stack->jbuf;
  stack->jbuf = &jmpbuff;
  if ((retval = _setjmp(jmpbuff)) !=0) 
  {
    if (retval == EWOULDBLOCK)
    {
      /* here iff execute abnormal task: save pc, fp and return EWOULDBLOCK */
      stack->jbuf = save_buff;
      return EWOULDBLOCK;
    }
    else { // is EINTR: task is terminated
      --fp; 
      fp->pc = --pc;
      goto push_frame;
    }
  }
#endif
  
  fp = (kaapi_frame_t*)stack->sfp;

push_frame: /* here assume fp current frame where to execute task */

  sp = fp->sp;
  pc = fp->pc;

  /* init new frame for the next task to execute */
  fp[1].pc        = sp;
  fp[1].sp        = sp;
  fp[1].sp_data   = fp->sp_data;
  
  /* force previous write before next write */
  kaapi_writemem_barrier();

  /* push and update the current frame */
  stack->sfp = ++fp;
  kaapi_assert_debug( stack->sfp - stack->stackframe <KAAPI_MAX_RECCALL);
  
  /* stack of task growth down ! */
  for (; pc != sp; --pc)
  {
    kaapi_assert_debug( pc > sp );

redo_exec:
    
#if !defined(KAAPI_USE_JMP)
    if (likely(kaapi_task_markexec( pc )))
#endif
      ((kaapi_task_body_internal_t)pc->body)( pc->sp, fp, pc );
#if !defined(KAAPI_USE_JMP)
    else {
      state = pc->state;
      if (state == KAAPI_TASK_STATE_MERGE)
      {
        kaapi_aftersteal_body(pc->sp, fp, pc);
      }
      else if (state == KAAPI_TASK_STATE_STEAL)
      {
        /* try to preempted the task 0:do nothing, EINTR: the victim get it back... */
        int retval = __kaapi_try_preempt(stack,pc);
        if (retval == EWOULDBLOCK) 
        {
          fp[-1].pc = pc;  
          stack->sfp = fp-1;
#if defined(HUGEDEBUG)
  printf("%i::[execframe] Task %p, state: 0x%X is under execution suspended thread\n", 
    kaapi_get_self_kid(),
    (void*)stack->sfp->pc, 
    (unsigned int)stack->sfp->pc->state
  ); 
  fflush(stdout);
#endif
          return EWOULDBLOCK;
        }
        if (retval == EINTR) goto redo_exec;
        /* else: terminated, to nothing */
      }
      /* if I have been preempted, then continue to the next task */
      if (state & KAAPI_TASK_STATE_SIGNALED)
      {
        printf("I was preempted\n"); fflush(stdout);
        continue;
      }
    }
#endif

#if defined(KAAPI_USE_PERFCOUNTER)
    ++cnt_tasks;
#endif

    /* post execution: new tasks created ??? */
    if (unlikely(sp > fp->sp))
    {
      /* same pc in fp */
      fp[-1].pc = pc;
      goto push_frame;
    }
    else {
      /* else task has been executed: delete it from the queue in order
         stealer may detected ready tasks
      */
      fp[-1].pc = pc;
    }

  } /* end of the loop */
  kaapi_assert_debug( pc == sp );

  --fp;
  fp->pc = pc;

  kaapi_assert_debug( fp >= eframe);

//  kaapi_sched_lock(&stack->proc->lock);
  int tolock = 0;
  if (fp > eframe)
  {
    /* here it's a pop of frame: we lock the thread */
    while (fp > eframe) 
    {
      --fp;

      tolock = tolock || (fp <= stack->thieffp);
      if (tolock)
        kaapi_sched_lock(&stack->lock);
      /* finish to execute child tasks, pop current task of the frame */
      if (--fp->pc > fp->sp)
      {
        stack->sfp = fp;
//        kaapi_sched_unlock(&stack->proc->lock);
        if (tolock)
          kaapi_sched_unlock(&stack->lock);
//        printf("Pop framefp:%p, pc: %p, state (%i)\n", (void*)(fp), (void*)pc, (int)pc->state ); fflush(stdout);
        goto push_frame; /* remains work do do */
      }
    } 
    fp->sp = fp->pc;
  }
  stack->sfp = fp;
//  kaapi_sched_unlock(&stack->proc->lock);
  if (tolock)
    kaapi_sched_unlock(&stack->lock);

  /* end of the pop: we have finish to execute all the tasks */
  kaapi_assert_debug( fp->pc == fp->sp );
  kaapi_assert_debug( stack->sfp == eframe );

#if defined(KAAPI_USE_PERFCOUNTER)
  KAAPI_PERF_REG(stack->proc, KAAPI_PERF_ID_TASKS) += cnt_tasks;
  cnt_tasks = 0;
#endif
  return 0;
}


//#define HUGEDEBUG

/* try to preempt to stolen task:
   - lock the current state: add LOCKED flag to the state, if old value is not yet STEAL
    then the thief task has been terminated
*/
int __kaapi_try_preempt( kaapi_stack_t* stack, kaapi_task_t* pc )
{
  if ((kaapi_task_orstate(pc, KAAPI_TASK_STATE_LOCKED) & ~KAAPI_TASK_STATE_LOCKED) != KAAPI_TASK_STATE_STEAL)
  {
#if defined(HUGEDEBUG)
    printf("In __kaapi_try_preempt: task %p terminated/or aftersteal\n", pc); fflush(stdout);
#endif
    /* do not suspend because it is terminated */
    kaapi_assert( kaapi_task_getstate(pc) == (KAAPI_TASK_STATE_TERM | KAAPI_TASK_STATE_LOCKED) );
    return 0;
  }

  /* number of cycles = number of cycles of the thief requires between marksteal 
     and set reserved to the thief task
  */
  /* while ((void* volatile)(pc->reserved) == 0) */
  while ( ((volatile kaapi_task_t*)pc)->reserved ==0 )
  {
    if ((kaapi_task_getstate(pc) & ~KAAPI_TASK_STATE_LOCKED) == KAAPI_TASK_STATE_TERM) 
    {
#if defined(HUGEDEBUG)
      printf("In __kaapi_try_preempt: task %p terminated/or aftersteal\n", pc); fflush(stdout);
#endif
      return 0;
    } 
    kaapi_slowdown_cpu();
  }

  /* Uncomment these lines in order to activate preemption between DFG task
  */
#if 1
  /* try to preempt it: mark the thief task' state with SIGNALED bit */
  uintptr_t oldstate = kaapi_task_orstate(pc->reserved, KAAPI_TASK_STATE_SIGNALED);
  if ((oldstate == KAAPI_TASK_STATE_INIT) || (oldstate == KAAPI_TASK_STATE_ALLOCATED))
  {
#if defined(HUGEDEBUG)
    printf("In __kaapi_try_preempt: task: %p successfully preempts thief; %p\n", pc, pc->reserved); 
    fflush(stdout);
#endif
    /* else get back the task for myself : restore state to init */
    kaapi_task_setstate(pc, KAAPI_TASK_STATE_INIT);
    return EINTR;  
  }
#endif

  /* restore STEAL state and return EWOULDBLOCK */
#if defined(HUGEDEBUG)
  printf("%i::Task %p, state: 0x%X is under execution by thief task: %p, state: 0x%X. Suspended thread\n", 
    kaapi_get_self_kid(),
    (void*)pc, 
    (unsigned int)pc->state, 
    (void*)pc->reserved,
    (unsigned int)pc->reserved->state
  ); 
  fflush(stdout);
#endif

  kaapi_task_setstate(pc, KAAPI_TASK_STATE_STEAL);
  return EWOULDBLOCK;
}
