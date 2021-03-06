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
#include "libgomp.h"

/*
*/
bool GOMP_loop_ull_static_start (
          bool up, 
          unsigned long long start,
					unsigned long long end,
					unsigned long long incr,
					unsigned long long chunk_size,
					unsigned long long *istart,
					unsigned long long *iend
)
{
  return GOMP_loop_ull_dynamic_start(
      up,
      start,
      end,
      incr,
      chunk_size,
      istart,
      iend
  );
}

/*
*/
bool GOMP_loop_ull_guided_start (
          bool up, 
          unsigned long long start,
					unsigned long long end,
					unsigned long long incr,
					unsigned long long chunk_size,
					unsigned long long *istart,
					unsigned long long *iend
)
{
  return GOMP_loop_ull_dynamic_start(
      up,
      start,
      end,
      incr,
      chunk_size,
      istart,
      iend
  );
}

/*
*/
bool GOMP_loop_ull_runtime_start (
          bool up, 
          unsigned long long start,
					unsigned long long end,
					unsigned long long incr,
					unsigned long long *istart,
					unsigned long long *iend
)
{
  return GOMP_loop_ull_dynamic_start(
      up,
      start,
      end,
      incr,
      -1,
      istart,
      iend
  );
}

/*
*/
bool GOMP_loop_ull_ordered_static_start (
          bool up, 
          unsigned long long start,
					unsigned long long end,
					unsigned long long incr,
					unsigned long long chunk_size,
					unsigned long long *istart,
					unsigned long long *iend
)
{
  printf("%s not implemented\n", __PRETTY_FUNCTION__ ); fflush(stdout);
  return false;
}

/*
*/
bool GOMP_loop_ull_ordered_dynamic_start (
          bool up, 
          unsigned long long start,
					unsigned long long end,
					unsigned long long incr,
					unsigned long long chunk_size,
					unsigned long long *istart,
					unsigned long long *iend
)
{
  printf("%s not implemented\n", __PRETTY_FUNCTION__ ); fflush(stdout);
  return false;
}

/*
*/
bool GOMP_loop_ull_ordered_guided_start (
          bool up, 
          unsigned long long start,
					unsigned long long end,
					unsigned long long incr,
					unsigned long long chunk_size,
					unsigned long long *istart,
					unsigned long long *iend
)
{
  printf("%s not implemented\n", __PRETTY_FUNCTION__ ); fflush(stdout);
  return false;
}

/*
*/
bool GOMP_loop_ull_ordered_runtime_start (
          bool up, 
          unsigned long long start,
					unsigned long long end,
					unsigned long long incr,
					unsigned long long *istart,
					unsigned long long *iend
)
{
  printf("%s not implemented\n", __PRETTY_FUNCTION__ ); fflush(stdout);
  return false;
}                         

/*
*/
bool GOMP_loop_ull_static_next (
					unsigned long long *istart,
					unsigned long long *iend
)
{
  return GOMP_loop_ull_dynamic_next(istart, iend);
}

/*
*/
bool GOMP_loop_ull_guided_next (
					unsigned long long *istart,
					unsigned long long *iend
)
{
  return GOMP_loop_ull_dynamic_next(istart, iend);
}

/*
*/
bool GOMP_loop_ull_runtime_next(
					unsigned long long *istart,
					unsigned long long *iend
)
{
  return GOMP_loop_ull_dynamic_next(istart, iend);
}

/*
*/
bool GOMP_loop_ull_ordered_static_next (
					unsigned long long *istart,
					unsigned long long *iend
)
{
  printf("%s not implemented\n", __PRETTY_FUNCTION__ ); fflush(stdout);
  return false;
}

/*
*/
bool GOMP_loop_ull_ordered_dynamic_next (
					unsigned long long *istart,
					unsigned long long *iend
)
{
  printf("%s not implemented\n", __PRETTY_FUNCTION__ ); fflush(stdout);
  return false;
}

/*
*/
bool GOMP_loop_ull_ordered_guided_next (
					unsigned long long *istart,
					unsigned long long *iend
)
{
  printf("%s not implemented\n", __PRETTY_FUNCTION__ ); fflush(stdout);
  return false;
}

/*
*/
bool GOMP_loop_ull_ordered_runtime_next (
					unsigned long long *istart,
					unsigned long long *iend
)
{
  printf("%s not implemented\n", __PRETTY_FUNCTION__ ); fflush(stdout);
  return false;
}
