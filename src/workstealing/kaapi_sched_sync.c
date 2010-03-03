/*
** kaapi_sched_sync.c
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


/** kaapi_sched_sync
    * pc is the current running task, exec all tasks from [pc-1,...,sp[
    * do not restore sp_data because data may have scope of the caller
    of kaapi_sched_sync.
*/
int kaapi_sched_sync(kaapi_stack_t* stack)
{
  int err;
  kaapi_task_t* savepc;
  kaapi_task_t* savesp;
  kaapi_task_t* savesavedsp;

  if (kaapi_stack_isempty( stack ) ) return 0;

#if 0
  /* look for retn */
  while ((kaapi_task_getbody(pc) != kaapi_retn_body) && (pc != stack->sp)) 
    --pc;
#endif

  savepc      = stack->pc;
  savesp      = stack->sp;
  savesavedsp = stack->saved_sp;
  stack->pc   = savesavedsp;

redo:
  err = kaapi_stack_execchild(stack, stack->pc);
  if (err == EWOULDBLOCK)
  {
    kaapi_sched_suspend( kaapi_get_current_processor() );
    goto redo;
  }
  if (err) /* but do not restore stack */
    return err;
  stack->pc = savepc;
  stack->sp = savesavedsp;       /* have executed all tasks from saved_sp to now */
  stack->saved_sp = savesavedsp;
  return err;
}
