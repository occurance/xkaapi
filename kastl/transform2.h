/*
 *  test_transform.cpp
 *  ckaapi
 *
 *  Created by TG on 18/02/09.
 *  Copyright 2009 INRIA. All rights reserved.
 *
 */
#ifndef _CKAAPI_TRANSFORM_H
#define _CKAAPI_TRANSFORM_H
#include "kaapi_adapt.h"
#include <algorithm>


template<class InputIterator, class OutputIterator, class UnaryOperator>
void transform ( kaapi_steal_context_t* stealcontext, InputIterator begin, InputIterator end, OutputIterator to_fill, UnaryOperator op );

/** Stucture of a work for transform
*/
template<class InputIterator, class OutputIterator, class UnaryOperator>
class TransformStruct {
public:
  /* cstor */
  TransformStruct(
    kaapi_steal_context_t* sc,
    InputIterator ibeg,
    InputIterator iend,
    OutputIterator obeg,
    UnaryOperator  op
  ) : _sc(sc), _ibeg(ibeg), _iend(iend), _obeg(obeg), _op(op) 
  {}
  
  /* do transform */
  void doit();

  typedef typename std::iterator_traits<InputIterator>::value_type value_type;

protected:  
  kaapi_steal_context_t* _sc;
  InputIterator  _ibeg;
  InputIterator  _iend;
  OutputIterator _obeg;
  UnaryOperator  _op;
  
  /* Entry in case of thief execution */
  static void thief_entrypoint(kaapi_steal_context_t* sc, void* data)
  {
    TransformStruct<InputIterator, OutputIterator, UnaryOperator>* w = (TransformStruct<InputIterator, OutputIterator, UnaryOperator>*)data;
    w->_sc = sc;
    w->doit();
  }

  /** splitter_work is called within the context of the steal point
  */
  static void splitter( kaapi_steal_context_t* stealcontext, int count, kaapi_steal_request_t** request, 
                        InputIterator ibeg, InputIterator& iend, OutputIterator obeg, UnaryOperator op
                      )
  {
    int i = 0;

    size_t size = (iend - ibeg);
    RandomAccessIterator middle = iend - (size/2);
    bool steal = false;

    TransformStruct<InputIterator, OutputIterator, UnaryOperator>* output_work =0;

    /* threshold should be defined (...) */
    if (size < 512) goto reply_failed;
    
    while (count >0)
    {
     
      if(steal) goto reply_failed;

      if (request[i] !=0)
      {
        if (kaapi_steal_context_alloc_result( stealcontext, 
                                              request[i], 
                                              (void**)&output_work, 
                                              sizeof(TransformStruct<InputIterator, OutputIterator, UnaryOperator>) 
                                            ) ==0)
        {
          steal = true;
          output_work->_iend = local_end;
          output_work->_ibeg = local_end-bloc;
          local_end -= bloc;
          output_work->_obeg = obeg + (output_work->_ibeg - ibeg);
          ckaapi_assert( output_work->_iend - output_work->_ibeg >0);
          output_work->_op   = op;

          /* reply ok (1) to the request */
          kaapi_request_reply( request[i], stealcontext, &thief_entrypoint, 1, CKAAPI_MASTER_FINALIZE_FLAG);
        }
        else {
          /* reply failed (=last 0 in parameter) to the request */
          kaapi_request_reply( request[i], stealcontext, 0, 0, CKAAPI_DEFAULT_FINALIZE_FLAG);
        }
        --count; 
      }
      ++i;
    }
  /* mute the end of input work of the victim */
  iend  = middle;
  ckaapi_assert( iend - ibeg >0);
  return;
      
reply_failed:
    while (count >0)
    {
      if (request[i] !=0)
      {
        /* reply failed (=last 0 in parameter) to the request */
        kaapi_request_reply( request[i], stealcontext, 0, 0, CKAAPI_DEFAULT_FINALIZE_FLAG);
        --count; 
      }
      ++i;
    }
  }


  /* Called by the victim thread to collect work from one other thread
  */
  static void reducer( kaapi_steal_context_t* sc, void* thief_data, TransformStruct<InputIterator, OutputIterator, UnaryOperator>* victim_work )
  {
    TransformStruct<InputIterator, OutputIterator, UnaryOperator>* thief_work = 
      (TransformStruct<InputIterator, OutputIterator, UnaryOperator>* )thief_data;
    if (thief_work->_ibeg != thief_work->_iend)
    {
#if defined(SEQ_SUBTRANSFORM)
      std::transform( thief_work->_ibeg, thief_work->_iend, thief_work->_obeg, thief_work->_op );
#else
      TransformStruct<InputIterator, OutputIterator, UnaryOperator> 
        work( sc, 
              thief_work->_ibeg, 
              thief_work->_iend, 
              thief_work->_obeg, 
              victim_work->_op);
      work.doit();
#endif
    }
  }
};


/** Adaptive transform
*/
template<class InputIterator, class OutputIterator, class UnaryOperator>
void TransformStruct<InputIterator,OutputIterator,UnaryOperator>::doit()
{
  /* local iterator for the nano loop */
  InputIterator nano_iend;
  
  /* amount of work per iteration of the nano loop */
  ptrdiff_t unit_size = 512;
  ptrdiff_t tmp_size = 0;

  while (_iend != _ibeg)
  {
    /* definition of the steal point where steal_work may be called in case of steal request 
       -here size is pass as parameter and updated in case of steal.
    */
    kaapi_stealpoint( _sc, splitter, _ibeg, _iend, _obeg, _op );

    tmp_size = _iend-_ibeg;
    if(tmp_size < unit_size ) {
       unit_size = tmp_size; nano_iend = _iend;
    } else {
       nano_iend = _ibeg + unit_size;
    }
    /* sequential computation*/
    _obeg=std::transform(_ibeg, nano_iend, _obeg, _op);
    _ibeg +=unit_size;
    
    //if (kaapi_preemptpoint( _sc, 0 )) return ;
  }

  /* definition of the finalization point where all stolen work a interrupt and collected */
  kaapi_finalize_steal( _sc, 0, (kaapi_reducer_function_t)&reducer, this );

  /* Here the thiefs have finish the computation and returns their values which have been reduced using reducer function. */  
}


/**
*/
template<class InputIterator, class OutputIterator, class UnaryOperator>
void transform ( kaapi_steal_context_t* stealcontext, InputIterator begin, InputIterator end, OutputIterator to_fill, UnaryOperator op )
{
  kaapi_steal_context_initpush( stealcontext );
  TransformStruct<InputIterator, OutputIterator, UnaryOperator> work( stealcontext, begin, end, to_fill, op);
  work.doit();

}
#endif
