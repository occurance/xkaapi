/*
** kaapi_sched_advance.c
** xkaapi
** 
** Created on Tue Mar 31 15:18:04 2009
** Copyright 2009 INRIA.
**
** Contributors :
**
** christophe.laferriere@imag.fr
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

int kaapi_advance ( void )
{
  return kaapi_sched_advance( _kaapi_get_current_processor() );
}

/*
*/
int kaapi_sched_advance ( kaapi_processor_t* kproc )
{
  int i, replied = 0;

  kaapi_readmem_barrier();
  int count = KAAPI_ATOMIC_READ( &kproc->hlrequests.count );
  if (count ==0) return 0;
  
  /* process request on the kprocessor */
  kaapi_sched_stealprocessor( kproc );
  
  /* reply to all other request: no work ... */
  for (i=0; i<KAAPI_MAX_PROCESSOR; ++i)
  {
    if ( kaapi_request_ok( &kproc->hlrequests.requests[i] ) )
    {
      kaapi_request_reply( kproc->ctxt, 0, &kproc->hlrequests.requests[i], 0, 0 );
      ++replied;
      if (replied == count) break;
    }
  }
  KAAPI_ATOMIC_SUB( &kproc->hlrequests.count, replied ); 
  kaapi_assert_debug( KAAPI_ATOMIC_READ( &kproc->hlrequests.count ) >= 0 );

  return 0;
}


/* force link with kaapi_mt_init */
static void __attribute__((unused)) __kaapi_dumy_dummy(void)
{
  _kaapi_dummy(NULL);
}
