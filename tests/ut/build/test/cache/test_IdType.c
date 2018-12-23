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

), (UNITY_UINT)(54), UNITY_DISPLAY_STYLE_INT);



 id = KIdRequest(-1);

 UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_INT);

}



void test_KIdRequest_valid_group_single(void)

{

 const Id_t exp_seq = 0;



 Id_t id = KIdRequest(ID_GROUP_MAILBOX);

 if (((id) != (0xFFFFFFFF))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(65)));};

 UnityAssertEqualNumber((UNITY_INT)(((id & 0xFF000000))), (UNITY_INT)((((Id_t)(ID_GROUP_MAILBOX << 24) & 0xFF000000))), (

((void *)0)

), (UNITY_UINT)(66), UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((UNITY_INT)(((id & 0x00FFFFFF))), (UNITY_INT)(((exp_seq & 0x00FFFFFF))), (

((void *)0)

), (UNITY_UINT)(67), UNITY_DISPLAY_STYLE_INT);

}



void test_KIdRequest_valid_group_all(void)

{

 Id_t exp_seq = 0;

 Id_t id = 0xFFFFFFFF;



 for(int i = 0; i < ID_GROUP_NUM; i++) {

  id = KIdRequest(i);

  if (((id) != (0xFFFFFFFF))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(77)));};

  UnityAssertEqualNumber((UNITY_INT)(((id & 0xFF000000))), (UNITY_INT)((((Id_t)(i << 24) & 0xFF000000))), (

 ((void *)0)

 ), (UNITY_UINT)(78), UNITY_DISPLAY_STYLE_INT);

  UnityAssertEqualNumber((UNITY_INT)(((id & 0x00FFFFFF))), (UNITY_INT)(((exp_seq & 0x00FFFFFF))), (

 ((void *)0)

 ), (UNITY_UINT)(79), UNITY_DISPLAY_STYLE_INT);

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

   if (((id) != (0xFFFFFFFF))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(92)));};

   UnityAssertEqualNumber((UNITY_INT)(((id & 0xFF000000))), (UNITY_INT)((((Id_t)(i << 24) & 0xFF000000))), (

  ((void *)0)

  ), (UNITY_UINT)(93), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(((id & 0x00FFFFFF))), (UNITY_INT)(((j & 0x00FFFFFF))), (

  ((void *)0)

  ), (UNITY_UINT)(94), UNITY_DISPLAY_STYLE_INT);

  }

 }

 UnityAssertEqualNumber((UNITY_INT)((id_cnt)), (UNITY_INT)((0x00FFFFFF * ID_GROUP_NUM)), (

((void *)0)

), (UNITY_UINT)(97), UNITY_DISPLAY_STYLE_INT);

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

   if (((id) != (0xFFFFFFFF))) {} else {UnityFail( ((" Expected Not-Equal")), (UNITY_UINT)((UNITY_UINT)(110)));};

   UnityAssertEqualNumber((UNITY_INT)(((id & 0xFF000000))), (UNITY_INT)((((Id_t)(i << 24) & 0xFF000000))), (

  ((void *)0)

  ), (UNITY_UINT)(111), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(((id & 0x00FFFFFF))), (UNITY_INT)(((j & 0x00FFFFFF))), (

  ((void *)0)

  ), (UNITY_UINT)(112), UNITY_DISPLAY_STYLE_INT);

  }

  id_cnt++;

  id = KIdRequest(i);

  UnityAssertEqualNumber((UNITY_INT)((id)), (UNITY_INT)((0xFFFFFFFF)), (

 ((void *)0)

 ), (UNITY_UINT)(116), UNITY_DISPLAY_STYLE_INT);

 }

 UnityAssertEqualNumber((UNITY_INT)((id_cnt)), (UNITY_INT)(((0x00FFFFFF + 1) * ID_GROUP_NUM)), (

((void *)0)

), (UNITY_UINT)(118), UNITY_DISPLAY_STYLE_INT);

 printf("Number of IDs: %u\n", id_cnt);

}







void test_IdSequenceNumberGet_valid(void)

{

 const Id_t id = ID_GROUP_MESSAGE_QUEUE | 0x00002000;



 Id_t id_seq = IdSequenceNumberGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_seq)), (UNITY_INT)(((id & 0x00FFFFFF))), (

((void *)0)

), (UNITY_UINT)(129), UNITY_DISPLAY_STYLE_INT);

}



void test_IdSequenceNumberGet_invalid_id(void)

{

 const Id_t id = 0xFFFFFFFF;



 Id_t id_seq = IdSequenceNumberGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_seq)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(137), UNITY_DISPLAY_STYLE_INT);

}



void test_IdSequenceNumberGet_invalid_group(void)

{

 Id_t id = 0xFF000000 | 0x00001000;

 Id_t id_seq = IdSequenceNumberGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_seq)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(144), UNITY_DISPLAY_STYLE_INT);



 id = (ID_GROUP_NUM + 1) << 24 | 0x00001000;

 id_seq = IdSequenceNumberGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_seq)), (UNITY_INT)((0xFFFFFFFF)), (

((void *)0)

), (UNITY_UINT)(148), UNITY_DISPLAY_STYLE_INT);

}







void test_IdGroupGet_valid(void)

{

 const Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << 24) | 0x00002000;

 IdGroup_t id_group = IdGroupGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_group)), (UNITY_INT)((ID_GROUP_MESSAGE_QUEUE)), (

((void *)0)

), (UNITY_UINT)(157), UNITY_DISPLAY_STYLE_INT);

}



void test_IdGroupGet_invalid_group(void)

{

 Id_t id = 0xFF000000 | 0x00001000;

 IdGroup_t id_group = IdGroupGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_group)), (UNITY_INT)((ID_GROUP_INV)), (

((void *)0)

), (UNITY_UINT)(164), UNITY_DISPLAY_STYLE_INT);



 id = (ID_GROUP_NUM + 1) << 24 | 0x00001000;

 id_group = IdGroupGet(id);

 UnityAssertEqualNumber((UNITY_INT)((id_group)), (UNITY_INT)((ID_GROUP_INV)), (

((void *)0)

), (UNITY_UINT)(168), UNITY_DISPLAY_STYLE_INT);

}







void test_IdIsInGroup_valid_true(void)

{

 Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << 24) | 0x00002000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_MESSAGE_QUEUE);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((1)), (

((void *)0)

), (UNITY_UINT)(177), UNITY_DISPLAY_STYLE_INT);

}



void test_IdIsInGroup_valid_false(void)

{

 Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << 24) | 0x00002000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_EVENTGROUP);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(184), UNITY_DISPLAY_STYLE_INT);

}



void test_IdIsInGroup_invalid_group(void)

{

 Id_t id = ((Id_t)ID_GROUP_MESSAGE_QUEUE << 24) | 0x00002000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_NUM);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(191), UNITY_DISPLAY_STYLE_INT);

}



void test_IdIsInGroup_invalid_id(void)

{

 Id_t id = 0xFF000000 | 0x00001000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_EVENTGROUP);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(198), UNITY_DISPLAY_STYLE_INT);

}



void test_IdIsInGroup_invalid_group_and_id(void)

{

 Id_t id = 0xFF000000 | 0x00001000;

 U8_t in_group = IdIsInGroup(id, ID_GROUP_NUM);

 UnityAssertEqualNumber((UNITY_INT)((in_group)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(205), UNITY_DISPLAY_STYLE_INT);

}







void test_IdListInit_valid(void)

{

 IdList_t list;

 IdListInit(&list);

 UnityAssertEqualNumber((UNITY_INT)((list.n)), (UNITY_INT)((0)), (

((void *)0)

), (UNITY_UINT)(214), UNITY_DISPLAY_STYLE_INT);

 for(uint8_t i = 0; i < 5; i++) {

  UnityAssertEqualNumber((UNITY_INT)((list.ids[i])), (UNITY_INT)((0xFFFFFFFF)), (

 ((void *)0)

 ), (UNITY_UINT)(216), UNITY_DISPLAY_STYLE_INT);

 }

}



void test_IdListInit_invalid(void)

{

 IdListInit(

           ((void *)0)

               );

}
