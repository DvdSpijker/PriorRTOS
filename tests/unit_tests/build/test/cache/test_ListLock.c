#include "build/temp/_test_ListLock.c"
#include "IdTypeDef.h"
#include "IdType.h"
#include "List.h"
#include "unity.h"




















void setUp(void)

{

}



void tearDown(void)

{

}











void test_ListLock_invalid_list(void)

{

 OsResult_t res = ListLock(

                          ((void *)0)

                              , 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (

((void *)0)

), (UNITY_UINT)(55), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_read(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(64), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_read_twice(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(73), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(76), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_read_max(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 for(int i = 0; i < 63; i++) {

  res = ListLock(&list, 0x00);

  UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

 ((void *)0)

 ), (UNITY_UINT)(86), UNITY_DISPLAY_STYLE_INT);

 }

}



void test_ListLock_lock_read_max_plus_one(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 for(int i = 0; i < 63; i++) {

  res = ListLock(&list, 0x00);

  UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

 ((void *)0)

 ), (UNITY_UINT)(97), UNITY_DISPLAY_STYLE_INT);

 }



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(101), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_write(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(110), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_write_twice(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(119), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(122), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_read_write(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(131), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(134), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_write_read(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(143), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(146), UNITY_DISPLAY_STYLE_INT);

}
