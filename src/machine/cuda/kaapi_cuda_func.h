/*
** kaapi_cuda_func.h
** xkaapi
** 
**
** Copyright 2010 INRIA.
**
** Contributors :
**
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


#ifndef KAAPI_CUDA_FUNC_H_INCLUDED
# define KAAPI_CUDA_FUNC_H_INCLUDED


#include <cuda.h>


typedef struct kaapi_cuda_func
{
  CUmodule mod;
  CUfunction fu;
  int off;
} kaapi_cuda_func_t;


typedef struct kaapi_cuda_dim2
{
  unsigned int x;
  unsigned int y;
} kaapi_cuda_dim2_t;


typedef struct kaapi_cuda_dim3
{
  unsigned int x;
  unsigned int y;
  unsigned int z;
} kaapi_cuda_dim3_t;


int kaapi_cuda_func_init(kaapi_cuda_func_t*);
int kaapi_cuda_func_load_ptx(kaapi_cuda_func_t*, const char*, const char*);
int kaapi_cuda_func_unload_ptx(kaapi_cuda_func_t*);
int kaapi_cuda_func_push_ptr(kaapi_cuda_func_t*, CUdeviceptr);
int kaapi_cuda_func_push_uint(kaapi_cuda_func_t*, unsigned int);
int kaapi_cuda_func_push_float(kaapi_cuda_func_t*, float);
int kaapi_cuda_func_call_async
(kaapi_cuda_func_t*, CUstream, const kaapi_cuda_dim2_t*, const kaapi_cuda_dim3_t*);
int kaapi_cuda_func_wait(kaapi_cuda_func_t*, CUstream);


#endif /* ! KAAPI_CUDA_FUNC_H_INCLUDED */
