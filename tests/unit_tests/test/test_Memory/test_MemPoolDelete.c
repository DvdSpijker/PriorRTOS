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

void test_MemPoolDelete_invalid_id(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = ID_INVALID;
	
	LogError_Ignore();
	
	res = MemPoolDelete(id);
	
	TEST_ASSERT_EQUAL(OS_RES_INVALID_ID, res);
	
	id = MEM_NUM_POOLS + 1;
	
	res = MemPoolDelete(id);
	
	TEST_ASSERT_EQUAL(OS_RES_INVALID_ID, res);
}

void test_MemPoolDelete_kernel_pool(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = KERNEL_POOL_ID;
	
	LogError_Ignore();
	
	res = MemPoolDelete(id);
	
	TEST_ASSERT_EQUAL(OS_RES_RESTRICTED, res);
}

void test_MemPoolDelete_obj_pool(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = OBJ_POOL_ID;
	
	LogError_Ignore();
	
	res = MemPoolDelete(id);
	
	TEST_ASSERT_EQUAL(OS_RES_RESTRICTED, res);
}

void test_MemPoolDelete_single(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = ID_INVALID;
	
	LogError_Ignore();
	OsCritSectBegin_Ignore();
	OsCritSectEnd_Ignore();
	
	id = MemPoolCreate(TEST_POOL_SIZE);	
	
	res = MemPoolDelete(id);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_EQUAL(0, TestPoolTable[id].pool_size);
	TEST_ASSERT_EQUAL(0, TestPoolTable[id].start_index);
}

void test_MemPoolDelete_max(void)
{
	OsResult_t res = OS_RES_ERROR;
	Id_t id = ID_INVALID;
	Id_t exp_id = OBJ_POOL_ID + 1;
	
	LogError_Ignore();
	OsCritSectBegin_Ignore();
	OsCritSectEnd_Ignore();
	
	for(; exp_id < MEM_NUM_POOLS; exp_id++) {
		printf("Pool ID: %u\n", exp_id);
		id = MemPoolCreate(TEST_POOL_SIZE);
		TEST_ASSERT_EQUAL(exp_id, id);
		
		res = MemPoolDelete(id);
		
		TEST_ASSERT_EQUAL(OS_RES_OK, res);
		TEST_ASSERT_EQUAL(0, TestPoolTable[id].pool_size);
		TEST_ASSERT_EQUAL(0, TestPoolTable[id].start_index);
	}

}
