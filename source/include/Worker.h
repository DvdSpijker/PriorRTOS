/******************************************************************************************************************************************
 *  File: Worker.h
 *  Description: Worker API.

 *  OS Version: V0.4
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *  Copyright© 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed or sold, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************************************************************************************/
#ifndef WORKER_H_
#define WORKER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Types.h>
#include <List.h>
#include <Task.h>

typedef U32_t (*WorkerJobFunction)(void *arg0, void *arg1, U32_t arg2, U32_t arg3);

typedef enum {
    JOB_STATE_QUEUED,
    JOB_STATE_ERROR,
    JOB_STATE_WORKING,
    JOB_STATE_DONE,
} WorkerJobState_t;


#define JOB_FLAG_LOCK_SCHEDULER 0x01    /* The job may not be pre-empted. */
#define JOB_FLAG_CRIT_SECTION   0x02    /* The job has to be executed in a critical section i.e. no interrupts. */
#define JOB_FLAG_COPY           0x04    /* The job is copied upon to the worker queue i.e. WorkerJob struct can be deleted after adding it. */
#define JOB_FLAG_NO_RETURN      0x08    /* The job has no return value and will automatically removed after processing. */

struct WorkerJobPrivate_t;

struct WorkerJob {
    WorkerJobFunction   entry;
    void                *p_arg;
    U32_t               v_arg;

    /****** Read-only fields ******/
    void                *p_arg_ret;
    U32_t               v_arg_ret;
    /*****************************/

    struct WorkerJobPrivate_t   *private;
};

struct WorkerJobPrivate_t {
    U8_t    temp;
};

/* Worker Task prototype. Must be used as the task
 * handler function for a Worker Task. */
void WorkerTask(const void *p_arg, U32_t v_arg); 

/* Worker API. */
OsResult_t KWorkerInit(void);

/* A task must be linked to the worker.
 * A priority within this category can still be assigned (1-5).
 * Max size indicates the maximum queue size.
 * Max time indicates the maximum amount of time that may be spend processing. */
Id_t WorkerCreate(Id_t task_id, U32_t max_size, U32_t max_time);

OsResult_t WorkerDelete(Id_t worker_id);

TaskState_t WorkerStateGet(Id_t worker_id);

/* Worker Job API. */

Id_t WorkerJobAdd(Id_t worker_id, struct WorkerJob *job);

OsResult_t WorkerJobRemove(Id_t job_id);

OsResult_t WorkerJobResultGet(Id_t job_id, struct WorkerJob *job);

bool WorkerJobIsDone(Id_t job_id);

WorkerJobState_t WorkerJobStateGet(Id_t job_id);


#ifdef __cplusplus
}
#endif
#endif /* WORKER_H_ */