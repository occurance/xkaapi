/*
** kaapi_error.h
** xkaapi
** 
** Created on Tue Mar 31 15:19:09 2009
** Copyright 2009 INRIA.
**
** Contributors :
**
** christophe.laferriere@imag.fr
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
#ifndef _KAAPI_ERROR_H
#define _KAAPI_ERROR_H 1

#include <stdlib.h> /* at least abort */

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(NDEBUG)
#  define kaapi_assert( cond ) if (!(cond)) abort();
#  define kaapi_assert_debug( cond )
#  define KAAPI_NDEBUG 1
#else
#  include <errno.h>
#  include <stdio.h>
#  define kaapi_assert( cond ) if (!(cond)) { printf("Bad assertion, line:%i, file:'%s'\n", __LINE__, __FILE__ ); abort(); }
#  define kaapi_assert_debug( cond ) if (!(cond)) { printf("Bad assertion, line:%i, file:'%s'\n", __LINE__, __FILE__ ); abort(); }
#endif

/** Highest level, more trace generated */
#define KAAPI_LOG_LEVEL 10

#if defined(KAAPI_DEBUG)
#  define kaapi_assert_debug_m(cond, msg) \
      { int __kaapi_cond = cond; \
        if (!__kaapi_cond) \
        { \
          printf("[%s]: LINE: %u FILE: %s, ", msg, __LINE__, __FILE__);\
          abort();\
        }\
      }
#  define KAAPI_LOG(l, fmt, ...) \
      do { if (l<= KAAPI_LOG_LEVEL) { printf("%i:"fmt, kaapi_get_current_processor()->kid, ##__VA_ARGS__); fflush(0); } } while (0)

#  define KAAPI_DEBUG_INST(inst) inst
#else
#  define kaapi_assert_debug_m(cond, msg)
#  define KAAPI_LOG(l, fmt, ...) 
#  define KAAPI_DEBUG_INST(inst)
#endif /* defined(KAAPI_DEBUG)*/

#define kaapi_assert_m(cond, msg) \
      { \
        if (!(cond)) \
        { \
          printf("[%s]: \n\tLINE: %u FILE: %s, ", msg, __LINE__, __FILE__);\
          abort();\
        }\
      }

#if defined(__cplusplus)
}
#endif

#endif
