/*
** kaapi_staticsched.h
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
#ifndef _KAAPI_EVENT_H_
#define _KAAPI_EVENT_H_

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define __KAAPI_TRACE_VERSION__         1

/** Definition of internal KAAPI events.
    Not that any extension or modification of the events must
    be reflected in the utility bin/kaapi_event_reader.
*/
#define KAAPI_EVT_KPROC_START        0     /* kproc begins, i0: processor type (0:CPU, 1:GPU) */
#define KAAPI_EVT_KPROC_STOP         1     /* kproc ends */

#define KAAPI_EVT_TASK_BEG           2     /* begin execution of tasks */
#define KAAPI_EVT_TASK_END           3     /* end execution of tasks */

#define KAAPI_EVT_FRAME_TL_BEG       4     /* begin execution using frame tasklist */
#define KAAPI_EVT_FRAME_TL_END       5     /* end execution using frame tasklist */

#define KAAPI_EVT_STATIC_BEG         6     /* begin of static schedule computation */
#define KAAPI_EVT_STATIC_END         7     /* end of static schedule computation */

#define KAAPI_EVT_STATIC_TASK_BEG    8     /* begin of task after static schedule, param:  */
#define KAAPI_EVT_STATIC_TASK_END    9     /* end of task after static schedule */

#define KAAPI_EVT_SCHED_IDLE_BEG     10     /* begin when k-processor starts to steal */
#define KAAPI_EVT_SCHED_IDLE_END     11    /* end when k-processor starts to steal */

#define KAAPI_EVT_SCHED_SUSPEND_BEG  12 /* when k-processor is suspending */
#define KAAPI_EVT_SCHED_SUSPEND_END  13 /* when k-processor wakeup */

#define KAAPI_EVT_SCHED_SUSPEND_POST 14 /* master thread post suspend */
#define KAAPI_EVT_SCHED_SUSPWAIT_BEG 15 /* master thread wait */
#define KAAPI_EVT_SCHED_SUSPWAIT_END 16 /* master thread end wait */

#define KAAPI_EVT_REQUESTS_BEG       17 /* when k-processor begin to process requests, data=victim.id */
#define KAAPI_EVT_REQUESTS_END       18 /* when k-processor end to process requests */

#define KAAPI_EVT_STEAL_OP           19 /* when k-processor emit a steal request data=victimid, serial*/
#define KAAPI_EVT_SEND_REPLY         20 /* when k-processor send a reply request data=victimid, thiefid, serial */
#define KAAPI_EVT_RECV_REPLY         21 /* when k-processor recv reply, victim id, serial, status (1:ok 0:nok) */


#define KAAPI_EVT_FOREACH_BEG        25 /* */
#define KAAPI_EVT_FOREACH_END        26 /* */
#define KAAPI_EVT_FOREACH_STEAL      27 /* */

#define KAAPI_EVT_CUDA_GPU_HTOD_BEG      28 /* cudaMemcpy time from GPU */
#define KAAPI_EVT_CUDA_GPU_HTOD_END      29

#define KAAPI_EVT_CUDA_GPU_DTOH_BEG      30 /* cudaMemcpy time from CPU */
#define KAAPI_EVT_CUDA_GPU_DTOH_END      31

#define KAAPI_EVT_CUDA_GPU_KERNEL_BEG        32
#define KAAPI_EVT_CUDA_GPU_KERNEL_END        33


#define KAAPI_EVT_CUDA_CPU_SYNC_BEG	    34	    /* CUDA sync calls by CPU */
#define KAAPI_EVT_CUDA_CPU_SYNC_END	    35


/** Size of the event mask 
*/
typedef uint64_t kaapi_event_mask_type_t;

/** Heler for creating mask from an event
*/
#define KAAPI_EVT_MASK(eventno) \
  ((kaapi_event_mask_type_t)1 << eventno)

/* The following set is always in the mask
*/
#define KAAPI_EVT_MASK_STARTUP \
    (  KAAPI_EVT_MASK(KAAPI_EVT_KPROC_START) \
     | KAAPI_EVT_MASK(KAAPI_EVT_KPROC_STOP) \
    )

#define KAAPI_EVT_MASK_COMPUTE \
    (  KAAPI_EVT_MASK(KAAPI_EVT_TASK_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_TASK_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_STATIC_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_STATIC_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_STATIC_TASK_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_STATIC_TASK_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_FOREACH_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_FOREACH_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_FOREACH_STEAL) \
     | KAAPI_EVT_MASK(KAAPI_EVT_CUDA_GPU_HTOD_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_CUDA_GPU_HTOD_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_CUDA_GPU_DTOH_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_CUDA_GPU_DTOH_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_CUDA_GPU_KERNEL_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_CUDA_GPU_KERNEL_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_CUDA_CPU_SYNC_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_CUDA_CPU_SYNC_END) \
    )

#define KAAPI_EVT_MASK_IDLE \
    (  KAAPI_EVT_MASK(KAAPI_EVT_SCHED_IDLE_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_SCHED_IDLE_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_SCHED_SUSPEND_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_SCHED_SUSPEND_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_SCHED_SUSPWAIT_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_SCHED_SUSPWAIT_END) \
    )

#define KAAPI_EVT_MASK_STEALOP \
    (  KAAPI_EVT_MASK(KAAPI_EVT_REQUESTS_BEG) \
     | KAAPI_EVT_MASK(KAAPI_EVT_REQUESTS_END) \
     | KAAPI_EVT_MASK(KAAPI_EVT_STEAL_OP) \
     | KAAPI_EVT_MASK(KAAPI_EVT_SEND_REPLY) \
     | KAAPI_EVT_MASK(KAAPI_EVT_RECV_REPLY) \
    )


/* ........................................ Implementation notes ........................................*/

/* entry of event. 3 entry per event.
*/
typedef union {
    void*     p;
    uintptr_t i; 
} kaapi_event_data_t;


/* Event 
*/
typedef struct kaapi_event_t {
  uint8_t     evtno;      /* event number */
  uint8_t     type;       /* event number */
  uint16_t    kid;        /* processor identifier */
  uint32_t    gid;        /* global identifier */
  uint64_t    date;       /* nano second */
  kaapi_event_data_t d0;
  kaapi_event_data_t d1;
  kaapi_event_data_t d2;
} kaapi_event_t;

/** Event Buffer 
*/
#define KAAPI_EVENT_BUFFER_SIZE 8190
typedef struct kaapi_event_buffer_t {
  struct kaapi_event_buffer_t* volatile next;
  uint32_t                              pos;
  int                                   kid;
  kaapi_event_t                         buffer[KAAPI_EVENT_BUFFER_SIZE];
} kaapi_event_buffer_t;

/** Header of the trace file. First block of each trace file.
*/
typedef struct kaapi_eventfile_header {
  int       version;
  int       minor_version;
  int       trace_version;
  int       cpucount;
  uint64_t  event_mask;
  char      package[128];
} kaapi_eventfile_header;


#if defined(__cplusplus)
}
#endif

#endif
