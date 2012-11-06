/**
 *
 * @file core_cublas.h
 *
 *  PLASMA core_blas kernel for CUBLAS
 * (c) INRIA
 *
 * @version 1.0
 * @author Thierry Gautier
 *
 **/
#ifndef _CORE_CUBLAS_H_
#define _CORE_CUBLAS_H_

#include <cuda_runtime_api.h>
#include <cublas_v2.h>

#include "plasma.h"
#include "core_blas.h"
#include "core_dcublas.h"

#include "kaapi.h"

#ifdef __cplusplus
extern "C" {
#endif
  
extern cublasHandle_t kaapi_cuda_cublas_handle( void );

/***************************************************************************
 /* Helper functions */
static inline cublasOperation_t PLASMA_CUBLAS_convertToOp( PLASMA_enum trans ) 
{
  switch(trans) 
  {
    case PlasmaNoTrans:
      return CUBLAS_OP_N;
    case PlasmaTrans:
      return CUBLAS_OP_T;
    case PlasmaConjTrans:
      return CUBLAS_OP_C;                        
    default:
      return -1;
  }
}

static inline cublasFillMode_t PLASMA_CUBLAS_convertToFillMode( PLASMA_enum uplo ) 
{
  switch (uplo) 
  {
    case PlasmaUpper:
      return CUBLAS_FILL_MODE_UPPER;
    case PlasmaLower:
      return CUBLAS_FILL_MODE_LOWER;
    default:
      return -1;
  }        
}

static inline cublasDiagType_t PLASMA_CUBLAS_convertToDiagType( PLASMA_enum diag ) 
{
  switch (diag) 
  {
    case PlasmaUnit:
      return CUBLAS_DIAG_UNIT;
    case PlasmaNonUnit:
      return CUBLAS_DIAG_NON_UNIT;
    default:
      return -1;
  }    
}

static inline cublasSideMode_t PLASMA_CUBLAS_convertToSideMode( PLASMA_enum side ) 
{
  switch (side) {
    case PlasmaRight:
      return CUBLAS_SIDE_RIGHT;
    case PlasmaLeft:
      return CUBLAS_SIDE_LEFT;
    default:
      return -1;
  } 
}

#define PLASMA_CUBLAS_ASSERT(status) \
  if (CUBLAS_STATUS_SUCCESS != status) {\
  fprintf(stderr,"%s:: CUBLAS Error:%i\n",__FUNCTION__,status);\
  }
  
#ifdef __cplusplus
}
#endif

#endif /* _CORE_CUBLAS_H_ */
