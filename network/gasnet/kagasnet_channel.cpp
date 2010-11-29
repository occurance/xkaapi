/*
** xkaapi
** 
** Copyright 2010 INRIA.
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
#include "kagasnet_channel.h"

namespace GASNET {

// -----------------------------------------------------------------------
int OutChannel::initialize( int dest) throw()
{
  ka::OutChannel::initialize();
  _dest = dest;
  return 0;
}


// -----------------------------------------------------------------------
int OutChannel::terminate() throw()
{
  return 0;
}


// -----------------------------------------------------------------------
void OutChannel::flush( ka::Instruction* first, ka::Instruction* last )
{
  GASNET_BEGIN_FUNCTION();
  /* */

//  ka::logfile() << "In " << __PRETTY_FUNCTION__ << " " << last-first << std::endl;
  std::cout << ka::System::local_gid << "::[OutChannel::flush] begin" << std::endl << std::flush;    
  ka::Instruction* curr = first;
  int err;
  while (curr != last)
  {
    switch (curr->type) {
      case ka::Instruction::INST_VOID:
        break;

      case ka::Instruction::INST_NOP:
        if (curr->i_cbk.cbk !=0)
          (*curr->i_cbk.cbk)(0, this, curr->i_cbk.arg);
        break;

      case ka::Instruction::INST_WB:
        if (curr->i_cbk.cbk !=0)
          (*curr->i_cbk.cbk)(0, this, curr->i_cbk.arg);
        break;

      case ka::Instruction::INST_AM:
      {
        /* send header: first 2 fields */
        kaapi_uintptr_t handler = curr->i_am.handler;
        gasnet_handlerarg_t handlerH = handler >> 32UL;
        gasnet_handlerarg_t handlerL = handler & 4294967295UL;
        std::cout << ka::System::local_gid << "::[OutChannel::flush] send AM to:" << _dest << ", size:" << curr->i_am.size 
                  << ", service:(" << handlerH << "," << handlerL << ")=" << (void*)curr->i_am.handler << std::endl << std::flush;
        err = gasnet_AMRequestMedium2( _dest, kaapi_gasnet_service_call_id, 
            (void*)curr->i_am.lptr, curr->i_am.size,
            handlerH, handlerL
        );
        kaapi_assert(err == GASNET_OK);

        if (curr->i_cbk.cbk !=0)
          (*curr->i_cbk.cbk)(err, this, curr->i_cbk.arg);
      } break;

      case ka::Instruction::INST_RWDMA:
        std::cout << ka::System::local_gid << "::[OutChannel::flush] send RDMA to:" << _dest << ", size:" << curr->i_rw.size << std::endl << std::flush;    
        /* send header: first 2 fields */
        gasnet_put_bulk( _dest, (void*)curr->i_rw.dptr, (void*)curr->i_rw.lptr, curr->i_rw.size );

        if (curr->i_cbk.cbk !=0)
          (*curr->i_cbk.cbk)(err, this, curr->i_cbk.arg);
      break;
    };
    ++curr;
  }
//  ka::logfile() << "In " << __PRETTY_FUNCTION__ << std::endl;
  return;
}

} // -namespace
