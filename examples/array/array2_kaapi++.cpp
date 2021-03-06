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
#include <stdlib.h>
#include "kaapi++" // this is the new C++ interface for Kaapi

/* Task Init
 * This task initializes each entries of the array to 1
 * The task declares an write access pointer to an array object.
 */
struct TaskInit : public ka::Task<2>::Signature<
  ka::W<int>,
  ka::W<ka::array<1,int> > 
> {};

template<>
struct TaskBodyCPU<TaskInit> {
  void operator() ( ka::pointer_w<int> dummy, ka::pointer_w<ka::array<1,int> > array )
  {
    size_t sz = array.size();
    std::cout << "In TaslInit/CPU, size of array = " << sz << std::endl;
    for (size_t i=0; i < sz; ++i)
      array[i] = 1;
  }
};



/* Task Accumulate
 * This task computes the sum of the entries of an array 
 * The task declares an object with CW access to accumulate the result and read pointer to an array object.
 * The task add 2 dummy formel argument to defines right dependencies between sub array computation
 */
struct TaskAccumulate : public ka::Task<4>::Signature<
  ka::CW<int>, 
  ka::R<int>,
  ka::R<int>,
  ka::R<ka::array<1,int> > 
> {};

template<>
struct TaskBodyCPU<TaskAccumulate> {
  void operator() ( ka::pointer_cw<int> acc, 
                    ka::pointer_r<int> d1, ka::pointer_r<int> d2, 
                    ka::pointer_r<ka::array<1,int> > array  )
  {
    size_t sz = array.size();
    for (size_t i=0; i < sz; ++i)
      *acc += array[i];
  }
};


/* Main task of the program
*/
struct doit {
  void operator()(int argc, char** argv )
  {
    int n= 10;
    if (argc >1) n = atoi(argv[1]);
    int med = n/2;
    if (med ==0) { n = 2; med = 1; }

    int* data = new int[n];

    /* form a view of data as an 1-dimensional array */
    ka::array<1,int> arr(data, n); 
    int res = 0;
    int v1;
    int v2;

    /* be carrefull here: the array is equivalent as if each of its entries has
       been passed to the task (the formal parameter is array<1,W<int> >).
    */
    ka::Spawn<TaskInit>()( &v1, arr[ka::rangeindex(0,med)] );

    /* be carrefull here: the array is equivalent as if each of its entries has
       been passed to the task (the formal parameter is array<1,W<int> >).
    */
    ka::Spawn<TaskInit>()( &v2, arr[ka::rangeindex(med,n)] );


    /* Here the dependencies is accross each entries of the array arr,
       thus this task is ready iff the two previous tasks are finished
    */
    ka::Spawn<TaskAccumulate>()( &res, &v1, &v2, arr );
    ka::Sync();

    std::cout << "Res = " << res << std::endl;
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
  catch (const std::exception& E) {
    ka::logfile() << "Catch : " << E.what() << std::endl;
  }
  catch (...) {
    ka::logfile() << "Catch unknown exception: " << std::endl;
  }
  
  return 0;
}
