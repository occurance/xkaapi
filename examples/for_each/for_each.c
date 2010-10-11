#include "kaapi.h"


typedef struct work
{
  volatile long lock;

  void (*op)(double*);

  double* array;

  volatile size_t i;
  volatile size_t j;

  /* master stealcontext */
  kaapi_stealcontext_t* msc;

} work_t;


/* fwd decl */
static void entry(void*, kaapi_thread_t*);


/* memory alignment */
static inline void* __attribute__((unused))
align_addr(void* addr)
{
  static const unsigned long ptrsize = sizeof(void*);
  static const unsigned long mask = sizeof(void*) - 1UL;

  /* assume n in bytes, power of 2 */
  if ((unsigned long)addr & mask)
    addr = (void*)((unsigned long)addr + ptrsize);

  return (void*)((unsigned long)addr & ~mask);
}


/* spinlocking */
static void lock_work(work_t* w)
{
  while (!__sync_bool_compare_and_swap(&w->lock, 0, 1))
    __asm__ __volatile__ ("pause\n\t");
}

static void unlock_work(work_t* w)
{
  __sync_fetch_and_and(&w->lock, 0);
}


/* parallel work splitter */
static int split
(kaapi_stealcontext_t* sc, int nreq, kaapi_request_t* req, void* args)
{
  /* victim work */
  work_t* const vw = (work_t*)args;

  /* master stealcontext */
  kaapi_stealcontext_t* msc = (vw->msc == NULL) ? sc : vw->msc;

  /* stolen range */
  size_t i, j;

  /* reply count */
  int nrep = 0;

  /* size per request */
  unsigned int unit_size;

  /* concurrent with victim */
  lock_work(vw);

  const size_t total_size = vw->j - vw->i;

  /* how much per req */
#define CONFIG_PAR_GRAIN 128
  unit_size = 0;
  if (total_size > CONFIG_PAR_GRAIN)
  {
    unit_size = total_size / (nreq + 1);
    if (unit_size == 0)
    {
      nreq = (total_size / CONFIG_PAR_GRAIN) - 1;
      unit_size = CONFIG_PAR_GRAIN;
    }

    /* steal and update victim range */
    const size_t stolen_size = unit_size * nreq;
    i = vw->j - stolen_size;
    j = vw->j;
    vw->j -= stolen_size;
  }

  unlock_work(vw);

  if (unit_size == 0)
    return 0;

  for (; nreq; --nreq, ++req, ++nrep, j -= unit_size)
  {
    /* thief work */
    work_t* const tw = kaapi_reply_pushtask(msc, req, entry);

    tw->lock = 0;
    tw->op = vw->op;
    tw->array = vw->array;
    tw->i = j - unit_size;
    tw->j = j;
    tw->msc = msc;

    kaapi_request_reply_head(sc, req, NULL);
  }

  return nrep;
}


/* seq work extractor */
static int extract_seq
(work_t* w, double** pos, double** end)
{
  /* extract from range beginning */

#define CONFIG_SEQ_GRAIN 64
  size_t seq_size = CONFIG_SEQ_GRAIN;

  size_t i, j;

  lock_work(w);

  i = w->i;

  if (seq_size > (w->j - w->i))
    seq_size = w->j - w->i;

  j = w->i + seq_size;
  w->i += seq_size;

  unlock_work(w);

  if (seq_size == 0)
    return -1;

  *pos = w->array + i;
  *end = w->array + j;

  return 0;
}


/* entrypoint */
static void entry
(void* args, kaapi_thread_t* thread)
{
  /* adaptive stealcontext flags */
  const int flags = KAAPI_SC_CONCURRENT | KAAPI_SC_NOPREEMPTION;

  /* push an adaptive task */
  kaapi_stealcontext_t* const sc =
    kaapi_task_begin_adaptive(thread, flags, split, args);

  /* process the work */
  work_t* const w = (work_t*)args;

  /* range to process */
  double* pos;
  double* end;

  /* while there is sequential work to do*/
  while (extract_seq(w, &pos, &end) != -1)
  {
    /* apply w->op foreach item in [pos, end[ */
    for (; pos != end; ++pos)
      w->op(pos);
  }

  /* wait for thieves */
  kaapi_task_end_adaptive(sc);
}


/* algorithm main function */
static void for_each
(double* array, size_t size, void (*op)(double*))
{
  /* self thread, task */
  kaapi_thread_t* const thread = kaapi_self_thread();
  kaapi_task_t* const task = kaapi_thread_toptask(thread);

  kaapi_frame_t frame;
  kaapi_thread_save_frame(thread, &frame);

  /* work */
  work_t* const w = kaapi_alloca_align(64, sizeof(work_t));

  /* initialize work */
  w->lock = 0;
  w->array = array;
  w->op = op;
  w->i = 0;
  w->j = size;
  w->msc = NULL;

  /* fork root task */
  kaapi_task_init(task, entry, (void*)w);
  kaapi_thread_pushtask(thread);
  kaapi_sched_sync();

  kaapi_thread_restore_frame(thread, &frame);
}


/* unit */

#include <string.h>

static int check(const double* array, size_t size)
{
  for (; size; ++array, --size)
    if (*array != 1.f)
      return -1;
  return 0;
}

static void addone(double* d)
{
  ++*d;
}

int main(int ac, char** av)
{
#define ITEM_COUNT 100000
  static double array[ITEM_COUNT];

  /* initialize the runtime */
  kaapi_init();

  for (ac = 0; ac < 1000; ++ac)
  {
    /* initialize, apply, check */
    memset(array, 0, sizeof(array));
    for_each(array, ITEM_COUNT, addone);
    if (check(array, ITEM_COUNT) == -1)
    {
      printf("invalid %d\n", ac);
      break ;
    }
  }

  printf("done\n");

  /* finalize the runtime */
  kaapi_finalize();

  return 0;
}
