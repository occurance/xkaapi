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
#include <iostream>
#include "kaapi++" // this is the new C++ interface for Kaapi
#include "kanet_network.h" // this is the new C++ interface for Kaapi
#include <string.h>

/*
   xkaapi config: 
   ../xkaapi/configure  --prefix=/Users/thierry/Developpment/KAAPI/kaapi/install/xkaapi --enable-mode=debug --disable-static --with-gasnet=/usr/local/gasnet/include/udp-conduit/udp-par.mak --enable-network
  runing
> KAAPI_NETWORK=gasnet SSH_SERVERS=localhost,localhost ./ping_xx 2

*/
/*
*/
volatile int isrecv = 0;
static void service(int err, ka::GlobalId source, void* buffer, size_t sz_buffer )
{
  char* msg = (char*)buffer;
  ka::logfile() << ": local_gid:" << ka::System::local_gid << ", receive msg= '" << msg << "'" << std::endl;
  isrecv = 1;
}
enum {
  service_id = 128
};


/* main entry point : Kaapi initialization
*/
int main(int argc, char** argv)
{
  try {
    ka::Network::register_service( service_id,       &service );

    /* Join the initial group of computation : it is defining
       when launching the program by a1run.
    */
    ka::System::join_community( argc, argv );
    
    /*
    */
    ka::Network::object.dump_info();
    std::cout << "My local_gid is:" << ka::System::local_gid << std::endl << std::flush;    

    if (ka::System::local_gid == 0)
    {
      for (unsigned int i=1; i<ka::Network::object.size(); ++i)
      {
        ka::OutChannel* channel = ka::Network::object.get_default_local_route(i);
        kaapi_assert(channel != 0);
        const char* msg = "Ceci est un message de 0";
      
        channel->insert_am( service_id, msg, strlen(msg)+1 );
        channel->sync();
      }
    }
    else {
      while (isrecv ==0) 
        ka::Network::object.poll();
    }
    ka::Network::object.dump_info();

    /*
    */
    ka::Network::object.terminate();

    /* */
    ka::System::terminate();
  }
  catch (const std::exception& E) {
    ka::logfile() << "Catch : " << E.what() << std::endl;
  }
  catch (...) {
    ka::logfile() << "Catch unknown exception: " << std::endl;
  }
  
  return 0;
}
