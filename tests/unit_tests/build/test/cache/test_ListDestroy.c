#include "build/temp/_test_ListDestroy.c"
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











void test_ListDestroy_invalid_list(void)

{

 OsResult_t res = ListDestroy(

                             ((void *)0)

                                 );

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (("Wrong result code on invalid list.")), (UNITY_UINT)(55), UNITY_DISPLAY_STYLE_INT);

}



void test_ListDestroy_locked_list(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (("Wrong result code on valid list.")), (UNITY_UINT)(62), UNITY_DISPLAY_STYLE_INT);



 list.lock = 1;



 res = ListDestroy(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (("Wrong result code on locked list.")), (UNITY_UINT)(67), UNITY_DISPLAY_STYLE_INT);

}



void test_ListDestroy_empty_list(void)

{

 LinkedList_t list;

 OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (("Wrong result code on valid list.")), (UNITY_UINT)(74), UNITY_DISPLAY_STYLE_INT);



 res = ListDestroy(&list);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (("Wrong result code on empty list.")), (UNITY_UINT)(77), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((ListIsLocked(&list))), (UNITY_INT)((

1

)), (("Detroyed list must be locked.")), (UNITY_UINT)(78), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.head)), (UNITY_INT)((

((void *)0)

)), (("Head not deinitialized.")), (UNITY_UINT)(79), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.tail)), (UNITY_INT)((

((void *)0)

)), (("Tail not deinitialized.")), (UNITY_UINT)(80), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.size)), (UNITY_INT)((0)), (("Destroyed list must be size 0.")), (UNITY_UINT)(81), UNITY_DISPLAY_STYLE_INT);

}
