/*
** kaapi_fmt_predef.c
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
#include "kaapi_impl.h"
#include <string.h>


/* default functions */
static size_t kaapi_format_default_get_count_params
    (const struct kaapi_format_t* f, const void* p)
{ return f->_count_params; }

static kaapi_access_mode_t kaapi_format_default_get_mode_param
    (const struct kaapi_format_t* f, unsigned int i, const void* p)
{ return f->_mode_params[i]; }

static void* kaapi_format_default_get_off_param
    (const struct kaapi_format_t* f, unsigned int i, const void* p)
{ return (void*)((uintptr_t)p + f->_off_params[i]); }

static kaapi_access_t kaapi_format_default_get_access_param
    (const struct kaapi_format_t* f, unsigned int i, const void* p)
{ return *(kaapi_access_t*)((uintptr_t)p + f->_off_params[i]); }

static void kaapi_format_default_set_access_param
    (const struct kaapi_format_t* f, unsigned int i, void* p, const kaapi_access_t* a)
{ *(kaapi_access_t*)((uintptr_t)p + f->_off_params[i]) = *a; }

static const struct kaapi_format_t* kaapi_format_default_get_fmt_param
    (const struct kaapi_format_t* f, unsigned int i, const void* p)
{ return f->_fmt_params[i]; }

static kaapi_memory_view_t kaapi_format_default_get_view_param
    (const struct kaapi_format_t* f, unsigned int i, const void* p)
{ return f->_view_params[i]; }

static void kaapi_format_default_reduce_param 
    (const struct kaapi_format_t* f, unsigned int i, void* p, const void* value)
{ (*f->_reducor_params[i])((void*)((uintptr_t)p + f->_off_params[i]), value); }

static void kaapi_format_default_redinit_param 
    (const struct kaapi_format_t* f, unsigned int i, const void* sp, void* value )
{ (*f->_redinit_params[i])(value); }

static void kaapi_format_default_get_task_binding 
  (const struct kaapi_format_t* f, const void* task, kaapi_task_binding_t* b)
{ b->type = KAAPI_BINDING_ANY; }

/**
*/
kaapi_format_id_t kaapi_format_taskregister_static( 
        kaapi_format_t*             fmt,
        kaapi_task_body_t           body,
        kaapi_task_body_t           bodywh,
        const char*                 name,
        size_t                      size,
        int                         count,
        const kaapi_access_mode_t   mode_param[],
        const kaapi_offset_t        offset_param[],
        const kaapi_offset_t        offset_version[],
        const kaapi_format_t*       fmt_param[],
        const kaapi_memory_view_t   view_param[],
        const kaapi_reducor_t       reducor_param[],
        const kaapi_redinit_t       redinit_param[],
        const kaapi_task_binding_t* task_binding)
{
  kaapi_format_register( fmt, name );

  fmt->flag = KAAPI_FORMAT_STATIC_FIELD;
  
  fmt->size = (uint32_t)size;
  fmt->_count_params    = count;
  
  fmt->_mode_params = malloc( sizeof(kaapi_access_mode_t)*count );
  kaapi_assert( fmt->_mode_params !=0);
  memcpy(fmt->_mode_params, mode_param, sizeof(kaapi_access_mode_t)*count );

  fmt->_off_params = malloc( sizeof(kaapi_offset_t)*count );
  kaapi_assert( fmt->_off_params !=0);
  memcpy(fmt->_off_params, offset_param, sizeof(kaapi_offset_t)*count );

  fmt->_off_versions = malloc( sizeof(kaapi_offset_t)*count );
  kaapi_assert( fmt->_off_versions !=0);
  memcpy(fmt->_off_versions, offset_version, sizeof(kaapi_offset_t)*count );
  
  fmt->_fmt_params = malloc( sizeof(kaapi_format_t*)*count );
  kaapi_assert( fmt->_fmt_params !=0);
  memcpy(fmt->_fmt_params, fmt_param, sizeof(kaapi_format_t*)*count );

  fmt->_view_params = 0;
  if (view_param !=0)
  {
    fmt->_view_params = malloc( sizeof(kaapi_memory_view_t)*count );
    kaapi_assert( fmt->_view_params !=0);
    memcpy(fmt->_view_params, view_param, sizeof(kaapi_memory_view_t)*count );
  }

  if (reducor_param !=0)
  {
    fmt->_reducor_params = malloc( sizeof(kaapi_reducor_t)*count );
    kaapi_assert( fmt->_reducor_params !=0);
    memcpy(fmt->_reducor_params, reducor_param, sizeof(kaapi_reducor_t)*count );
  }
  if (redinit_param !=0)
  {
    fmt->_redinit_params = malloc( sizeof(kaapi_redinit_t)*count );
    kaapi_assert( fmt->_redinit_params !=0);
    memcpy(fmt->_redinit_params, redinit_param, sizeof(kaapi_redinit_t)*count );
  }

  if (task_binding)
    fmt->_task_binding = *task_binding;

  /* initialize to default functions */
  fmt->get_count_params = kaapi_format_default_get_count_params;
  fmt->get_mode_param   = kaapi_format_default_get_mode_param;
  fmt->get_off_param    = kaapi_format_default_get_off_param;
  fmt->get_access_param = kaapi_format_default_get_access_param;
  fmt->set_access_param = kaapi_format_default_set_access_param;
  fmt->get_fmt_param    = kaapi_format_default_get_fmt_param;
  fmt->get_view_param   = kaapi_format_default_get_view_param;
  fmt->reducor          = kaapi_format_default_reduce_param;
  fmt->redinit          = kaapi_format_default_redinit_param;
  fmt->get_task_binding	= kaapi_format_default_get_task_binding;
  
  memset(fmt->entrypoint, 0, sizeof(fmt->entrypoint));
  
  if (body !=0)
    kaapi_format_taskregister_body(fmt, body, bodywh, KAAPI_PROC_TYPE_CPU);
  return fmt->fmtid;
}
