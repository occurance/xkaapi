/*
 ** xkaapi
 ** 
 **
 ** Copyright 2009,2010,2011,2012 INRIA.
 **
 ** Contributors :
 ** joao.lima@imag.fr
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

static inline int kaapi_onereadytasklist_steal(
    const kaapi_processor_t* thiefprocessor,
    kaapi_onereadytasklist_t* ortl,
    kaapi_taskdescr_t** td 
) 
{
  int size_ws;
  kaapi_taskdescr_t* curr;
  
  if( kaapi_onereadytasklist_isempty( ortl ) )
    return EBUSY;

  kaapi_atomic_lock( &ortl->lock );
  size_ws = kaapi_onereadytasklist_size( ortl );
  if( size_ws == 0 ) {
    kaapi_atomic_unlock( &ortl->lock );
    return 1;
  }

  curr = ortl->tail;

  curr = kaapi_default_param.steal_by_affinity( thiefprocessor, curr );

  if (curr ==0) 
  {
    kaapi_atomic_unlock( &ortl->lock );
    return EBUSY;
  }
  
  /* unlink curr */
  if( curr->prev != 0 )
    curr->prev->next = curr->next;
  else
    ortl->head = curr->next;
  if (curr ->next !=0)
    curr->next->prev = curr->prev;
  else
    ortl->tail = curr->prev;
  ortl->size--;
  kaapi_atomic_unlock( &ortl->lock );
  curr->prev = curr->next = 0;
  *td = curr;
  return 0;
}

int kaapi_readylist_steal( const kaapi_processor_t* thiefprocessor, kaapi_readytasklist_t* rtl, kaapi_taskdescr_t** td ) 
{
  kaapi_onereadytasklist_t* onertl;
  int err;
  int prio;
  int max_prio= 0, min_prio= 0, inc_prio= 0;
  
  if( KAAPI_ATOMIC_READ( &rtl->cnt_tasks) == 0 )
    return 1;
  kaapi_readylist_get_priority_range( &min_prio, &max_prio, &inc_prio );
  for( prio = max_prio; prio != min_prio; prio += inc_prio ) 
  {
    onertl = &rtl->prl[prio];
    err = kaapi_onereadytasklist_steal( thiefprocessor, onertl, td );
    if( err == 0 )
    {
	    KAAPI_ATOMIC_DECR( &rtl->cnt_tasks );
	    return 0;
    }
  }
  return 1;
}
