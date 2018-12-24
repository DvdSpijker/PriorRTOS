#include "build/temp/_test_ListInit.c"
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

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (("Wrong result code on invalid list.")), (UNITY_UINT)(55), UNITY_DISPLAY_STYLE_INT);

}



void test_ListInit_valid_list(void)

{

 OsResult_t res = OS_RES_ERROR;

 LinkedList_t list;



 res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (("Wrong result code on valid list.")), (UNITY_UINT)(64), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.head)), (UNITY_INT)((

((void *)0)

)), (("Head has a non-initialized value.")), (UNITY_UINT)(65), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.tail)), (UNITY_INT)((

((void *)0)

)), (("Tail has a non-initialized value.")), (UNITY_UINT)(66), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.id_group)), (UNITY_INT)((ID_GROUP_MESSAGE_QUEUE)), (("ID group has a non-initialized value.")), (UNITY_UINT)(67), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.size)), (UNITY_INT)((0)), (("List size is not 0.")), (UNITY_UINT)(68), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.lock)), (UNITY_INT)((0)), (("List is in a locked state.")), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

}
