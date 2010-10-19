/*
** kaapi_task_reqreply.c
** xkaapi
** 
** Created on Tue Mar 31 15:18:04 2009
** Copyright 2009 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** fabien.lementec@imag.fr
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


/* adaptive task body
 */

typedef void (*athief_body_t)(void*, kaapi_thread_t*, kaapi_stealcontext_t*);

void kaapi_adapt_body(void* arg, kaapi_thread_t* thread)
{
  /* 2 cases to handle:
     . either we are an adaptive task created with
     kaapi_task_begin_adpative. the argument is a
     kaapi_taskadaptive task. This is the user role
     to ensure task finalization.
     . otherwise, we have been forked during a steal.
     the argument is a athief_taskarg and we retrieve
     the stealcontext since it is global to the thread
   */

  kaapi_stealcontext_t* const sc = (kaapi_stealcontext_t*)arg;

  /* this is a root adaptive task, do not execute body */
  if (sc->msc == sc)
    return ;

  /* todo: save the sp and sync if changed during
     the call (ie. wait for tasks forked)
  */

  /* execute the user task entrypoint */
  athief_body_t const body = (athief_body_t)sc->ubody;
  kaapi_assert_debug(body != NULL);
  body((void*)sc->udata, thread, sc);

  if (!(sc->msc->flag & KAAPI_SC_PREEMPTION))
  {
    /* non preemptive algorithm decrement the
       thievecount. this is the only way for
       the master to sync on algorithm term.
    */
    KAAPI_ATOMIC_DECR(&sc->msc->thievescount);
  }
  else if (ta->ktr != 0)
  {
    /* preemptive algorithms need to inform
       they are done so they can be reduced.
    */
    sc->ktr->thief_term = 1;
    sc->ktr->is_signaled = 1;
  }

  /* todo: kaapi_thread_restore_frame */

  kaapi_writemem_barrier();
}


/* common reply internal function
 */

#define KAAPI_REQUEST_REPLY_HEAD 0x0
#define KAAPI_REQUEST_REPLY_TAIL 0x1

static int request_reply
(kaapi_stealcontext_t* sc, kaapi_request_t* req, int headtail_flag)
{
  /* sc the stolen stealcontext */

  /* if there is preemption, link to thieves */
  if (sc->msc->flag & KAAPI_SC_PREEMPTION)
  {
    /* stolen task */
    kaapi_taskadaptive_result_t* const ktr = req->reply->sc.ktr;

    /* concurrence with preempt_thief */
    while (!KAAPI_ATOMIC_CAS(&sc->thieves.list.lock, 0, 1))
      kaapi_slowdown_cpu();

    /* insert in head or tail */
    if (sc->thieves.list.head == 0)
    {
      sc->thieves.list.tail = ktr;
      sc->thieves.list.head = ktr;
    }
    else if ((headtail_flag & 0x1) == KAAPI_REQUEST_REPLY_HEAD)
    { 
      ktr->next = sc->thieves.list.head;
      sc->thieves.list.head->prev = ktr;
      sc->thieves.list.head = ktr;
    } 
    else 
    {
      ktr->prev = sc->thieves.list.tail;
      sc->thieves.tail->next = ktr;
      sc->thieves.tail = ktr;
    }

    KAAPI_ATOMIC_WRITE(&sc->thieves.list.lock, 0);
  }
  else
  {
    /* non preemptive algorithm, inc the root master theifcount */
    KAAPI_ATOMIC_INCR(&sc->msc->thievescount);
  }

  return _kaapi_request_reply(req, KAAPI_REPLY_S_TASK);
}


/*
 */
void* kaapi_reply_init_adaptive_task
(
  kaapi_request_t*             req,
  kaapi_task_body_t            body,
  size_t		       size,
  kaapi_stealcontext_t*        vsc,
  kaapi_taskadaptive_result_t* ktr
)
{
  /* vsc the victim stealcontext */
  /* tsc the thief stealcontext */

  kaapi_reply_t* const rep = req->reply;
  kaapi_stealcontext_t* const tsc = &rep->sc;

  /* first part initialization. finalization
     is done in kaapi_sched_emitsteal. only
     the needed fields to avoid subsequent
     remote reads are initialized.
   */

  tsc->msc = vsc->msc;
  tsc->flag = vsc->msc->flag;
  tsc->ktr = ktr;

  /* initialize the reply
   */

  rep->data_size = size;
  rep->s_task.ubody = kaapi_adapt_body;
  rep->s_task.udata = req->reply->u.s_task.data;

  /* user put args in this area
   */

  return (void*)tsc->udata;
}


/*
*/
void kaapi_reply_pushhead_adaptive_task
(kaapi_stealcontext_t* sc, kaapi_request_t* req)
{
  /* sc the stolen stealcontext */
  request_reply(sc, req, KAAPI_REQUEST_REPLY_HEAD);
}


/*
*/
void kaapi_reply_pushtail_adaptive_task
(kaapi_stealcontext_t* sc, kaapi_request_t* req)
{
  /* sc the stolen stealcontext */
  request_reply(sc, req, KAAPI_REQUEST_REPLY_TAIL);
}



void kaapi_request_reply_failed(kaapi_request_t* req)
{
  /* sc the stolen stealcontext */
  _kaapi_request_reply(req, KAAPI_REPLY_S_NOK);
}
