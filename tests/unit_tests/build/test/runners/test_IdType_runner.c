/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Test Runner Used To Run Each Test Below=====*/
#define RUN_TEST(TestFunc, TestLineNum) \
{ \
  Unity.CurrentTestName = #TestFunc; \
  Unity.CurrentTestLineNumber = TestLineNum; \
  Unity.NumberOfTests++; \
  if (TEST_PROTECT()) \
  { \
      setUp(); \
      TestFunc(); \
  } \
  if (TEST_PROTECT()) \
  { \
    tearDown(); \
  } \
  UnityConcludeTest(); \
}

/*=======Automagically Detected Files To Include=====*/
#ifdef __WIN32__
#define UNITY_INCLUDE_SETUP_STUBS
#endif
#include "unity.h"
#ifndef UNITY_EXCLUDE_SETJMP_H
#include <setjmp.h>
#endif
#include <stdio.h>

int GlobalExpectCount;
int GlobalVerifyOrder;
char* GlobalOrderError;

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test_KIdRequest_invalid_group(void);
extern void test_KIdRequest_valid_group_single(void);
extern void test_KIdRequest_valid_group_all(void);
extern void test_KIdRequest_all(void);
extern void test_KIdRequest_all_plus_one(void);
extern void test_IdSequenceNumberGet_valid(void);
extern void test_IdSequenceNumberGet_invalid_id(void);
extern void test_IdSequenceNumberGet_invalid_group(void);
extern void test_IdGroupGet_valid(void);
extern void test_IdGroupGet_invalid_group(void);
extern void test_IdIsInGroup_valid_true(void);
extern void test_IdIsInGroup_valid_false(void);
extern void test_IdIsInGroup_invalid_group(void);
extern void test_IdIsInGroup_invalid_id(void);
extern void test_IdIsInGroup_invalid_group_and_id(void);
extern void test_IdListInit_valid(void);
extern void test_IdListInit_invalid(void);
extern void test_IdListIdAdd_single(void);
extern void test_IdListIdAdd_all(void);
extern void test_IdListIdAdd_all_plus_one(void);
extern void test_IdListIdRemove_invalid_list(void);
extern void test_IdListIdRemove_single(void);
extern void test_IdListIdRemove_all(void);
extern void test_IdListIdRemove_all_plus_one(void);
extern void test_IdListCount_invalid_list(void);
extern void test_IdListCount_single(void);
extern void test_IdListCount_all(void);
extern void test_IdListCopy_valid_lists(void);


/*=======Suite Setup=====*/
static void suite_setup(void)
{
#if defined(UNITY_WEAK_ATTRIBUTE) || defined(UNITY_WEAK_PRAGMA)
  suiteSetUp();
#endif
}

/*=======Suite Teardown=====*/
static int suite_teardown(int num_failures)
{
#if defined(UNITY_WEAK_ATTRIBUTE) || defined(UNITY_WEAK_PRAGMA)
  return suiteTearDown(num_failures);
#else
  return num_failures;
#endif
}

/*=======Test Reset Option=====*/
void resetTest(void);
void resetTest(void)
{
  tearDown();
  setUp();
}


/*=======MAIN=====*/
int main(void)
{
  suite_setup();
  UnityBegin("test_IdType.c");
  RUN_TEST(test_KIdRequest_invalid_group, 54);
  RUN_TEST(test_KIdRequest_valid_group_single, 63);
  RUN_TEST(test_KIdRequest_valid_group_all, 73);
  RUN_TEST(test_KIdRequest_all, 86);
  RUN_TEST(test_KIdRequest_all_plus_one, 104);
  RUN_TEST(test_IdSequenceNumberGet_valid, 127);
  RUN_TEST(test_IdSequenceNumberGet_invalid_id, 135);
  RUN_TEST(test_IdSequenceNumberGet_invalid_group, 143);
  RUN_TEST(test_IdGroupGet_valid, 156);
  RUN_TEST(test_IdGroupGet_invalid_group, 163);
  RUN_TEST(test_IdIsInGroup_valid_true, 176);
  RUN_TEST(test_IdIsInGroup_valid_false, 183);
  RUN_TEST(test_IdIsInGroup_invalid_group, 190);
  RUN_TEST(test_IdIsInGroup_invalid_id, 197);
  RUN_TEST(test_IdIsInGroup_invalid_group_and_id, 204);
  RUN_TEST(test_IdListInit_valid, 213);
  RUN_TEST(test_IdListInit_invalid, 223);
  RUN_TEST(test_IdListIdAdd_single, 230);
  RUN_TEST(test_IdListIdAdd_all, 245);
  RUN_TEST(test_IdListIdAdd_all_plus_one, 264);
  RUN_TEST(test_IdListIdRemove_invalid_list, 288);
  RUN_TEST(test_IdListIdRemove_single, 294);
  RUN_TEST(test_IdListIdRemove_all, 313);
  RUN_TEST(test_IdListIdRemove_all_plus_one, 343);
  RUN_TEST(test_IdListCount_invalid_list, 378);
  RUN_TEST(test_IdListCount_single, 384);
  RUN_TEST(test_IdListCount_all, 414);
  RUN_TEST(test_IdListCopy_valid_lists, 460);

  return suite_teardown(UnityEnd());
}
