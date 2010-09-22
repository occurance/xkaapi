#include "kaapi_impl.h"
#include "kaapi++"
#include <iostream>


// --------------------------------------------------------------------
struct TaskW: public ka::Task<1>::Signature<ka::W<int> > {};
template<>
struct TaskBodyCPU<TaskW> {
  void operator() ( ka::pointer_w<int> d )
  {
    *d = 123;
    kaapi_processor_t* kproc = kaapi_get_current_processor();
    printf("[%p->%p] :: In Task W=123\n", kproc, kproc->thread);
    usleep(100);
  }
};
static ka::RegisterBodyCPU<TaskW> dummy_object_TaskW;

// --------------------------------------------------------------------
struct TaskR1: public ka::Task<1>::Signature<ka::R<int> > {};
template<>
struct TaskBodyCPU<TaskR1> {
  void operator() ( ka::Thread* thread, ka::pointer_r<int> d )
  {
    kaapi_processor_t* kproc = kaapi_get_current_processor();
    printf("[%p->%p] :: In Task R1=%i\n", kproc, kproc->thread, *d);
    usleep(100);
  }
};
static ka::RegisterBodyCPU<TaskR1> dummy_object_TaskR1;

// --------------------------------------------------------------------
struct TaskR2: public ka::Task<1>::Signature<ka::R<int> > {};
template<>
struct TaskBodyCPU<TaskR2> {
  void operator() ( ka::Thread* thread, ka::pointer_r<int> d )
  {
    kaapi_processor_t* kproc = kaapi_get_current_processor();
    printf("[%p->%p] :: In Task R2=%i\n", kproc, kproc->thread, *d);
    usleep(100);
  }
};
static ka::RegisterBodyCPU<TaskR2> dummy_object_TaskR2;


/* Main of the program
*/
struct doit {
  void operator()(int argc, char** argv )
  {
    std::cout << "My pid=" << getpid() << std::endl;

    ka::ThreadGroup threadgroup( 2 );
    ka::auto_pointer<int> a      = ka::Alloca<int>(1);
    *a = 1;

    threadgroup.begin_partition();

    threadgroup.Spawn<TaskW>  (ka::SetPartition(0))  ( a );
    threadgroup.Spawn<TaskR1> (ka::SetPartition(1))  ( a );
    threadgroup.Spawn<TaskR2> (ka::SetPartition(1))  ( a );

    threadgroup.print();    

    threadgroup.end_partition();

    threadgroup.execute();
  }
};


/*
*/
int main( int argc, char** argv ) 
{
  try {
    ka::Community com = ka::System::join_community( argc, argv );
    
    ka::SpawnMain<doit>()(argc, argv); 
          
    com.leave();

    ka::System::terminate();
  }
  catch (const ka::Exception& E) {
    ka::logfile() << "Catch : "; E.print(std::cout); std::cout << std::endl;
  }
  catch (...) {
    ka::logfile() << "Catch unknown exception: " << std::endl;
  }
  return 0;    
}
