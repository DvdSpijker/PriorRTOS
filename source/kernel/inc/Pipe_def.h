/*
 * Prior_osdef_pipe.h
 *
 * Created: 2-10-2017 12:54:38
 *  Author: Dorus
 */


#ifndef PRIOR_OSDEF_PIPE_H_
#define PRIOR_OSDEF_PIPE_H_


#include  <List.h>
#include <Types.h>


typedef struct PipeBlock_t {
    U8_t *buffer;
    U8_t size;
    struct PipeBlock_t *next_pipe_block;
} PipeBlock_t;


typedef struct Pipe_t {
    ListElement_t list_element;

    U16_t size;
    U16_t data_count;
    U8_t  block_count;
    U8_t  open;

    PipeBlock_t *head;
} Pipe_t;


typedef struct Pipe_t * pPipe_t;



void PipeInit(void);

#endif /* PRIOR_OSDEF_PIPE_H_ */