#include "build/temp/_test_ListNodeUnlock.c"
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











void test_ListNodeUnlock_invalid_node(void)

{

 OsResult_t res = OS_RES_ERROR;



 res = ListNodeUnlock(

                     ((void *)0)

                         );

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_INVALID_ARGUMENT)), (

((void *)0)

), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeUnlock_not_locked(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 res = ListNodeUnlock(&node);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_ERROR)), (

((void *)0)

), (UNITY_UINT)(68), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeUnlock_once(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );

 ListNodeLock(&node, 0x00);



 res = ListNodeUnlock(&node);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(80), UNITY_DISPLAY_STYLE_INT);

}



void test_ListNodeUnlock_max(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );



 for(int i = 0; i < 63; i++) {

  ListNodeLock(&node, 0x00);

 }



 for(int i = 0; i < 63; i++) {

  res = ListNodeUnlock(&node);

  UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

 ((void *)0)

 ), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_INT);

 }

}



void test_ListNodeUnlock_lock_after_unlock(void)

{

 OsResult_t res = OS_RES_ERROR;

 ListNode_t node;



 ListNodeInit(&node, 

                    ((void *)0)

                        );

 ListNodeLock(&node, 0x00);

 ListNodeLock(&node, 0x00);



 res = ListNodeUnlock(&node);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_OK)), (

((void *)0)

), (UNITY_UINT)(110), UNITY_DISPLAY_STYLE_INT);



 res = ListNodeLock(&node, 0x01);

 UnityAssertEqualNumber((UNITY_INT)((res)), (UNITY_INT)((OS_RES_LOCKED)), (

((void *)0)

), (UNITY_UINT)(113), UNITY_DISPLAY_STYLE_INT);

}
