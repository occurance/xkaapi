/*
** xkaapi
** fibonacci with spawn
** 
**
** Copyright 2009,2010,2011,2012 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** marie.durand@inria.fr
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

#include <kaapic.h>
#include <stdio.h>
#include <stdlib.h>
#include "fib_verify.h"

void fibonacci(const int n, int* result)
{
  /* task user specific code */
  if (n<2)
    *result = n;
  else
  {
    int err;
    int result1 = 0;
    int result2 = 0;
    err = kaapic_spawn(0, 2, fibonacci, 
        KAAPIC_MODE_V, KAAPIC_TYPE_INT, 1, n-1,
        KAAPIC_MODE_W, KAAPIC_TYPE_INT, 1, &result1
    );
    kaapi_assert(err==0);

    err = kaapic_spawn(0, 2, fibonacci, 
        KAAPIC_MODE_V, KAAPIC_TYPE_INT, 1, n-2,
        KAAPIC_MODE_W, KAAPIC_TYPE_INT, 1, &result2
    );
    kaapi_assert(err==0);

    kaapic_sync();
    *result = result1 + result2;
  }
}

int main(int argc, char *argv[])
{
  int n = 30;
  int result = 0;

  int err = kaapic_init( KAAPIC_START_ONLY_MAIN );
  kaapi_assert(err == 0);

  if (argc > 1)
  {
    char *e;
    n = strtol(argv[1], &e,0);
  }

  /* example of attribut for spawn, not used */
  kaapic_spawn_attr_t attr;
  err = kaapic_spawn_attr_init(&attr);
  kaapi_assert(err == 0);

  err = kaapic_begin_parallel (KAAPIC_FLAG_DEFAULT);
  kaapi_assert(err == 0);

  double start = kaapic_get_time();

  err = kaapic_spawn(&attr, 2, fibonacci, 
      KAAPIC_MODE_V, KAAPIC_TYPE_INT, 1, n,
      KAAPIC_MODE_W, KAAPIC_TYPE_INT, 1, &result
  );
  kaapi_assert(err == 0);

  kaapic_sync();

  double stop = kaapic_get_time();

  err = kaapic_end_parallel (KAAPIC_FLAG_DEFAULT);
  kaapi_assert(err == 0);

  fprintf(stdout, "Fibo(%d) = %d\n", n, result);
  kaapi_assert( result == (int)fiboseq_verify(n) );

  fprintf(stdout, "Time : %f (s)\n", stop-start);

  err = kaapic_finalize();
  kaapi_assert(err == 0);
}
