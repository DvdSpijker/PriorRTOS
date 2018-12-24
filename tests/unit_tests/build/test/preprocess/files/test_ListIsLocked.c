#include "build/temp/_test_ListIsLocked.c"
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

), (UNITY_UINT)(59), UNITY_DISPLAY_STYLE_INT);

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

), (UNITY_UINT)(72), UNITY_DISPLAY_STYLE_INT);

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

), (UNITY_UINT)(83), UNITY_DISPLAY_STYLE_INT);

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

), (UNITY_UINT)(97), UNITY_DISPLAY_STYLE_INT);

}
