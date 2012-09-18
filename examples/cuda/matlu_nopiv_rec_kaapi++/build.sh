#!/bin/bash

#SCRATCH=/scratch/jvlima
XKAAPIDIR=$HOME/install/xkaapi/default

CXX=g++

CUBLAS_CFLAGS="-DCONFIG_USE_CUBLAS=1"
CUBLAS_LDFLAGS="-lcublas"

CUDA_CFLAGS="-DCONFIG_USE_CUDA=1 $CUDA_CFLAGS"

CFLAGS="-DKAAPI_DEBUG=0 -DKAAPI_NDEBUG=1 
-DCONFIG_USE_DOUBLE=1 -I$XKAAPIDIR/include"
LDFLAGS="-L$XKAAPIDIR/lib -lkaapi -lkaapi++"

#MAGMA_CFLAGS="-DCONFIG_USE_MAGMA=1 $MAGMA_CFLAGS"

$CXX -g -Wall \
    $CFLAGS \
    $CUDA_CFLAGS \
    $CBLAS_CFLAGS \
    $CUBLAS_CFLAGS \
    $LAPACK_CLAGS \
    $LAPACKE_CFLAGS \
    $MAGMA_CFLAGS \
    -c matlu_nopiv_rec_kaapi++.cpp 


$CXX -g -Wall \
    -o matlu_nopiv_rec_kaapi++ \
    matlu_nopiv_rec_kaapi++.o \
    $LDFLAGS \
    $MAGMA_LDFLAGS \
    $CUDA_LDFLAGS \
    $CUBLAS_LDFLAGS \
    $LAPACKE_LDFLAGS \
    $LAPACK_LDFLAGS \
    $CBLAS_LDFLAGS 
