/*
 ** xkaapi
 ** 
 ** Created on Tue Feb 23 16:56:43 2010
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

/*
*/
void kaapi_taskwaitend_body( void* sp, kaapi_thread_t* thread )
{
  kaapi_threadgroup_t thgrp = (kaapi_threadgroup_t)sp;
  KAAPI_ATOMIC_WRITE( &thgrp->countend, 0 );
}


/*
*/
void kaapi_tasksignalend_body( void* sp, kaapi_thread_t* thread )
{
  kaapi_threadgroup_t thgrp = (kaapi_threadgroup_t)sp;

#if 0 // TODO: distributed re-execution.
/* Thread should not be referenced by a thief during this operation and after all
   other thread have been re-executed
*/
  /* reload the thread if it was saved */
  if (thgrp->save_mainthread !=0)
  {
    int partid = _kaapi_self_thread()->partid;
    if (partid != -1)
      kaapi_assert( 0 == kaapi_threadgroup_restore_thread( thgrp, partid ) );
  }
#endif

  if (KAAPI_ATOMIC_INCR( &thgrp->countend ) == thgrp->group_size)
  {
    kaapi_task_setbody( thgrp->waittask, kaapi_taskwaitend_body );
/*    pthread_cond_signal( &thgrp->cond ); */
  }

  /* detach the thread from the processor (it was managed by the group) */
  kaapi_processor_t* kproc = _kaapi_get_current_processor();
  kaapi_thread_context_t* kthread = kproc->thread;
  
  if (kthread != thgrp->mainctxt)
  {
#if 0
    printf("Thread: %p affinity:%u  mapped on proc:%i\n", kthread, kthread->affinity, kproc->kid );
    fflush(stdout);
#endif
    /* detach the thread */
    kproc->thread = 0;
  }
}