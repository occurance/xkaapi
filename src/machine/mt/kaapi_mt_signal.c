/*
** xkaapi
** 
**
** Copyright 2009,2010,2011,2012 INRIA.
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

#if defined(_WIN32)
#    define KAAPI_NOT_USE_EXECINFO 1
#elif defined(__APPLE__)
#  if (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1040)
#    include <execinfo.h>
#  else
#    define KAAPI_NOT_USE_EXECINFO 1
#  endif
#else 
#  include <execinfo.h>
#endif
#include "kaapi_impl.h"
#include <signal.h>
#include <unistd.h>


// --------------------------------------------------------------------
void _kaapi_signal_dump_backtrace(int sig, siginfo_t *si, void *unused)
{
#if defined(__APPLE__) || defined(__linux__)
  const unsigned int MAX_DEPTH = 100;
  void *trace[MAX_DEPTH];
  unsigned int trace_size;
  char **trace_strings;
  
  if (sig == SIGSEGV)
  {
    printf("Catch SIGSEGV, address: %p\n", si->si_addr );
  }

  trace_size = backtrace(trace, MAX_DEPTH);
  trace_strings = backtrace_symbols(trace, trace_size);
  for (unsigned int i=0; i<trace_size; ++i)
  { 
    printf("[%i]: %s\n", i,  trace_strings[i] );
  }
  free(trace_strings); // malloc()ed by backtrace_symbols
#endif
#if defined(KAAPI_USE_PERFCOUNTER)
  _kaapi_signal_dump_counters(sig);  
#endif
  _exit(sig);
}


/* Kaapi signal handler to dump the state of the all the kprocessor
*/
void _kaapi_signal_dump_state(int sig)
{
  unsigned int i;
  /* block alarm, if period was to short... */
  sigset_t block_alarm;
  sigemptyset (&block_alarm);
  sigaddset (&block_alarm, SIGALRM);
  pthread_sigmask( SIG_BLOCK, &block_alarm, 0 );
 
  
  printf("\n\n>>>>>>>>>>>>>>>> DUMP PROCESSORS STATE\n");
  printf("                  Date: %f (s)\n", (double)(kaapi_get_elapsedns() - kaapi_default_param.startuptime)/1000000000.0 );
  for (i=0; i< kaapi_count_kprocessors; ++i)
  {
    kaapi_processor_t* kproc = kaapi_all_kprocessors[i];
    
    kaapi_sched_lock( &kproc->lock );
    printf("\n\n*****Kprocessor kid: %i\n", i);
    printf("Proc type       : %s\n", (kproc->proc_type == KAAPI_PROC_TYPE_CPU ? "CPU" : "GPU") );
    printf("Kprocessor cpuid: %i\n", kproc->cpuid);
    printf("Current thread  : %p\n", (void*)kproc->thread);
    printf("\n");
    fflush(stdout);

    fflush(stdout);

    printf("SuspendList     : %s", (kaapi_sched_suspendlist_empty(kproc) ? "no" : "yes") );
    if (!kaapi_sched_suspendlist_empty( kproc ))
    {
      printf(", thread(s): ");
      kaapi_wsqueuectxt_cell_t* cell = kproc->lsuspend.head;
      while (cell !=0)
      {
        int status = KAAPI_ATOMIC_READ(&cell->state);
        printf("[%p", (void*)cell->thread);
        switch (status) {
          case KAAPI_WSQUEUECELL_INLIST:
            printf(" inlist]  ");
            break;
          case KAAPI_WSQUEUECELL_READY:
            printf(" ready]  ");
            break;
          case KAAPI_WSQUEUECELL_OUTLIST:
            printf(" outlist]  ");
            break;
          case KAAPI_WSQUEUECELL_STEALLIST:
            printf(" steal]  ");
            break;
          default:
            printf(" <%i>]  ",status);
            break;
        }
        cell = cell->next;
      }
    }
    printf("\n");
    fflush(stdout);

    printf("\n**** Thread(s):\n");
    /* dump each thread */
    if (kproc->thread !=0)
    {
      kaapi_thread_print(stdout, kproc->thread);
      fflush(stdout);
    }
        
    if (!kaapi_sched_suspendlist_empty( kproc ))
    {
      kaapi_wsqueuectxt_cell_t* cell = kproc->lsuspend.head;
      while (cell !=0)
      {
        if (cell->thread !=0)
        {
          printf("Suspended thread:%p -> @task condition:%p\n",
            (void*)cell->thread, 
            (void*)cell->thread->stack.sfp->pc
          );
          kaapi_thread_print(stdout, cell->thread);
        }
        cell = cell->next;
      }
      fflush(stdout);
    }

    kaapi_sched_unlock( &kproc->lock );
  }
  printf("<<<<<<<<<<<<<<<<< END DUMP\n\n\n");
  fflush(stdout);

  pthread_sigmask( SIG_UNBLOCK, &block_alarm, 0 );
  alarm( kaapi_default_param.alarmperiod );
}
