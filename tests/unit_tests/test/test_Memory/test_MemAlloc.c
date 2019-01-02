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
#define TEST_HEAP_SIZE	MEM_BLOCK_SIZE * 15
#define TEST_USER_HEAP_SIZE MEM_BLOCK_SIZE * 12
#define TEST_POOL_SIZE MEM_BLOCK_SIZE * 12

#define TEST_ALLOC_SIZE_SINGLE_BLOCK MEM_BLOCK_SIZE - MEM_ALLOC_SIZE_SIZE_BYTES
#define TEST_ALLOC_SIZE_MULTI_BLOCK TEST_ALLOC_SIZE_SINGLE_BLOCK + MEM_BLOCK_SIZE

#define OBJ_POOL_ID 0

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/
 
/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/
 
Pmb_t TestPoolTable[MEM_NUM_POOLS];
U8_t TestHeap[TEST_HEAP_SIZE];
Id_t TestPool = ID_INVALID;
 
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
	
	TEST_ASSERT_EQUAL(KMemInit(TestHeap, TEST_HEAP_SIZE, 
			TEST_USER_HEAP_SIZE, TestPoolTable), OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();

	TestPool = MemPoolCreate(TEST_POOL_SIZE);
	TEST_ASSERT_NOT_EQUAL(ID_INVALID, TestPool);
}
 
void tearDown(void)
{
	TestPool = ID_INVALID;
}
 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_MemAlloc_invalid_id(void)
{
	void *addr = NULL;
	
	LogError_Ignore();
	
	addr = MemAlloc(ID_INVALID, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	TEST_ASSERT_EQUAL(NULL, addr);

	addr = MemAlloc(MEM_NUM_POOLS + 1, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	TEST_ASSERT_EQUAL(NULL, addr);
}

void test_MemAlloc_invalid_size(void)
{
	void *addr = NULL;
	
	LogError_Ignore();
	
	addr = MemAlloc(TestPool, 0);
	
	TEST_ASSERT_EQUAL(NULL, addr);
}

void test_MemAlloc_no_space(void)
{
	void *addr = NULL;
	
	LogError_Ignore();
	
	TestPoolTable[TestPool].mem_left = TEST_ALLOC_SIZE_SINGLE_BLOCK - 1;
	
	addr = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	TEST_ASSERT_EQUAL(NULL, addr);
}

void test_MemAlloc_obj_pool_no_kernel_mode(void)
{
	void *addr = NULL;
	
	LogError_Ignore();
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 0);

	addr = MemAlloc(OBJ_POOL_ID, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	TEST_ASSERT_EQUAL(NULL, addr);
}

void test_MemAlloc_single_block_single_alloc(void)
{
	void *addr = NULL;
	U32_t alloc_size = 0;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	addr = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	alloc_size = *((U32_t *)&TestHeap[TestPoolTable[TestPool].start_index]);
	
	TEST_ASSERT_EQUAL(&TestHeap[TestPoolTable[TestPool].start_index + MEM_ALLOC_DATA_OFFSET], addr);
	TEST_ASSERT_EQUAL(TEST_ALLOC_SIZE_SINGLE_BLOCK + MEM_ALLOC_SIZE_SIZE_BYTES, alloc_size);
	TEST_ASSERT_EQUAL(1, TestPoolTable[TestPool].N);
	TEST_ASSERT_EQUAL(TEST_POOL_SIZE - alloc_size, TestPoolTable[TestPool].mem_left);
}

void test_MemAlloc_single_block_double_alloc(void)
{
	void *addr = NULL;
	U32_t alloc_size = 0;
	U32_t mem_index = 0;
	U32_t mem_left = 0;
	
	LogError_Ignore();
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	addr = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	alloc_size = *((U32_t *)&TestHeap[TestPoolTable[TestPool].start_index]);
	
	TEST_ASSERT_EQUAL(&TestHeap[TestPoolTable[TestPool].start_index + MEM_ALLOC_DATA_OFFSET], addr);
	TEST_ASSERT_EQUAL(TEST_ALLOC_SIZE_SINGLE_BLOCK + MEM_ALLOC_SIZE_SIZE_BYTES, alloc_size);
	TEST_ASSERT_EQUAL(1, TestPoolTable[TestPool].N);
	TEST_ASSERT_EQUAL(TEST_POOL_SIZE - alloc_size, TestPoolTable[TestPool].mem_left);
	mem_index = TestPoolTable[TestPool].start_index + alloc_size;
	mem_left = TestPoolTable[TestPool].mem_left;
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	addr = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	alloc_size = *((U32_t *)&TestHeap[mem_index]);
	
	TEST_ASSERT_EQUAL(&TestHeap[mem_index + MEM_ALLOC_DATA_OFFSET], addr);
	TEST_ASSERT_EQUAL(MEM_BLOCK_SIZE, alloc_size);
	TEST_ASSERT_EQUAL(2, TestPoolTable[TestPool].N);
	TEST_ASSERT_EQUAL(mem_left - alloc_size, TestPoolTable[TestPool].mem_left);
}

void test_MemAlloc_single_block_max_alloc(void)
{
	void *addr = NULL;
	U32_t alloc_size = 0;
	U32_t mem_index = TestPoolTable[TestPool].start_index;
	U32_t mem_left = TestPoolTable[TestPool].mem_left;
	int i = 0;
	
	LogError_Ignore();
	
	do {		
		OsCritSectBegin_Ignore();
		OsCritSectEnd_Ignore();
		
		addr = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
		
		if(addr == NULL) {
			break;
		}
		
		alloc_size = *((U32_t *)&TestHeap[mem_index]);
		
		TEST_ASSERT_EQUAL(&TestHeap[mem_index + MEM_ALLOC_DATA_OFFSET], addr);
		TEST_ASSERT_EQUAL(TEST_ALLOC_SIZE_SINGLE_BLOCK + MEM_ALLOC_SIZE_SIZE_BYTES, alloc_size);
		TEST_ASSERT_EQUAL(i + 1, TestPoolTable[TestPool].N);
		TEST_ASSERT_EQUAL(mem_left - alloc_size, TestPoolTable[TestPool].mem_left);
		
		mem_index += alloc_size;
		mem_left = TestPoolTable[TestPool].mem_left;
		alloc_size = *((U32_t *)&TestHeap[mem_index]);
		
		i++;
	} while(addr != NULL);
	
	TEST_ASSERT_EQUAL(i, TEST_POOL_SIZE / MEM_BLOCK_SIZE);
}

void test_MemAlloc_multi_block_alloc_after_free(void)
{
	void *addr = NULL;
	U32_t alloc_size = 0;
	U32_t index = TestPoolTable[TestPool].start_index + MEM_BLOCK_SIZE;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	TestHeap[index] = MEM_BLOCK_SIZE;
	index += MEM_BLOCK_SIZE;
	
	addr = MemAlloc(TestPool, TEST_ALLOC_SIZE_MULTI_BLOCK);
	
	alloc_size = *((U32_t *)&TestHeap[index]);
	
	TEST_ASSERT_EQUAL(&TestHeap[index + MEM_ALLOC_DATA_OFFSET], addr);
	TEST_ASSERT_EQUAL(TEST_ALLOC_SIZE_MULTI_BLOCK + MEM_ALLOC_SIZE_SIZE_BYTES, alloc_size);
	TEST_ASSERT_EQUAL(1, TestPoolTable[TestPool].N);
	TEST_ASSERT_EQUAL(TEST_POOL_SIZE - alloc_size, TestPoolTable[TestPool].mem_left);	
}


