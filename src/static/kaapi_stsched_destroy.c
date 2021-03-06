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
** threadctxts.
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

/**
*/
int kaapi_threadgroup_destroy(kaapi_threadgroup_t thgrp )
{
  int i;
  if (thgrp->state == KAAPI_THREAD_GROUP_EXEC_S) return EBUSY;

  if (thgrp->localgid == kaapi_threadgroup_tid2gid(thgrp,-1))
    /* reset stealing attribute on the main thread */
    thgrp->threadctxts[-1]->unstealable = 0;
    
  for (i=0; i<thgrp->group_size; ++i)
  {
    if (thgrp->localgid == kaapi_threadgroup_tid2gid(thgrp,i))
    {
      kaapi_context_free(thgrp->threadctxts[i]);
    }
  }

  --thgrp->threadctxts;
  free( thgrp->threadctxts );
  thgrp->threadctxts = 0;
  
  --thgrp->threads;
  free(thgrp->threads);
  thgrp->threads=0;

  --thgrp->tid2asid;    /* shift such that -1 == index 0 of allocate array */
  free( thgrp->tid2asid );
  thgrp->tid2asid = 0;
  
  thgrp->group_size = 0;

  return 0;
}
