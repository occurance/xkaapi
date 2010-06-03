/*
 ** xkaapi
 ** 
 ** Created on Tue Mar 31 15:19:14 2009
 ** Copyright 2009 INRIA.
 **
 ** Contributors :
 **
 ** thierry.gautier@inrialpes.fr
 ** fabien.lementec@gmail.com / fabien.lementec@imag.fr
 
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


#ifndef KASTL_COUNT_H_INCLUDED
# define KASTL_COUNT_H_INCLUDED


#include <iterator>
#include "kastl_loop.h"
#include "kastl_sequences.h"


namespace kastl
{

template<typename Iterator, typename Value, typename Size>
struct count_body
{
  typedef kastl::impl::numeric_result<Iterator, Size> result_type;

  const Value& _value;

  count_body(const Value& value)
    : _value(value)
  {}

  bool operator()(result_type& res, const Iterator& pos)
  {
    if (*pos == _value)
      ++res._value;
    return false;
  }

  bool reduce(result_type& lhs, const result_type& rhs)
  {
    lhs._value += rhs._value;
    return false;
  }
};

template<typename Iterator, typename Value, typename Size, typename Settings>
void count
(Iterator first, Iterator last,
 const Value& value, Size& size,
 const Settings& settings)
{
  kastl::rts::Sequence<Iterator> seq(first, last - first);
  kastl::impl::numeric_result<Iterator, Size> res(size);
  count_body<Iterator, Value, Size> body(value);
  kastl::impl::reduce_loop::run(res, seq, body, settings);
  size = res._value;
}

template<typename Iterator, typename Value, typename Size>
void count(Iterator first, Iterator last, const Value& value, Size& size)
{
  kastl::impl::static_settings settings(512, 512);
  return kastl::count(first, last, value, size, settings);
}

template<typename Iterator, typename Value>
typename std::iterator_traits<Iterator>::difference_type
count(Iterator first, Iterator last, const Value& value)
{
  typename std::iterator_traits<Iterator>::difference_type size = 0;
  kastl::count(first, last, value, size);
  return size;
}

} // kastl::


#endif // ! KASTL_COUNT_H_INCLUDED
