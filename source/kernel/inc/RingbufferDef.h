/*
 * RingpacketDef.h
 *
 * Created: 2-10-2017 12:59:35
 *  Author: Dorus
 */


#ifndef RINGpacket_DEF_H_
#define RINGpacket_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RINGpacket_LOCK_READ_MASK  0x80
#define RINGpacket_LOCK_WRITE_MASK 0x40


typedef struct Ringpacket_t {
    ListNode_t list_node;

    RingpacketBase_t  *packet;
    bool            ext_packet; /* packet is provided externally i.e. cannot be freed implicitly. */
    volatile U32_t  size;
    volatile U32_t  dcount;
    volatile U8_t   rw_lock;

    RingpacketBase_t* head;
    RingpacketBase_t* tail;
    volatile U32_t          write_index;
    volatile U32_t          read_index;

} Ringpacket_t;

typedef struct Ringpacket_t* pRingpacket_t;

OsResult_t KRingpacketInit(void);



#ifdef __cplusplus
}
#endif
#endif /* RINGpacket_DEF_H_ */
