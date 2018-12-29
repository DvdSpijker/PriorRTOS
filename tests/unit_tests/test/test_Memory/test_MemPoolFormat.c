/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
 
/* Test framework */
#include "unity.h"
 
#include "mock_CoreDef.h"
#include "mock_LoggerDef.h"
#include "mock_Logger.h"
#include "mock_Os.h"

/* UUT */
#include "Memory.h"
#include "MemoryDef.h"

/* Dependencies */

/* Other */

 
/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/
#define TEST_HEAP_SIZE	0x100
#define TEST_USER_HEAP_SIZE 0x50
#define TEST_POOL_SIZE 0x10

#define KERNEL_POOL_ID 0
#define OBJ_POOL_ID 1

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/
 
/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/
 
Pmb_t TestPoolTable[MEM_NUM_POOLS];
U8_t TestHeap[TEST_HEAP_SIZE];
 
/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
 
/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/
 
void setUp(void)
{
	KLogInfo_Ignore();
	LogError_Ignore();
	KCoreKernelModeEnter_IgnoreAndReturn(1);
	KCoreKernelModeExit_IgnoreAndReturn(0);
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	TEST_ASSERT_EQUAL(KMemInit(TestHeap, TEST_HEAP_SIZE, 
			TEST_USER_HEAP_SIZE, TestPoolTable), OS_RES_OK);
}
 
void tearDown(void)
{
}
 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_MemPoolFormat_invalid_id(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = ID_INVALID;
	
	LogError_Ignore();
	
	res = MemPoolFormat(id);
	
	TEST_ASSERT_EQUAL(OS_RES_INVALID_ID, res);
	
	id = MEM_NUM_POOLS + 1;
	
	res = MemPoolFormat(id);
	
	TEST_ASSERT_EQUAL(OS_RES_INVALID_ID, res);
}

void test_MemPoolFormat_kernel_pool_no_kernel_mode(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = KERNEL_POOL_ID;
	
	LogError_Ignore();
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 0);

	res = MemPoolFormat(id);
	
	TEST_ASSERT_EQUAL(OS_RES_RESTRICTED, res);
}

void test_MemPoolFormat_obj_pool_no_kernel_mode(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = OBJ_POOL_ID;
	
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 0);
	LogError_Ignore();
	
	res = MemPoolFormat(id);
	
	TEST_ASSERT_EQUAL(OS_RES_RESTRICTED, res);
}

void test_MemPoolFormat_single(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = ID_INVALID;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();

	id = MemPoolCreate(TEST_POOL_SIZE);
	TEST_ASSERT_NOT_EQUAL(ID_INVALID, id);
	
	for(int i = TestPoolTable[id].start_index; i < TestPoolTable[id].end_index; i++) {
		TestHeap[i] = 0xFF;
	}
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = MemPoolFormat(id);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_EQUAL(TestPoolTable[id].pool_size, TestPoolTable[id].mem_left);
	TEST_ASSERT_EQUAL(0, TestPoolTable[id].N);
	for(int i = TestPoolTable[id].start_index; i < TestPoolTable[id].end_index; i++) {
		TEST_ASSERT_EQUAL(0, TestHeap[i]);
	}	
}

void test_MemPoolFormat_kernel_pool_w_kernel_mode(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = KERNEL_POOL_ID;
	
	LogError_Ignore();
	
	for(int i = TestPoolTable[id].start_index; i < TestPoolTable[id].end_index; i++) {
		TestHeap[i] = 0xFF;
	}
	
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 1);
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = MemPoolFormat(id);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_EQUAL(TestPoolTable[id].pool_size, TestPoolTable[id].mem_left);
	TEST_ASSERT_EQUAL(0, TestPoolTable[id].N);
	for(int i = TestPoolTable[id].start_index; i < TestPoolTable[id].end_index; i++) {
		TEST_ASSERT_EQUAL(0, TestHeap[i]);
	}	
}

void test_MemPoolFormat_obj_pool_w_kernel_mode(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = KERNEL_POOL_ID;
	
	LogError_Ignore();
	
	for(int i = TestPoolTable[id].start_index; i < TestPoolTable[id].end_index; i++) {
		TestHeap[i] = 0xFF;
	}
	
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 1);
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = MemPoolFormat(id);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_EQUAL(TestPoolTable[id].pool_size, TestPoolTable[id].mem_left);
	TEST_ASSERT_EQUAL(0, TestPoolTable[id].N);
	for(int i = TestPoolTable[id].start_index; i < TestPoolTable[id].end_index; i++) {
		TEST_ASSERT_EQUAL(0, TestHeap[i]);
	}	
}


