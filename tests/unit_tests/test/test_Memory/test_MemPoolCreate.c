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


void test_MemPoolCreate_pool_size_zero(void)
{
	Id_t id = ID_INVALID;
	
	LogError_Ignore();
	
	id = MemPoolCreate(0);
	
	TEST_ASSERT_EQUAL(id, ID_INVALID);
}

void test_MemPoolCreate_pool_greater_pool(void)
{
	Id_t id = ID_INVALID;
	
	LogError_Ignore();
	
	id = MemPoolCreate(TEST_HEAP_SIZE + 1);
	
	TEST_ASSERT_EQUAL(id, ID_INVALID);
}

void test_MemPoolCreate_no_pool_available(void)
{
	Id_t id = ID_INVALID;
	
	for(int i = 0; i < MEM_NUM_POOLS; i++) {
		TestPoolTable[i].pool_size = 1;
	}

	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	id = MemPoolCreate(TEST_POOL_SIZE);
	
	TEST_ASSERT_EQUAL(id, ID_INVALID);
}

void test_MemPoolCreate_single(void)
{
	Id_t id = ID_INVALID;
	Id_t exp_id = OBJ_POOL_ID + 1;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	id = MemPoolCreate(TEST_POOL_SIZE);
	
	TEST_ASSERT_EQUAL(id, exp_id);
	TEST_ASSERT_EQUAL(TestPoolTable[id].pool_size, TEST_POOL_SIZE);
	TEST_ASSERT_EQUAL(TestPoolTable[id].mem_left, TEST_POOL_SIZE);
	TEST_ASSERT_EQUAL(TestPoolTable[id].start_index, TestPoolTable[OBJ_POOL_ID].end_index + 1);
	TEST_ASSERT_EQUAL(TestPoolTable[id].end_index, TestPoolTable[id].start_index + TEST_POOL_SIZE - 1);
}

void test_MemPoolCreate_max(void)
{
	Id_t id = ID_INVALID;
	Id_t exp_id = OBJ_POOL_ID + 1;
	
	LogError_Ignore();
	
	for(; exp_id < MEM_NUM_POOLS; exp_id++) { 
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		
		id = MemPoolCreate(TEST_POOL_SIZE);
		
		TEST_ASSERT_EQUAL(id, exp_id);
		TEST_ASSERT_EQUAL(TestPoolTable[id].pool_size, TEST_POOL_SIZE);
		TEST_ASSERT_EQUAL(TestPoolTable[id].mem_left, TEST_POOL_SIZE);
		TEST_ASSERT_EQUAL(TestPoolTable[id].start_index, TestPoolTable[exp_id - 1].end_index + 1);
		TEST_ASSERT_EQUAL(TestPoolTable[id].end_index, TestPoolTable[id].start_index + TEST_POOL_SIZE - 1);		
	}

}

void test_MemPoolCreate_no_space(void)
{
	Id_t id = ID_INVALID;
	Id_t exp_id = OBJ_POOL_ID + 1;
	
	LogError_Ignore();
	
	for(; exp_id < MEM_NUM_POOLS - 1; exp_id++) { 
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		
		id = MemPoolCreate(TEST_POOL_SIZE);
		
		TEST_ASSERT_EQUAL(id, exp_id);
		TEST_ASSERT_EQUAL(TestPoolTable[id].pool_size, TEST_POOL_SIZE);
		TEST_ASSERT_EQUAL(TestPoolTable[id].mem_left, TEST_POOL_SIZE);
		TEST_ASSERT_EQUAL(TestPoolTable[id].start_index, TestPoolTable[exp_id - 1].end_index + 1);
		TEST_ASSERT_EQUAL(TestPoolTable[id].end_index, TestPoolTable[id].start_index + TEST_POOL_SIZE - 1);		
	}
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	id = MemPoolCreate(TEST_HEAP_SIZE - TEST_POOL_SIZE);
	
	TEST_ASSERT_EQUAL(id, ID_INVALID);
	TEST_ASSERT_EQUAL(TestPoolTable[exp_id].pool_size, 0);
}
