/*
 * Prior_pipe.h
 *
 * Created: 27-9-2016 22:27:41
 *  Author: Dorus
 */


#ifndef PIPE_H_
#define PIPE_H_

#include <Prior_types.h>


/* Pipe Event macros */
#define PIPE_EVENT_CREATE         EVENT_TYPE_CREATE
#define PIPE_EVENT_DELETE         EVENT_TYPE_DELETE
#define PIPE_EVENT_DATA_IN        (EVENT_TYPE_ACCESS | 0x00001000)
#define PIPE_EVENT_DATA_OUT       (EVENT_TYPE_ACCESS | 0x00002000)
#define PIPE_EVENT_EMPTY          (EVENT_TYPE_STATE_CHANGE | 0x00001000)
#define PIPE_EVENT_FULL           (EVENT_TYPE_STATE_CHANGE | 0x00002000)
#define PIPE_EVENT_OPENED         (EVENT_TYPE_STATE_CHANGE | 0x00003000)
#define PIPE_EVENT_CLOSED         (EVENT_TYPE_STATE_CHANGE | 0x00004000)

Id_t        PipeCreate(U16_t max_pipe_size);
OsResult_t  PipeDelete(Id_t *pipe_id);
OsResult_t  PipeOpenNamed(Id_t pipe_id);
OsResult_t  PipeOpenDirect(Id_t task_id, U16_t max_pipe_size); 
OsResult_t  PipeClose(Id_t pipe_id); /* Pass INVALID_ID to close currently-open direct pipe. */
OsResult_t  PipeWrite(Id_t pipe_id, U8_t *src, U8_t length);
OsResult_t  PipeRead(Id_t pipe_id, U8_t *dest, U8_t amount);
U16_t       PipeDataCountGet(Id_t pipe_id); /* Pass INVALID_ID to reference open direct pipe. */
U16_t       PipeDataSpaceGet(Id_t pipe_id); /* Pass INVALID_ID to reference open direct pipe. */
U8_t        PipeOpenCountGet(Id_t pipe_id); /* Pass INVALID_ID to reference open direct pipe. */
U8_t        PipeBlockCountGet(Id_t pipe_id); 
OsResult_t  PipeFlush(Id_t pipe_id);

#endif 