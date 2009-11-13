/*
** kaapi_thread_join.c
** xkaapi
** 
** Created on Tue Mar 31 15:16:38 2009
** Copyright 2009 INRIA.
**
** Contributors :
**
** christophe.laferriere@imag.fr
** thierry.gautier@imag.fr
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


int kaapi_join(kaapi_t thread, void **value_ptr)
{
  kaapi_thread_futur_t* futur;
  kaapi_thread_descr_t* self   = kaapi_self_internal();
  
  kaapi_mutex_t mutex;
  kaapi_mutex_init(&mutex, 0);
  
  if ((self->scope == KAAPI_SYSTEM_SCOPE)    && (self->th.s.tid == thread.tid)) return EDEADLK;
  if ((self->scope == KAAPI_PROCESSOR_SCOPE) && (self->th.k.ctxt.tid == thread.tid)) return EDEADLK;

  futur = thread.futur;
  
  if (futur ==0) return EINVAL; /* thread created in detachable state */
  if ( (futur->state & KAAPI_FUTUR_MASK_DETACHED) == KAAPI_FUTUR_S_DETACHED) return EINVAL;
  
  kaapi_mutex_lock( &mutex );
  while ( (futur->state & KAAPI_FUTUR_MASK_TERMINATED) != KAAPI_FUTUR_S_TERMINATED)
  {
    kaapi_cond_wait( &futur->condition, &mutex );
  }
  kaapi_mutex_unlock( &mutex );
  
  kaapi_mutex_destroy(&mutex);

  kaapi_readmem_barrier();

  *value_ptr = futur->result;

  return 0;
}
