#include "build/temp/_test_List.c"
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















void test_ListInit_invalid_list(void)

{

 OsResult_t res = ListInit(

                          ((void *)0)

                              , ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (("Wrong result code on invalid list.")), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_INT);

}



void test_ListInit_valid_list(void)

{

 OsResult_t res = OS_RES_ERROR;

 LinkedList_t list;



 res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (("Wrong result code on valid list.")), (UNITY_UINT)(66), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.head)), (UNITY_INT)((

((void *)0)

)), (("Head has a non-initialized value.")), (UNITY_UINT)(67), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.tail)), (UNITY_INT)((

((void *)0)

)), (("Tail has a non-initialized value.")), (UNITY_UINT)(68), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.id_group)), (UNITY_INT)((ID_GROUP_MESSAGE_QUEUE)), (("ID group has a non-initialized value.")), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.size)), (UNITY_INT)((0)), (("List size is not 0.")), (UNITY_UINT)(70), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.lock)), (UNITY_INT)((0)), (("List is in a locked state.")), (UNITY_UINT)(71), UNITY_DISPLAY_STYLE_INT);

}









void test_ListDestroy_invalid_list(void)

{

 OsResult_t res = ListDestroy(

                             ((void *)0)

                                 );

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (("Wrong result code on invalid list.")), (UNITY_UINT)(80), UNITY_DISPLAY_STYLE_INT);

}



void test_ListDestroy_locked_list(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (("Wrong result code on valid list.")), (UNITY_UINT)(87), UNITY_DISPLAY_STYLE_INT);



 list.lock = 1;



 res = ListDestroy(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (("Wrong result code on locked list.")), (UNITY_UINT)(92), UNITY_DISPLAY_STYLE_INT);

}



void test_ListDestroy_empty_list(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (("Wrong result code on valid list.")), (UNITY_UINT)(99), UNITY_DISPLAY_STYLE_INT);



 res = ListDestroy(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (("Wrong result code on empty list.")), (UNITY_UINT)(102), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((ListIsLocked(&list))), (UNITY_INT)((

1

)), (("Detroyed list must be locked.")), (UNITY_UINT)(103), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.head)), (UNITY_INT)((

((void *)0)

)), (("Head not deinitialized.")), (UNITY_UINT)(104), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.tail)), (UNITY_INT)((

((void *)0)

)), (("Tail not deinitialized.")), (UNITY_UINT)(105), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.size)), (UNITY_INT)((0)), (("Destroyed list must be size 0.")), (UNITY_UINT)(106), UNITY_DISPLAY_STYLE_INT);

}







void test_ListLock_invalid_list(void)

{

 OsResult_t res = ListLock(

                          ((void *)0)

                              , 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (

((void *)0)

), (UNITY_UINT)(114), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_read(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(123), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_read_twice(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(132), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(135), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_read_max(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 for(int i = 0; i < 63; i++) {

  res = ListLock(&list, 0x00);

  UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

 ((void *)0)

 ), (UNITY_UINT)(145), UNITY_DISPLAY_STYLE_INT);

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

 ), (UNITY_UINT)(156), UNITY_DISPLAY_STYLE_INT);

 }



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(160), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_write(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(169), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_write_twice(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(178), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(181), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_read_write(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(190), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(193), UNITY_DISPLAY_STYLE_INT);

}



void test_ListLock_lock_write_read(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(202), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(205), UNITY_DISPLAY_STYLE_INT);

}









void test_ListUnlock_not_locked(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListUnlock(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_ERROR)), (

((void *)0)

), (UNITY_UINT)(217), UNITY_DISPLAY_STYLE_INT);

}



void test_ListUnlock_once(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 ListLock(&list, 0x01);



 res = ListUnlock(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(228), UNITY_DISPLAY_STYLE_INT);

}



void test_ListUnlock_max(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 for(int i = 0; i < 63; i++) {

  ListLock(&list, 0x00);

 }



 for(int i = 0; i < 63; i++) {

  res = ListUnlock(&list);

  UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

 ((void *)0)

 ), (UNITY_UINT)(242), UNITY_DISPLAY_STYLE_INT);

 }

}



void test_ListUnlock_lock_after_unlock(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 ListLock(&list, 0x00);

 ListLock(&list, 0x00);



 res = ListUnlock(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(255), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(258), UNITY_DISPLAY_STYLE_INT);

}







void test_ListIsLocked_invalid_list(void)

{

 

_Bool 

     is_locked = 

                 0

                      ;



 is_locked = ListIsLocked(

                         ((void *)0)

                             );

 UnityAssertEqualNumber((UNITY_INT)((is_locked)), (UNITY_INT)((

0

)), (

((void *)0)

), (UNITY_UINT)(268), UNITY_DISPLAY_STYLE_INT);

}



void test_ListIsLocked_locked(void)

{

 

_Bool 

     is_locked = 

                 0

                      ;



 LinkedList_t list;

 ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 ListLock(&list, 0x00);



 is_locked = ListIsLocked(&list);

 UnityAssertEqualNumber((UNITY_INT)((is_locked)), (UNITY_INT)((

1

)), (

((void *)0)

), (UNITY_UINT)(281), UNITY_DISPLAY_STYLE_INT);

}



void test_ListIsLocked_unlocked(void)

{

 

_Bool 

     is_locked = 

                 0

                      ;



 LinkedList_t list;

 ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 is_locked = ListIsLocked(&list);

 UnityAssertEqualNumber((UNITY_INT)((is_locked)), (UNITY_INT)((

0

)), (

((void *)0)

), (UNITY_UINT)(292), UNITY_DISPLAY_STYLE_INT);

}



void test_ListIsLocked_unlocked_after_locked(void)

{

 

_Bool 

     is_locked = 

                 0

                      ;



 LinkedList_t list;

 ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 ListLock(&list, 0x00);

 ListUnlock(&list);



 is_locked = ListIsLocked(&list);

 UnityAssertEqualNumber((UNITY_INT)((is_locked)), (UNITY_INT)((

0

)), (

((void *)0)

), (UNITY_UINT)(306), UNITY_DISPLAY_STYLE_INT);

}
