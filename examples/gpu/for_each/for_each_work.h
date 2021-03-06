/*
** xkaapi
** 
**
** Copyright 2009,2010,2011,2012 INRIA.
**
** Contributors :
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
#ifndef FOR_EACH_WORK_H
#define FOR_EACH_WORK_H

#include "kaapi++"
#include <algorithm>
#include <string.h>
#include <math.h>


/** Description of the example.

    Overview of the execution.
    
    What is shown in this example.
    
    Next example(s) to read.
*/
template<typename T, typename OP>
class Work {
public:
  /* cstor */
  Work(T* beg, T* end, OP op)
  {
    /* initialize work */
    _op    = op;
    _array = beg;
    kaapi_workqueue_init(&wq, 0, end-beg);
  }
  
  /* extract sequential work */
  bool extract_seq( T*& beg, T*& end)
  {
#define CONFIG_SEQ_GRAIN 256
    kaapi_workqueue_index_t b, e;
    if (kaapi_workqueue_pop(&wq, &b, &e, CONFIG_SEQ_GRAIN)) return false;
//    printf("Pop: [%li, %li)\n", b, e);
//    fflush(stdout);
    beg = _array+b;
    end = _array+e;
    return true;
  }
  
  /* extract parallel work for nreq. Return the unit size */
  bool extract_par
  (int& nreq, T*& beg, T*& end, size_t& unit_size)
  {
#define GRID_DIM 32
#define BLOCK_DIM 256
#define CONFIG_PAR_GRAIN 4096
    kaapi_workqueue_index_t steal_size, i,j;
    kaapi_workqueue_index_t range_size = kaapi_workqueue_size(&wq);

    if (range_size <= CONFIG_PAR_GRAIN) return false;

    unit_size = range_size / (nreq + 1);
    if (unit_size == 0)
    {
      nreq = (range_size / CONFIG_PAR_GRAIN) - 1;
      unit_size = CONFIG_PAR_GRAIN;
    }

    if (nreq == 0) return false;

    // size must be rounded on a multiple of GRID_DIM * BLOCK_DIM
    // and distributed evenly amongst thieves

    unit_size -= unit_size % (GRID_DIM * BLOCK_DIM);

    if (unit_size == 0) return false;

    steal_size = unit_size * nreq;

    if (kaapi_workqueue_steal(&wq, &i, &j, steal_size))
      return false;

    beg = _array + i;
    end = _array + j;

    return true;
  }
  
  /* name of the method should be splitter !!! split work and reply to requests */
  void split (
    ka::StealContext* sc, 
    int nreq, 
    ka::Request* req
  );
  
  T* begin() { return _array + kaapi_workqueue_range_begin(&wq); }
  T* end()   { return _array + kaapi_workqueue_range_end(&wq); }
protected:
  OP _op;
  T* _array;
  kaapi_workqueue_t wq;
};


/** Task for the thief
    CPU implementation: see different implementations
*/
template<typename T, typename OP>
struct TaskThief : public ka::Task<2>::Signature<ka::RW<ka::range1d<T> >, OP> {};


/* name of the method should be splitter !!! split work and reply to requests */
template<typename T, typename OP>
void Work<T,OP>::split (
  ka::StealContext* sc, 
  int nreq, 
  ka::Request* req
)
{
  /* stolen range */
  T* beg_theft;
  T* end_theft;
  size_t unit_size;

  if (extract_par(nreq, beg_theft, end_theft, unit_size) == false)
    return;

  for (; nreq; --nreq, ++req, beg_theft += unit_size)
  {
    req->Spawn<TaskThief<T,OP> >(sc)
      (ka::array<1,T>(beg_theft, unit_size), _op);
  }
}

#endif
