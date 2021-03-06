/*
** xkaapi
** 
**
** Copyright 2012 INRIA.
**
** Contributors :
**
** vincent.danjean@imag.fr
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
#ifndef _KAAPI_LOCK_INTERNAL_H_
#define _KAAPI_LOCK_INTERNAL_H_

#include "kaapi_impl.h"

/* Simple lock: map omp_lock_t to the atomic type in kaapi...
*/
typedef kaapi_atomic_t omp_lock_t;
typedef struct {
	omp_lock_t lock;
	int count;
	void* owner;
} omp_nest_lock_t;

/* Warning: as "omp.h" system header does not defined the following types
 * we can not automatically check they are compatible
 */
typedef kaapi_atomic_t omp_lock_25_t;
typedef struct {
	int owner;
	int count;
} omp_nest_lock_25_t;

#endif
