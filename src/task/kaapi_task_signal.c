/*
** kaapi_task_signal.c
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

/**
*/
static void kaapi_aftersteal_body( kaapi_task_t* task, kaapi_stack_t* stack)
{
  int i, countparam;
  kaapi_format_t* fmt;           /* format of the task */
  void* arg;
  
  /* the task has been stolen: format contains the original body */
  fmt = kaapi_format_resolvebybody( task->format );
  kaapi_assert_debug( fmt !=0 );

  KAAPI_LOG(100, "aftersteal task: 0x%x\n", task );

  /* report data to version to global data */
  arg = kaapi_task_getargs(task);
  countparam = fmt->count_params;
  for (i=0; i<countparam; ++i)
  {
    kaapi_access_mode_t m = KAAPI_ACCESS_GET_MODE(fmt->mode_params[i]);

    if (KAAPI_ACCESS_IS_ONLYWRITE(m))
    {
      void* param = (void*)(fmt->off_params[i] + (char*)arg);
      kaapi_format_t* fmt_param = fmt->fmt_params[i];
      kaapi_access_t* access = (kaapi_access_t*)(param);
      /* TODO: improve management of shared data, if it is a big data or not... */
      (*fmt_param->assign)( access->data, access->version );
      (*fmt_param->dstor)( access->version );
      free(((kaapi_gd_t*)access->version)-1);
      access->version = 0;
    }
    else
    { /* nothing to do ?
      */    
    }
  }
}


/**
*/
void kaapi_tasksig_body( kaapi_task_t* task, kaapi_stack_t* stack)
{
/*
  printf("Thief end, @stack: 0x%x\n", stack);
  fflush( stdout );
*/
  kaapi_task_t* task2sig;

  task2sig = kaapi_task_getargst( task, kaapi_task_t);
  KAAPI_LOG(100, "signaltask: 0x%x -> task2signal: 0x%x\n", task, task2sig );

  if (kaapi_task_isadaptive(task2sig))
  {
    kaapi_taskadaptive_t* ta = task2sig->sp;
    kaapi_assert_debug( ta !=0 );

    /* flush in memory all pending write */  
    kaapi_writemem_barrier();

    KAAPI_ATOMIC_DECR( &ta->thievescount );
  } 
  else if (kaapi_task_issync(task2sig))
  {
    /* */
    kaapi_task_setflags( task2sig, KAAPI_TASK_STICKY );

    /* flush in memory all pending write */  
    kaapi_writemem_barrier();

    task2sig->body   = &kaapi_aftersteal_body;
    KAAPI_LOG(100, "signaltask DFG task stolen: 0x%x\n", task2sig );
  }
}
