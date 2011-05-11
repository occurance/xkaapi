#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <numaif.h>
#include <numa.h>
#include "kaapi_impl.h"


typedef unsigned long kaapi_numaid_t;
typedef unsigned long kaapi_procid_t;


static unsigned int numa_node_count;

int kaapi_numa_initialize(void)
{
  numa_set_bind_policy(1);
  numa_set_strict(1);
  numa_node_count = (unsigned int)numa_max_node() + 1;

  return 0;
}

int kaapi_numa_bind
(const void* addr, size_t size, kaapi_numaid_t id)
{
  const int mode = MPOL_BIND;
  const unsigned int flags = MPOL_MF_STRICT | MPOL_MF_MOVE;
  const unsigned long maxnode = KAAPI_MAX_PROCESSOR;
  
  unsigned long nodemask
    [KAAPI_MAX_PROCESSOR / (8 * sizeof(unsigned long)) + 1];

  memset(nodemask, 0, sizeof(nodemask));

  nodemask[id / (8 * sizeof(unsigned long))] |=
    1UL << (id % (8 * sizeof(unsigned long)));

  if (mbind((void*)addr, size, mode, nodemask, maxnode, flags))
    return -1;

  return 0;
}

int kaapi_numa_alloc(void** addr, size_t size, kaapi_numaid_t id)
{
#define NUMA_FUBAR_FIX 1
#if NUMA_FUBAR_FIX /* bug: non reentrant??? */
  *addr = NULL;
  if (posix_memalign(addr, 0x1000, size) == 0)
  {
    if (kaapi_numa_bind(*addr, size, id))
    { free(addr); *addr = NULL; }
  }
#else
  *addr = numa_alloc_onnode(size, (int)id);
#endif
  return (*addr == NULL) ? -1 : 0;
}

void kaapi_numa_free(void* addr, size_t size)
{
#if NUMA_FUBAR_FIX
  free(addr);
#else
  numa_free(addr, size);
#endif
}


#if 0 /* UNUSED */

int kaapi_numa_alloc_with_procid
(void** addr, size_t size, kaapi_procid_t procid)
{
  const kaapi_numaid_t numaid = kaapi_numa_procid_to_numaid(procid);
  return kaapi_numa_alloc(addr, size, numaid);
}

kaapi_numaid_t kaapi_numa_get_page_node(uintptr_t addr)
{
  const unsigned long flags = MPOL_F_NODE | MPOL_F_ADDR;
  kaapi_bitmap_t nodemask;

  const int err = get_mempolicy
    (NULL, nodemask.bits, KAAPI_MAX_PROCESSOR, (void*)addr, flags);

  if (err || kaapi_bitmap_is_empty(&nodemask))
    return KAAPI_INVALID_PROCID;

  return kaapi_bitmap_scan(&nodemask, 0);
}

#endif /* unused */


/* exported numa allocator */

static uintptr_t base_addr = 0;

void* kaapi_numa_alloc_interleaved(size_t size)
{
  kaapi_numaid_t numaid;
  size_t off;
  void* addr;

  if (posix_memalign(&addr, 0x1000, size))
  {
    printf("[!]posix_memalign\n");
    return NULL;
  }

  /* bind interleaved */
  numaid = 0;
  for (off = 0; off < size; off += 0x1000)
  {
    kaapi_numa_bind((const void*)((uintptr_t)addr + off), 0x1000, numaid);
    numaid = (numaid + 1) % numa_node_count;
  }

  /* update only the first allocation. this is very
     ephemeral, since we should not need this global
     pointer when views are available.
   */
  if (base_addr == 0) base_addr = (uintptr_t)addr;

  return addr;
}

void kaapi_numa_free_interleaved(void* addr)
{
  free(addr);
}

kaapi_numaid_t kaapi_numa_get_addr_binding(void* addr)
{
  /* return the numa node addr is allocated on.
     we know the base address from a previous
     allocation. we know the stride size from
     the previous allocation.
   */

  const size_t off = ((uintptr_t)addr - base_addr) / 0x1000;
  return off % numa_node_count;
}


/* processor to numa id */

kaapi_numaid_t kaapi_numa_get_kid_binding(unsigned int kid)
{
  const int procid = (int)kaapi_default_param.kid2cpu[kid];
  return (kaapi_numaid_t)numa_node_of_cpu(procid);
}

kaapi_numaid_t kaapi_numa_get_self_binding(void)
{
  return kaapi_numa_get_kid_binding(kaapi_get_self_kid());
}

kaapi_procid_t kaapi_get_self_cpuid(void)
{
  return (kaapi_procid_t)
    kaapi_default_param.kid2cpu[kaapi_get_self_kid()];
}
