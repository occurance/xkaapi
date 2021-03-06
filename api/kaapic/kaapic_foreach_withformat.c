/*
 ** xkaapi
 ** 
 ** Created on Tue Mar 31 15:19:14 2009
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
#include "kaapi_impl.h"
#include "kaapic_impl.h"
#include <stdarg.h>

/*
*/
void kaapic_foreach_with_format( 
  int32_t first, 
  int32_t last, 
  const kaapic_foreach_attr_t* attr,
  int32_t nargs, 
  ...
)
{
  int32_t k;
  kaapic_body_arg_t* body_arg = (kaapic_body_arg_t*)alloca(
    offsetof(kaapic_body_arg_t, args) + nargs * sizeof(void*)
  ); 
  va_list va_args;
  va_start(va_args, nargs);
  
  /* format of each effective parameter is a list of tuple:
       mode, @, count, type
  */
  body_arg->u.f_c = va_arg(va_args, kaapic_body_arg_f_c_t);
  body_arg->nargs = nargs;
  for (k = 0; k < nargs; ++k)
  {
    /* skip mode */
    va_arg(va_args, uintptr_t);

    body_arg->args[k] = va_arg(va_args, void*);

    /* skip count, type */
    va_arg(va_args, uintptr_t);
    va_arg(va_args, uintptr_t);
  }
  va_end(va_args);
  
  kaapic_foreach_common( first, last, attr, kaapic_foreach_body2user, body_arg);
}
