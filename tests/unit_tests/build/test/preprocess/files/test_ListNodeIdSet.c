#include "build/temp/_test_ListNodeIdSet.c"
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











void test_ListNodeIdSet_invalid_node(void)

{

 OsResult_t res = OS_RES_ERROR;

 const Id_t id = 2;



 res = ListNodeIdSet(

                    ((void *)0)

                        , id);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (

((void *)0)

), (UNITY_UINT)(59), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeInit_valid_node(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;

 const Id_t id = 2;



 memset(&node, 1, sizeof(node));



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 res = ListNodeIdSet(&node, id);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(73), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((node.id)), (UNITY_INT)((id)), (

((void *)0)

), (UNITY_UINT)(74), UNITY_DISPLAY_STYLE_INT);

}
