/*
 * RingbufferDef.h
 *
 * Created: 2-10-2017 12:59:35
 *  Author: Dorus
 */


#ifndef RINGBUFFER_DEF_H_
#define RINGBUFFER_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RINGBUF_LOCK_READ_MASK  0x80
#define RINGBUF_LOCK_WRITE_MASK 0x40


typedef struct Ringbuf_t {
    ListNode_t list_node;

    RingbufBase_t  *buffer;
    bool            ext_buffer; /* Buffer is provided externally i.e. cannot be freed implicitly. */
    volatile U32_t  size;
    volatile U32_t  dcount;
    volatile U8_t   rw_lock;

    RingbufBase_t* head;
    RingbufBase_t* tail;
    volatile U32_t          write_index;
    volatile U32_t          read_index;

} Ringbuf_t;

typedef struct Ringbuf_t* pRingbuf_t;

OsResult_t KRingbufInit(void);



#ifdef __cplusplus
}
#endif
#endif /* RINGBUFFER_DEF_H_ */