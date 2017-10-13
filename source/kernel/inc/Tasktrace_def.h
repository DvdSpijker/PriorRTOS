/*
 * Prior_osdef_tasktrace.h
 *
 * Created: 2-10-2016 13:16:12
 *  Author: Dorus
 */ 


#ifndef PRIOR_OSDEF_TASKTRACE_H_
#define PRIOR_OSDEF_TASKTRACE_H_


pRingbuf_t tasktrace_rxbuffer;
pRingbuf_t tasktrace_txbuffer;


OsResult_t TtTasksActiveGet(Id_t *list, U8_t Listlength);

OsResult_t TtTasksTotalGet(Id_t *list, U8_t Listlength);


#endif /* PRIOR_OSDEF_TASKTRACE_H_ */