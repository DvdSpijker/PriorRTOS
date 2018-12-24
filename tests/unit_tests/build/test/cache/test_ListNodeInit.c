#include "build/temp/_test_ListNodeInit.c"
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











void test_ListNodeInit_invalid_node(void)

{

 OsResult_t res = OS_RES_ERROR;



 res = ListNodeInit(

                   ((void *)0)

                       , &res);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (

((void *)0)

), (UNITY_UINT)(58), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeInit_valid_node_no_child(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 memset(&node, 1, sizeof(node));



 res = ListNodeInit(&node, 

                          ((void *)0)

                              );

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((node.child)), (UNITY_INT)((

((void *)0)

)), (

((void *)0)

), (UNITY_UINT)(70), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeInit_valid_node_w_child(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 memset(&node, 1, sizeof(node));



 res = ListNodeInit(&node, &res);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(81), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((node.child)), (UNITY_INT)((&res)), (

((void *)0)

), (UNITY_UINT)(82), UNITY_DISPLAY_STYLE_INT);

}
