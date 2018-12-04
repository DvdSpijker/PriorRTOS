/*
 * EventgroupTest.h
 *
 * Created: 27-7-2017 13:22:37
 *  Author: Dorus
 */


#ifndef EVENTGROUPTEST_H_
#define EVENTGROUPTEST_H_



#include <Types.h>

void EventgroupTestSet(const void *p_arg, U32_t v_arg);
void EventgroupTestListen(const void *p_arg, U32_t v_arg);

Id_t TskEventgroupTestSet;

Id_t TskEventgroupTestListen;



#endif /* EVENTGROUPTEST_H_ */