#include "build/temp/_test_ListNodeIdGet.c"
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











void test_ListNodeIdGet_invalid_node(void)

{

 Id_t id = 0xFFFFFFFF;



 id = ListNodeIdGet(

                   ((void *)0)

                       );

 UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(58), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeInit_valid_node(void)

{

 ListNode_t node;

 Id_t id = 0xFFFFFFFF;

 Id_t exp_id = 2;



 memset(&node, 1, sizeof(node));



 node.id = exp_id;



 id = ListNodeIdGet(&node);

 UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((exp_id)), (

((void *)0)

), (UNITY_UINT)(72), UNITY_DISPLAY_STYLE_INT);

}
