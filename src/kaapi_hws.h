/*
** kaapi_hws.h
** xkaapi
** 
**
** Copyright 2009 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** fabien.lementec@gmail.com / fabien.lementec@imag.fr
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
#ifndef KAAPI_HWS_H_INCLUDED
# define KAAPI_HWS_H_INCLUDED

/* internal to hws */

#define CONFIG_HWS_COUNTERS 1

/** TODO: DESCRIPTION !!!
*/
typedef enum kaapi_ws_error
{
  KAAPI_WS_ERROR_SUCCESS = 0,
  KAAPI_WS_ERROR_EMPTY,
  KAAPI_WS_ERROR_FAILURE
} kaapi_ws_error_t;

#include "kaapi_ws_queue.h"


/* workstealing lock, cas implementation only */
/* todo: replace with kaapi_sched_lock */
/* TG: base lock on top of kaapi_atomic_lock_XX subroutines */

typedef kaapi_atomic_t kaapi_ws_lock_t;

static inline int kaapi_ws_lock_init(kaapi_ws_lock_t* lock)
{
  return kaapi_atomic_initlock(lock);
}

static inline int kaapi_ws_lock_trylock(kaapi_ws_lock_t* lock)
{
  return kaapi_atomic_trylock(lock);
}

static inline int kaapi_ws_lock_lock(kaapi_ws_lock_t* lock)
{
  return kaapi_atomic_lock(lock);
}

static inline int kaapi_ws_lock_unlock(kaapi_ws_lock_t* lock)
{
  return kaapi_atomic_unlock(lock);
}


/* Workstealing block.
   A workstealing block is a node in the hierarchy used to aggregate
   requestion from sub-nodes. It contains :
   - a lock used to aggregate work stealing requests
   - a kid_mask to iterate over the right set of bit in the bitmap of posted requests.
   - a queue where to push/pop or steal work
   - and the list of participating k-processors.
*/
typedef struct kaapi_ws_block
{
  /* concurrent workstealing sync */
  
  /* todo: cache aligned, alone in the line */
  kaapi_ws_lock_t lock;

  /* workstealing queue */
  struct kaapi_ws_queue* queue;

  /* kid map of all the participants */
  kaapi_processor_id_t* kids;
  unsigned int kid_count;
  
  /* kid mask, for request iterator update operation */
  kaapi_bitmap_value_t kid_mask;

} kaapi_ws_block_t;

typedef struct kaapi_hws_level
{
  /* a hws_level contains all the node at a same level */

  /* block for a given kid at this level */
  kaapi_ws_block_t** kid_to_block;

  /* one block per node */
  kaapi_ws_block_t* blocks;
  unsigned int block_count;

} kaapi_hws_level_t;


/* globals */

static const int hws_level_count = (int)KAAPI_HWS_LEVELID_MAX;
extern kaapi_hws_level_t* hws_levels;
extern kaapi_hws_levelmask_t hws_levelmask;
extern kaapi_listrequest_t hws_requests;


/* internal exported functions */
extern const char* kaapi_hws_levelid_to_str(kaapi_hws_levelid_t);

static inline 
unsigned int kaapi_hws_is_levelid_set(
    kaapi_hws_levelid_t levelid
)
{
  return hws_levelmask & (1 << levelid);
}


#if CONFIG_HWS_COUNTERS

#include <stdint.h>

#define container_of(__ptr, __type, __member) \
  (__type*)((uintptr_t)(__ptr) - offsetof(__type, __member))

static inline kaapi_ws_queue_t* __p_to_queue(void* p)
{
  return container_of(p, kaapi_ws_queue_t, data);
}

static inline void kaapi_hws_inc_pop_counter(void* p)
{
  /* increment the counter associated to the block containing a queue */
  kaapi_ws_queue_t* const q = __p_to_queue(p);
  KAAPI_ATOMIC_INCR(&q->pop_counter);
}

static inline void kaapi_hws_inc_steal_counter
(void* p, kaapi_processor_id_t kid)
{
  /* increment the counter associated to the block containing a queue */
  kaapi_ws_queue_t* const q = __p_to_queue(p);
  KAAPI_ATOMIC_INCR(&q->steal_counters[kid]);
}

#endif /* CONFIG_HWS_COUNTERS */


/* toremove */
extern void kaapi_hws_sched_init_sync(void);
extern void kaapi_hws_sched_inc_sync(void);
extern void kaapi_hws_sched_dec_sync(void);
/* toremove */


#endif /* ! KAAPI_HWS_H_INCLUDED */