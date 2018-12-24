#include "build/temp/_test_ListNodeLock.c"
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











void test_ListNodeLock_invalid_node(void)

{

 OsResult_t res = ListNodeLock(

                              ((void *)0)

                                  , 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (

((void *)0)

), (UNITY_UINT)(55), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeLock_lock_read(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 res = ListNodeLock(&node, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(66), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeLock_lock_read_twice(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 res = ListNodeLock(&node, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(77), UNITY_DISPLAY_STYLE_INT);



 res = ListNodeLock(&node, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(80), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeLock_lock_read_max(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 for(int i = 0; i < 63; i++) {

  res = ListNodeLock(&node, 0x00);

  UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

 ((void *)0)

 ), (UNITY_UINT)(92), UNITY_DISPLAY_STYLE_INT);

 }

}



void test_ListNodeLock_lock_read_max_plus_one(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 for(int i = 0; i < 63; i++) {

  res = ListNodeLock(&node, 0x00);

  UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

 ((void *)0)

 ), (UNITY_UINT)(105), UNITY_DISPLAY_STYLE_INT);

 }



 res = ListNodeLock(&node, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(109), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeLock_lock_write(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 res = ListNodeLock(&node, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(120), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeLock_lock_write_twice(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 res = ListNodeLock(&node, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(131), UNITY_DISPLAY_STYLE_INT);



 res = ListNodeLock(&node, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(134), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeLock_lock_read_write(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 res = ListNodeLock(&node, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(145), UNITY_DISPLAY_STYLE_INT);



 res = ListNodeLock(&node, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(148), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeLock_lock_write_read(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 res = ListNodeLock(&node, 0x00);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(159), UNITY_DISPLAY_STYLE_INT);



 res = ListNodeLock(&node, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(162), UNITY_DISPLAY_STYLE_INT);

}
