/**********************************************************************************************************************************************
 *  File: Prior_pipes.c
 *
 *  Description: Prior Pipe module
 *
 *  OS Version: V0.3
 *  Date: 13/03/2015
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *
 *  Copyright© 2016    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific prior written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************************************************************************/

#include "../Include/List.h"
#include "../Include/Prior_types.h"
#include "../Include/Prior_pipe.h"
#include "../Include/Prior_mm.h"
#include "../OSDefinitions/Prior_osdef_pipe.h"
#include "../OSDefinitions/Prior_osdef_core.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

Id_t PipePool;
Id_t PipeIdBuffer[2];
LinkedList_t PipeList;

pPipe_t UtilPipeFromId(Id_t pipe_id);

void PipeInit()
{
    PipePool = MmPoolCreate(CONFIG_PIPE_MEMSIZE);
    ListInit(&PipeList, ID_TYPE_PIPE, PipeIdBuffer, 2);
}

Id_t PipeCreate(U8_t pipe_size)
{
    pPipe_t new_pipe = (pPipe_t)malloc(sizeof(Pipe_t));

    if(new_pipe != NULL) {
        ListElementInit(&new_pipe->list_element, (void*)new_pipe);
        new_pipe->size = new_pipe->data_count = pipe_size;
        new_pipe->buffer = (U8_t*)MmAllocDynamic(PipePool, pipe_size);
        return ListElementIdGet(&new_pipe->list_element);
    } else {
        return INV_ID;
    }
}


OsResult_t PipeDelete(Id_t *pipe_id)
{
    pPipe_t tmp_pipe = UtilPipeFromId(*pipe_id);

    if(tmp_pipe->data_count == tmp_pipe->size) {//Pipe is empty
        ListElementDeinit(&PipeList, &tmp_pipe->list_element);
        free((void*)tmp_pipe);
        *pipe_id = INV_ID;
        return e_ok;
    } else { // Pipe is not empty
        return e_fail;
    }
}

OsResult_t  PipeOpen(Id_t pipe_id)
{
    pPipe_t pipe = UtilPipeFromId(pipe_id);
    if(pipe != NULL) {
        if(pipe->open < 2) {
            pipe->open++;
            return e_ok;
        } else {
            return e_locked;
        }
    }
    return e_error;
}

OsResult_t PipeClose(Id_t pipe_id)
{
    pPipe_t pipe = UtilPipeFromId(pipe_id);
    if(pipe != NULL) {
        if(pipe->open > 0) {
            pipe->open--;
            return e_ok;
        } else {
            return e_locked;
        }
    }
    return e_error;
}

OsResult_t PipeWrite(Id_t pipe_id, U8_t *src, U8_t length)
{
    pPipe_t pipe = UtilPipeFromId(pipe_id);
    if(pipe != NULL) {
        if(pipe->open == 0) {
            return e_locked;
        }
        if((pipe->size - pipe->data_count) < length) {
            return e_fail;
        }

        for (U8_t i = 0; i < length; i++) {
            pipe->buffer[pipe->write_index] = data_source[i];
            pipe->data_count++;
            if(pipe->write_index < (pipe->size - 1)) {
                pipe->write_index++;
            } else {
                break;
            }
        }
        util_BroadcastWaitList(pipe_id);
        return e_ok;
    }

    return e_error;
}

OsResult_t  PipeRead(Id_t pipe_id, U8_t *dst, U8_t amount)
{
    pPipe_t pipe = UtilPipeFromId(pipe_id);

    if(pipe != NULL) {
        if(pipe->open == 0) {
            return e_locked;
        }
        if((pipe->data_count) < amount) {
            return e_fail;
        }

        for (U8_t i = 0; i < amount; i++) {
            data_destination[i] = pipe->buffer[pipe->write_index];
            pipe->data_count--;
            if(pipe->read_index < (pipe->size - 1)) {
                pipe->read_index++;
            } else {
                break;
            }
        }
        util_BroadcastWaitList(pipe_id);
        return e_ok;
    }


    return e_error;
}

U16_t PipeDataCountGet(Id_t pipe_id)
{
    pPipe_t tmp_pipe = UtilPipeFromId(pipe_id);
    return tmp_pipe == NULL ? 0 : tmp_pipe->data_count;
}

U16_t PipeDataSpaceGet(Id_t pipe_id)
{
    pPipe_t tmp_pipe = UtilPipeFromId(pipe_id);
    return tmp_pipe == NULL ? 0 : (tmp_pipe->size - tmp_pipe->data_count);
}

U8_t PipeOpenCountGet(Id_t pipe_id)
{
    pPipe_t tmp_pipe = UtilPipeFromId(pipe_id);
    return tmp_pipe == NULL ? 0 : (tmp_pipe->open);
}

U8_t PipeBlockCountGet(Id_t pipe_id)
{
    pPipe_t tmp_pipe = UtilPipeFromId(pipe_id);
    return tmp_pipe == NULL ? 0 : (tmp_pipe->block_count);
}

pPipe_t UtilPipeFromId(Id_t pipe_id)
{
    ListElement_t *element = ListElementSearchId(&PipeList, pipe_id);
    if(element != NULL) {
        return (pPipe_t)ListElementChildGet(element);
    } else {
        return NULL;
    }
}