#include "build/temp/_test_ListUnlock.c"
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











void test_ListUnlock_not_locked(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 res = ListUnlock(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_ERROR)), (

((void *)0)

), (UNITY_UINT)(58), UNITY_DISPLAY_STYLE_INT);

}



void test_ListUnlock_once(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);



 ListLock(&list, 0x01);



 res = ListUnlock(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

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

 ), (UNITY_UINT)(83), UNITY_DISPLAY_STYLE_INT);

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

), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_INT);



 res = ListLock(&list, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(99), UNITY_DISPLAY_STYLE_INT);

}
