/*
** xkaapi
** 
** Copyright 2011 INRIA.
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
#include <stdlib.h>
#include <math.h>

#define THRESHOLD 8

#pragma kaapi task write([begin:end)) value (op)
void for_each( double* begin, double* end, void (*op)(double*) )
{
   size_t size = (end-begin);
  if (size <THRESHOLD)
  {
    while (begin != end)
      op(begin++);
  }
  else {
    /* simple recursive for_each */
    size_t med = size/2;
    for_each( begin, begin+med, op);
    for_each( begin+med, end, op);
  }
}


/**
 */
static void apply_cos( double* v )
{
  *v += cos(*v);
}

/**
 */
int main(int ac, char** av)
{
  double sum = 0.f;
  size_t i;
  size_t iter;
  
#define ITEM_COUNT 100000
  static double array[ITEM_COUNT];
  
#pragma kaapi parallel  
  for_each( array, array+ITEM_COUNT, apply_cos );

  return 0;
}
