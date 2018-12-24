#include "build/temp/_test_IdType.c"
#include "IdTypeDef.h"
#include "IdType.h"
#include "unity.h"














void setUp(void)

{

 KIdInit();

}



void tearDown(void)

{

}















void test_KIdRequest_invalid_group(void)

{

 Id_t id = KIdRequest(ID_GROUP_NUM + 1);

 UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_INT);



 id = KIdRequest(-1);

 UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(60), UNITY_DISPLAY_STYLE_INT);

}



void test_KIdRequest_valid_group_single(void)

{

 const Id_t exp_seq = 0;



 Id_t id = KIdRequest(ID_GROUP_MAILBOX);

 if (((id) != (0xFFFFFFFF))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(68)));};

 UnityAssertEqualNumber((UNITY_INT)(((id & 0xFF000000))), (UNITY_INT)((((Id_t)(ID_GROUP_MAILBOX << 24) & 0xFF000000))), (

((void *)0)

), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)(((id & 0x00FFFFFF))), (UNITY_INT)(((exp_seq & 0x00FFFFFF))), (

((void *)0)

), (UNITY_UINT)(70), UNITY_DISPLAY_STYLE_INT);

}



void test_KIdRequest_valid_group_all(void)

{

 Id_t exp_seq = 0;

 Id_t id = 0xFFFFFFFF;



 for(int i = 0; i < ID_GROUP_NUM; i++) {

  id = KIdRequest(i);

  if (((id) != (0xFFFFFFFF))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(80)));};

  UnityAssertEqualNumber((UNITY_INT)(((id & 0xFF000000))), (UNITY_INT)((((Id_t)(i << 24) & 0xFF000000))), (

 ((void *)0)

 ), (UNITY_UINT)(81), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)(((id & 0x00FFFFFF))), (UNITY_INT)(((exp_seq & 0x00FFFFFF))), (

 ((void *)0)

 ), (UNITY_UINT)(82), UNITY_DISPLAY_STYLE_INT);

 }

}



void test_KIdRequest_all(void)

{

 uint32_t id_cnt = 0;

 Id_t id = 0xFFFFFFFF;



 for(int i = 0; i < ID_GROUP_NUM; i++) {

  for(uint32_t j = 0; j < (0xFFFFFFFF & 0x00FFFFFF); j++) {

   id_cnt++;

   id = KIdRequest(i);

   if (((id) != (0xFFFFFFFF))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(95)));};

   UnityAssertEqualNumber((UNITY_INT)(((id & 0xFF000000))), (UNITY_INT)((((Id_t)(i << 24) & 0xFF000000))), (

  ((void *)0)

  ), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(((id & 0x00FFFFFF))), (UNITY_INT)(((j & 0x00FFFFFF))), (

  ((void *)0)

  ), (UNITY_UINT)(97), UNITY_DISPLAY_STYLE_INT);

  }

 }

 UnityAssertEqualNumber((UNITY_INT)((id_cnt)), (UNITY_INT)((0x00FFFFFF * ID_GROUP_NUM)), (

((void *)0)

), (UNITY_UINT)(100), UNITY_DISPLAY_STYLE_INT);

 printf("Number of IDs: %u\n", id_cnt);

}



void test_KIdRequest_all_plus_one(void)

{

 uint32_t id_cnt = 0;

 Id_t id = 0xFFFFFFFF;



 for(int i = 0; i < ID_GROUP_NUM; i++) {

  for(uint32_t j = 0; j < (0xFFFFFFFF & 0x00FFFFFF); j++) {

   id_cnt++;

   id = KIdRequest(i);

   if (((id) != (0xFFFFFFFF))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(113)));};

   UnityAssertEqualNumber((UNITY_INT)(((id & 0xFF000000))), (UNITY_INT)((((Id_t)(i << 24) & 0xFF000000))), (

  ((void *)0)

  ), (UNITY_UINT)(114), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(((id & 0x00FFFFFF))), (UNITY_INT)(((j & 0x00FFFFFF))), (

  ((void *)0)

  ), (UNITY_UINT)(115), UNITY_DISPLAY_STYLE_INT);

  }

  id_cnt++;

  id = KIdRequest(i);

  UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((0xFFFFFFFF)), (

 ((void *)0)

 ), (UNITY_UINT)(119), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((id_cnt)), (UNITY_INT)(((0x00FFFFFF + 1) * ID_GROUP_NUM)), (

((void *)0)

), (UNITY_UINT)(121), UNITY_DISPLAY_STYLE_INT);

 printf("Number of IDs: %u\n", id_cnt);

}







void test_IdSequenceNumberGet_valid(void)

{

 const Id_t id = ID_GROUP_MESSAGE_QUEUE | 0x00002000;



 Id_t id_seq = IdSequenceNumberGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_seq)), (UNITY_INT)(((id & 0x00FFFFFF))), (

((void *)0)

), (UNITY_UINT)(132), UNITY_DISPLAY_STYLE_INT);

}



void test_IdSequenceNumberGet_invalid_id(void)

{

 const Id_t id = 0xFFFFFFFF;



 Id_t id_seq = IdSequenceNumberGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_seq)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(140), UNITY_DISPLAY_STYLE_INT);

}



void test_IdSequenceNumberGet_invalid_group(void)

{

 Id_t id = 0xFF000000 | 0x00001000;

 Id_t id_seq = IdSequenceNumberGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_seq)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(147), UNITY_DISPLAY_STYLE_INT);



 id = (ID_GROUP_NUM + 1) << 24 | 0x00001000;

 id_seq = IdSequenceNumberGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_seq)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(151), UNITY_DISPLAY_STYLE_INT);

}







void test_IdGroupGet_valid(void)

{

 const Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << 24) | 0x00002000;

 IdGroup_t id_group = IdGroupGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_group)), (UNITY_INT)((ID_GROUP_MESSAGE_QUEUE)), (

((void *)0)

), (UNITY_UINT)(160), UNITY_DISPLAY_STYLE_INT);

}



void test_IdGroupGet_invalid_group(void)

{

 Id_t id = 0xFF000000 | 0x00001000;

 IdGroup_t id_group = IdGroupGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_group)), (UNITY_INT)((ID_GROUP_INV)), (

((void *)0)

), (UNITY_UINT)(167), UNITY_DISPLAY_STYLE_INT);



 id = (ID_GROUP_NUM + 1) << 24 | 0x00001000;

 id_group = IdGroupGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_group)), (UNITY_INT)((ID_GROUP_INV)), (

((void *)0)

), (UNITY_UINT)(171), UNITY_DISPLAY_STYLE_INT);

}







void test_IdIsInGroup_valid_true(void)

{

 Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << 24) | 0x00002000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((1)), (

((void *)0)

), (UNITY_UINT)(180), UNITY_DISPLAY_STYLE_INT);

}



void test_IdIsInGroup_valid_false(void)

{

 Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << 24) | 0x00002000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_EVENTGROUP);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(187), UNITY_DISPLAY_STYLE_INT);

}



void test_IdIsInGroup_invalid_group(void)

{

 Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << 24) | 0x00002000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_NUM);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(194), UNITY_DISPLAY_STYLE_INT);

}



void test_IdIsInGroup_invalid_id(void)

{

 Id_t id = 0xFF000000 | 0x00001000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_EVENTGROUP);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(201), UNITY_DISPLAY_STYLE_INT);

}



void test_IdIsInGroup_invalid_group_and_id(void)

{

 Id_t id = 0xFF000000 | 0x00001000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_NUM);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(208), UNITY_DISPLAY_STYLE_INT);

}







void test_IdListInit_valid(void)

{

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(217), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (

 ((void *)0)

 ), (UNITY_UINT)(219), UNITY_DISPLAY_STYLE_INT);

 }

}



void test_IdListInit_invalid(void)

{

 IdListInit(

           ((void *)0)

               );

}







void test_IdListIdAdd_single(void)

{

 Id_t id = 1;

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(235), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(237), UNITY_DISPLAY_STYLE_INT);

 }



 IdListIdAdd(&list, id);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((1)), (("Expected 1 item in the ID list.")), (UNITY_UINT)(241), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is not correct.")), (UNITY_UINT)(242), UNITY_DISPLAY_STYLE_INT);

}



void test_IdListIdAdd_all(void)

{

 Id_t id = 1;

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(250), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(252), UNITY_DISPLAY_STYLE_INT);

 }



 for(uint8_t j = 0; j < 5; j++) {

  id = j;

  IdListIdAdd(&list, id);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((j + 1)), (("Number of items in the ID list incorrect.")), (UNITY_UINT)(258), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is incorrect.")), (UNITY_UINT)(259), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5)), (("Number of IDs in the list is incorrect")), (UNITY_UINT)(261), UNITY_DISPLAY_STYLE_INT);

}



void test_IdListIdAdd_all_plus_one(void)

{

 Id_t id = 1;

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(269), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(271), UNITY_DISPLAY_STYLE_INT);

 }



 for(uint8_t j = 0; j < 5; j++) {

  id = j;

  IdListIdAdd(&list, id);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((j + 1)), (("Expected number of items in the ID list incorrect.")), (UNITY_UINT)(277), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is incorrect.")), (UNITY_UINT)(278), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5)), (("Number of IDs in the list is incorrect")), (UNITY_UINT)(280), UNITY_DISPLAY_STYLE_INT);

 id++;

 IdListIdAdd(&list, id);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5)), (("Added an ID beyond the list capacity.")), (UNITY_UINT)(283), UNITY_DISPLAY_STYLE_INT);

}







void test_IdListIdRemove_invalid_list(void)

{

 Id_t id = IdListIdRemove(

                         ((void *)0)

                             );

 UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((0xFFFFFFFF)), (("Removed valid ID from invalid list.")), (UNITY_UINT)(291), UNITY_DISPLAY_STYLE_INT);

}



void test_IdListIdRemove_single(void)

{

 Id_t id = 1;

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(299), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(301), UNITY_DISPLAY_STYLE_INT);

 }



 IdListIdAdd(&list, id);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((1)), (("Expected 1 item in the ID list.")), (UNITY_UINT)(305), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is not correct.")), (UNITY_UINT)(306), UNITY_DISPLAY_STYLE_INT);



 Id_t rm_id = IdListIdRemove(&list);

 UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((rm_id)), (("Removed ID not equal to added ID.")), (UNITY_UINT)(309), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("Number of items in the list incorrect.")), (UNITY_UINT)(310), UNITY_DISPLAY_STYLE_INT);

}



void test_IdListIdRemove_all(void)

{

 Id_t id_array[5] = {0xFFFFFFFF};

 Id_t id = 1;

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(319), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(321), UNITY_DISPLAY_STYLE_INT);

 }



 for(uint8_t i = 0; i < 5; i++) {

  id = i;

  id_array[i] = id;

  IdListIdAdd(&list, id);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((i + 1)), (("Number of items in the ID list incorrect.")), (UNITY_UINT)(328), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is incorrect.")), (UNITY_UINT)(329), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5)), (("Number of IDs in the list is incorrect")), (UNITY_UINT)(331), UNITY_DISPLAY_STYLE_INT);





 Id_t rm_id = 0xFFFFFFFF;

 for(uint8_t i = 0; i < 5; i++) {

  rm_id = IdListIdRemove(&list);

  UnityAssertEqualNumber((UNITY_INT)((id_array[5 - (i + 1)])), (UNITY_INT)((rm_id)), (("Removed ID not equal to added ID.")), (UNITY_UINT)(337), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5 - (i + 1))), (("Number of items in the list incorrect.")), (UNITY_UINT)(338), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("Number of IDs in the list is not 0.")), (UNITY_UINT)(340), UNITY_DISPLAY_STYLE_INT);

}



void test_IdListIdRemove_all_plus_one(void)

{

 Id_t id_array[5] = {0xFFFFFFFF};

 Id_t id = 1;

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(349), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(351), UNITY_DISPLAY_STYLE_INT);

 }



 for(uint8_t i = 0; i < 5; i++) {

  id = i;

  id_array[i] = id;

  IdListIdAdd(&list, id);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((i + 1)), (("Number of items in the ID list incorrect.")), (UNITY_UINT)(358), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is incorrect.")), (UNITY_UINT)(359), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5)), (("Number of IDs in the list is incorrect")), (UNITY_UINT)(361), UNITY_DISPLAY_STYLE_INT);





 Id_t rm_id = 0xFFFFFFFF;

 for(uint8_t i = 0; i < 5; i++) {

  rm_id = IdListIdRemove(&list);

  UnityAssertEqualNumber((UNITY_INT)((id_array[5 - (i + 1)])), (UNITY_INT)((rm_id)), (("Removed ID not equal to added ID.")), (UNITY_UINT)(367), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5 - (i + 1))), (("Number of items in the list incorrect.")), (UNITY_UINT)(368), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("Number of IDs in the list is not 0.")), (UNITY_UINT)(370), UNITY_DISPLAY_STYLE_INT);



 rm_id = IdListIdRemove(&list);

 UnityAssertEqualNumber((UNITY_INT)((rm_id)), (UNITY_INT)((0xFFFFFFFF)), (("ID removed from empty list.")), (UNITY_UINT)(373), UNITY_DISPLAY_STYLE_INT);

}







void test_IdListCount_invalid_list(void)

{

 U8_t cnt = IdListCount(

                       ((void *)0)

                           );

 UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((0)), (("Valid count returned for an invalid list.")), (UNITY_UINT)(381), UNITY_DISPLAY_STYLE_INT);

}



void test_IdListCount_single(void)

{

 Id_t id = 1;

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(389), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(391), UNITY_DISPLAY_STYLE_INT);

 }



 U8_t cnt = IdListCount(&list);

 UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((0)), (("Non-empty count returned for an empty list.")), (UNITY_UINT)(395), UNITY_DISPLAY_STYLE_INT);



 IdListIdAdd(&list, id);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((1)), (("Expected 1 item in the ID list.")), (UNITY_UINT)(398), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is not correct.")), (UNITY_UINT)(399), UNITY_DISPLAY_STYLE_INT);



 cnt = IdListCount(&list);

 UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((1)), (("List count incorrect.")), (UNITY_UINT)(402), UNITY_DISPLAY_STYLE_INT);



 Id_t rm_id = IdListIdRemove(&list);

 UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((rm_id)), (("Removed ID not equal to added ID.")), (UNITY_UINT)(405), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("Number of items in the list incorrect.")), (UNITY_UINT)(406), UNITY_DISPLAY_STYLE_INT);



 cnt = IdListCount(&list);

 UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((0)), (("Non-empty count returned for an empty list.")), (UNITY_UINT)(409), UNITY_DISPLAY_STYLE_INT);





}



void test_IdListCount_all(void)

{

 Id_t id_array[5] = {0xFFFFFFFF};

 Id_t id = 1;

 IdList_t list;

 U8_t cnt = 0;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(421), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(423), UNITY_DISPLAY_STYLE_INT);

 }



 cnt = IdListCount(&list);

 UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((0)), (("Non-empty count returned for an empty list.")), (UNITY_UINT)(427), UNITY_DISPLAY_STYLE_INT);



 for(uint8_t i = 0; i < 5; i++) {

  id = i;

  id_array[i] = id;

  IdListIdAdd(&list, id);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((i + 1)), (("Number of items in the ID list incorrect.")), (UNITY_UINT)(433), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is incorrect.")), (UNITY_UINT)(434), UNITY_DISPLAY_STYLE_INT);

  cnt = IdListCount(&list);

  UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((i+1)), (("List count incorrect.")), (UNITY_UINT)(436), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5)), (("Number of IDs in the list is incorrect")), (UNITY_UINT)(438), UNITY_DISPLAY_STYLE_INT);

 cnt = IdListCount(&list);

 UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((5)), (("List count incorrect.")), (UNITY_UINT)(440), UNITY_DISPLAY_STYLE_INT);



 Id_t rm_id = 0xFFFFFFFF;

 for(uint8_t i = 0; i < 5; i++) {

  rm_id = IdListIdRemove(&list);

  UnityAssertEqualNumber((UNITY_INT)((id_array[5 - (i + 1)])), (UNITY_INT)((rm_id)), (("Removed ID not equal to added ID.")), (UNITY_UINT)(445), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5 - (i + 1))), (("Number of items in the list incorrect.")), (UNITY_UINT)(446), UNITY_DISPLAY_STYLE_INT);

  cnt = IdListCount(&list);

  UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((5 - (i + 1))), (("List count incorrect.")), (UNITY_UINT)(448), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("Number of IDs in the list is not 0.")), (UNITY_UINT)(450), UNITY_DISPLAY_STYLE_INT);

 cnt = IdListCount(&list);

 UnityAssertEqualNumber((UNITY_INT)((cnt)), (UNITY_INT)((0)), (("Non-empty count returned for an empty list.")), (UNITY_UINT)(452), UNITY_DISPLAY_STYLE_INT);





}









void test_IdListCopy_valid_lists(void)

{

 Id_t id = 1;

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (("List count was not initialized to ID_LIST_EMPTY.")), (UNITY_UINT)(465), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (("List IDs were not all initialized to ID_INVALID.")), (UNITY_UINT)(467), UNITY_DISPLAY_STYLE_INT);

 }



 for(uint8_t j = 0; j < 5; j++) {

  id = j;

  IdListIdAdd(&list, id);

  UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((j + 1)), (("Number of items in the ID list incorrect.")), (UNITY_UINT)(473), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)((list.ids[list.n - 1])), (UNITY_INT)((id)), (("ID in the list is incorrect.")), (UNITY_UINT)(474), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((5)), (("Number of IDs in the list is incorrect")), (UNITY_UINT)(476), UNITY_DISPLAY_STYLE_INT);



 IdList_t list_cpy;

 IdListInit(&list_cpy);



 IdListCopy(&list_cpy, &list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((list_cpy.n)), (("Item count is not equal after copy.")), (UNITY_UINT)(482), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t k = 0; k < 5; k++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[k])), (UNITY_INT)((list_cpy.ids[k])), (("IDs are not equal after copy.")), (UNITY_UINT)(484), UNITY_DISPLAY_STYLE_INT);

 }



}
