/*
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
#include <iostream>
#include "kaapi++" // this is the new C++ interface for Kaapi

/* Sum two integers
 * this task reads a and b (read acces mode) and write their sum to res (write access mode)
 * it will wait until previous write to a and b are done
 * once finished, further read of res will be possible
 */
struct TaskSum : public ka::Task<3>::Signature<ka::W<long>, ka::R<long>, ka::R<long> > {};

template<>
struct TaskBodyCPU<TaskSum> //: public TaskSum
{
  void operator() ( ka::pointer_w<long> res, 
                    ka::pointer_r<long> a, 
                    ka::pointer_r<long> b ) 
  {
    /* write is used to write data to a Shared_w
     * read is used to read data from a Shared_r
     */
    *res = *a + *b;
  }
};


/* Kaapi Fibo task.
   A Task is a type with respect a given signature. The signature specifies the number of arguments (2),
   and the type and access mode for each parameters.
   Here the first parameter is declared with a write mode. The second is passed by value.
 */
struct TaskFibo : public ka::Task<2>::Signature<ka::W<long>, const long > {};


/* Implementation for CPU machine 
*/
template<>
struct TaskBodyCPU<TaskFibo> /* : public TaskFibo */ 
{
  void operator() ( ka::pointer_w<long> res, const long n )
  {  
    if (n < 2){ //cutoff) {
      *res = n; //fiboseq(n);
      return;
    }
    else {
      ka::auto_pointer<long> res1 = ka::Alloca<long>();
      ka::auto_pointer<long> res2 = ka::Alloca<long>();

      /* the Spawn keyword is used to spawn new task
       * new tasks are executed in parallel as long as dependencies are respected
       */
      ka::Spawn<TaskFibo>() ( res1, n-1 );
      ka::Spawn<TaskFibo>() ( res2, n-2 );

      /* the Sum task depends on res1 and res2 which are written by previous tasks
       * it must wait until thoses tasks are finished
       */
      ka::Spawn<TaskSum>() ( res, res1, res2 );
    }
  }
};


/* Main of the program
*/
struct doit {

  void do_experiment(unsigned int n, unsigned int iter )
  {
    long* res_value = ka::Alloca<long>(1);
    ka::pointer<long> res = res_value;
    for (cutoff=2; cutoff<3; ++cutoff)
    {
      ka::Spawn<TaskFibo>()( res, n );
      /* */
      ka::Sync();
      start_time= ka::WallTimer::gettime();
      for (unsigned int i = 0 ; i < iter ; ++i)
      {   
        ka::Spawn<TaskFibo>()( res, n );
        /* */
        ka::Sync();
      }
      stop_time= ka::WallTimer::gettime();

      /*  ka::System::getRank() prints out the id of the node executing the task */
      ka::logfile() << ka::System::getRank() << ": -----------------------------------------" << std::endl;
      ka::logfile() << ka::System::getRank() << ": Res  = " << *res_value << std::endl;
      ka::logfile() << ka::System::getRank() << ": Time(s): " << (stop_time-start_time)/iter << std::endl;
      ka::logfile() << ka::System::getRank() << ": -----------------------------------------" << std::endl;
    }
  }

  void operator()(int argc, char** argv )
  {
    unsigned int n = 30;
    if (argc > 1) n = atoi(argv[1]);
    unsigned int iter = 1;
    if (argc > 2) iter = atoi(argv[2]);
    cutoff = 2;
    if (argc > 3) cutoff = atoi(argv[3]);
    
    ka::logfile() << "In main: n = " << n << ", iter = " << iter << ", cutoff = " << cutoff << std::endl;
    do_experiment( n, iter );
  }
};


/* main entry point : Kaapi initialization
*/
int main(int argc, char** argv)
{
  try {
    /* Join the initial group of computation : it is defining
       when launching the program by a1run.
    */
    ka::Community com = ka::System::join_community( argc, argv );
    
    /* Start computation by forking the main task */
    ka::SpawnMain<doit>()(argc, argv); 
    
    /* Leave the community: at return to this call no more athapascan
       tasks or shared could be created.
    */
    com.leave();

    /* */
    ka::System::terminate();
  }
  catch (const ka::InvalidArgumentError& E) {
    ka::logfile() << "Catch invalid arg" << std::endl;
  }
  catch (const ka::BadAlloc& E) {
    ka::logfile() << "Catch bad alloc" << std::endl;
  }
  catch (const ka::Exception& E) {
    ka::logfile() << "Catch : "; E.print(std::cout); std::cout << std::endl;
  }
  catch (...) {
    ka::logfile() << "Catch unknown exception: " << std::endl;
  }
  
  return 0;
}

