/*
 *  kaapi_malloc_hook.c
 *  xkaapi
 *
 *  Created by Gautier Thierry on 25/04/10.
 *  Copyright 2010 CR INRIA. All rights reserved.
 *
 */
#include <malloc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

#include "kaapi_impl.h"
//typedef unsigned long long kaapi_uint64_t;

#if defined(__linux__)
 
/* Prototypes for our hooks.  */
static void *kaapi_malloc_hook (size_t, const void *);
static void kaapi_free_hook (void*, const void *);
static void *kaapi_realloc_hook (void *ptr, size_t size, const void *caller);

/* flush */
static void flush_buffer(void);

/* old hook */
static void *(*old_malloc_hook) (size_t, const void *);
static void (*old_free_hook) (void*, const void *);
static void *(*old_realloc_hook) (void *ptr, size_t size, const void *caller);

/* Buffer */
typedef struct EventRecord {
  kaapi_uint64_t date;
  const void*    ptr;
  const void*    sp;
  int            size; /* -1 == free, -2 == realloc */
} EventRecord;

static EventRecord* buffer_data;
static int buffer_size = 0;
static int buffer_wpos = 0;

static int fd_events = -1;

/* Override initializing hook from the C library. */
//void (*__malloc_initialize_hook) (void) = kaapi_init_mallochook;


/* put it thread safe or display 'per thread basis' */
void
kaapi_init_mallochook (void)
{
  old_malloc_hook  = __malloc_hook;
  old_free_hook    = __free_hook;
  old_realloc_hook = __realloc_hook;
  
  __malloc_hook  = kaapi_malloc_hook;
  __free_hook    = kaapi_free_hook;
  __realloc_hook = kaapi_realloc_hook;

  /* 1MByte */
  buffer_size = 128*1024*1024/sizeof(EventRecord);
  buffer_data = mmap( 0, buffer_size*sizeof(EventRecord), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, (off_t)0 );
  if (buffer_data == (void*)-1)
  {
    exit(1);
  }
}


/**/
static void *
kaapi_malloc_hook (size_t size, const void *caller)
{
  void *result;
  if (size ==0) return 0;
  /* Restore all old hooks */
  __malloc_hook  = old_malloc_hook;
  __free_hook    = old_free_hook;
  __realloc_hook = old_realloc_hook;
  
  /* Call recursively */
  result = malloc (size);

  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;
  old_realloc_hook  = __realloc_hook;

  buffer_data[buffer_wpos].date = kaapi_get_elapsedns();
  buffer_data[buffer_wpos].size = size;
  buffer_data[buffer_wpos].ptr = result;
  buffer_data[buffer_wpos].sp  = caller;
  ++buffer_wpos;
  if (buffer_wpos == buffer_size) flush_buffer();

  /* Restore our own hooks */
  __malloc_hook  = kaapi_malloc_hook;
  __free_hook    = kaapi_free_hook;
  __realloc_hook = kaapi_realloc_hook;
  return result;
}

/**/
static void
kaapi_free_hook (void *ptr, const void *caller)
{
  /* Restore all old hooks */
  __malloc_hook  = old_malloc_hook;
  __free_hook    = old_free_hook;
  __realloc_hook = old_realloc_hook;

  /* Call recursively */
  free (ptr);

  /* Save underlying hooks */
  old_malloc_hook   = __malloc_hook;
  old_free_hook     = __free_hook;
  old_realloc_hook  = __realloc_hook;

  buffer_data[buffer_wpos].date = kaapi_get_elapsedns();
  buffer_data[buffer_wpos].size = (size_t)0; /* means free op */
  buffer_data[buffer_wpos].ptr = ptr;
  buffer_data[buffer_wpos].sp = caller;
  ++buffer_wpos;
  if (buffer_wpos == buffer_size) flush_buffer();

  /* Restore our own hooks */
  __malloc_hook  = kaapi_malloc_hook;
  __free_hook    = kaapi_free_hook;
  __realloc_hook = kaapi_realloc_hook;
}


/**/
static 
void *kaapi_realloc_hook (void *ptr, size_t size, const void *caller)
{
  void *result;
  /* Restore all old hooks */
  __malloc_hook  = old_malloc_hook;
  __free_hook    = old_free_hook;
  __realloc_hook = old_realloc_hook;
  
  /* Call recursively */
  result = realloc (ptr, size);

  /* Save underlying hooks */
  old_malloc_hook   = __malloc_hook;
  old_free_hook     = __free_hook;
  old_realloc_hook  = __realloc_hook;

  buffer_data[buffer_wpos].date = kaapi_get_elapsedns();
  buffer_data[buffer_wpos].size = -size;
  buffer_data[buffer_wpos].ptr = result;
  buffer_data[buffer_wpos].sp = caller;
  ++buffer_wpos;
  if (buffer_wpos == buffer_size) flush_buffer();

  /* Restore our own hooks */
  __malloc_hook  = kaapi_malloc_hook;
  __free_hook    = kaapi_free_hook;
  __realloc_hook = kaapi_realloc_hook;
  return result;
}


void flush_buffer(void)
{
  ssize_t sz_w;
  size_t size;
  char* buffer;
  if (fd_events == -1)
  {
     fd_events = open("memevt_kaapi.raw", O_WRONLY|O_CREAT|O_TRUNC|O_EXCL);
     if (fd_events == -1) {
       exit(1);
     }
  }
  buffer = (char*)buffer_data;
  size = sizeof(EventRecord)*buffer_wpos;
  if (size >0)
  {
    do {
      sz_w = write( fd_events, buffer, size);
      if (sz_w != -1) 
      {
        buffer = buffer + sz_w;
        size -= sz_w; 
      }
    } while ((size >0) || ((sz_w ==-1) && (errno == EINTR)));
    if (sz_w == -1) 
    {
      exit(1);
    }
  }
  buffer_wpos = 0;
}


void
kaapi_fini_mallochook (void)
{
  flush_buffer();
  close( fd_events );
}


void kaapi_display_rawevents(int fd )
{
  kaapi_uint64_t t0 = 0;
  EventRecord e;
  ssize_t sz;
  do {
    sz = read(fd, &e, sizeof(e) );
    if ((sz ==-1) && (errno != EBUSY)) break;
    if ((sz !=0) && (sz != sizeof(e))) 
    {
      printf("Bad read of event: readsz=%i, waiting for=%i\n", sz, sizeof(e));
      exit(1);
    }
    if (t0 ==0) t0 = e.date;
    printf("%lu\t %i\t %p\t %p\n",e.date-t0, e.size, e.ptr, e.sp ); 
  } while (sz >0);
}

#else /* __linux */
void
kaapi_init_mallochook (void)
{
}

void
kaapi_fini_mallochook (void)
{
}

void kaapi_display_rawevents(int fd )
{
}

#endif
