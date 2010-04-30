#ifndef KASTL_FIND_HH_INCLUDED
# define KASTL_FIND_HH_INCLUDED



#include <algorithm>
#include <functional>
#include <iterator>
#include "kastl_impl.hh"


namespace kastl
{

namespace impl
{

template
<
  typename SequenceType,
  typename ConstantType,
  typename ResultType,
  typename MacroType,
  typename NanoType,
  typename SplitterType
>
class FindWork : public BaseWork
<SequenceType, ConstantType, ResultType, MacroType, NanoType, SplitterType>
{
  typedef FindWork
  <SequenceType, ConstantType, ResultType, MacroType, NanoType, SplitterType>
  SelfType;

  typedef BaseWork
  <SequenceType, ConstantType, ResultType, MacroType, NanoType, SplitterType>
  BaseType;

  inline void reduce_result(const ResultType& res)
  {
    this->_res = res;
    this->_is_done = true;
  }

public:

  FindWork() : BaseType() {}

  FindWork(const SequenceType& s, const ConstantType* c, const ResultType& r)
  : BaseType(s, c, r) {}

  inline void prepare()
  {
    this->_res = this->_seq._end;
  }

  inline void compute(const SequenceType& seq)
  {
    ResultType res = std::find(seq._beg, seq._end, *this->_const);
    if (res == seq._end)
      return ;

    reduce_result(res);
  }

  inline void reduce(const BaseType& tw)
  {
    if (tw._is_done == false)
      return ;

    reduce_result(tw._res);
  }

};

// tuning params

struct FindTuningParams : Daouda1TuningParams
{
  static const enum TuningTag macro_tag = TAG_LINEAR;
  static const size_t macro_min_size = 1024;
  static const size_t macro_max_size = 32768;
  static const size_t macro_step_size = 1024;
};

} // kastl::impl


template
<
  class InputIterator,
  class ValueType,
  class ParamType
>
InputIterator find
(
 InputIterator beg,
 InputIterator end,
 ValueType val
)
{
  typedef kastl::impl::BasicSequence<InputIterator>
    SequenceType;

  typedef typename kastl::impl::make_macro_type
    <ParamType::macro_tag, ParamType, SequenceType>::Type
    MacroType;

  typedef typename kastl::impl::make_nano_type
    <ParamType::nano_tag, ParamType, SequenceType>::Type
    NanoType;

  typedef typename kastl::impl::make_splitter_type
    <ParamType::splitter_tag, ParamType>::Type
    SplitterType;

  typedef ValueType ConstantType;

  typedef InputIterator ResultType;

  typedef kastl::impl::FindWork
    <SequenceType, ConstantType, ResultType, MacroType, NanoType, SplitterType>
    WorkType;

  WorkType work(SequenceType(beg, end), &val, end);
  kastl::impl::compute(work);
  return work._res;
}


template
<
  class InputIterator,
  class ValueType
>
InputIterator find
(
 InputIterator begin,
 InputIterator end,
 const ValueType& value
)
{
  typedef kastl::impl::FindTuningParams ParamType;

  return kastl::find
    <InputIterator, ValueType, ParamType>
    (begin, end, value);
}

} // kastl


#endif // ! KASTL_FIND_HH_INCLUDED