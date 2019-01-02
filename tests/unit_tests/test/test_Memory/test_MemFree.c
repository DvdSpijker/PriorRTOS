/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
 
/* Test framework */
#include "unity.h"
 
/* UUT */
#include "Memory.h"
#include "MemoryDef.h"

/* Dependencies */
#include "mock_CoreDef.h"
#include "mock_LoggerDef.h"
#include "mock_Logger.h"
#include "mock_Os.h"

/* Other */
#include <string.h> /* For memset. */

 
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

void test_MemFree_invalid_pointer(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	res = MemFree(NULL);
	
	TEST_ASSERT_EQUAL(OS_RES_INVALID_ARGUMENT, res);
}

void test_MemFree_oob_pointer(void)
{
	OsResult_t res = OS_RES_ERROR;
	void *ptr = NULL;
	
	ptr = &TestHeap[TEST_HEAP_SIZE] + 1;
	
	res = MemFree((void **)&ptr);
	
	TEST_ASSERT_EQUAL(OS_RES_ERROR, res);
}

void test_MemFree_obj_pool_no_kernel_mode(void)
{
	OsResult_t res = OS_RES_ERROR;
	void *ptr = NULL;
	
	LogError_Ignore();
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 1);
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();

	ptr = MemAlloc(OBJ_POOL_ID, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_NOT_NULL(ptr);
	
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 0);
	
	res = MemFree(&ptr);
	
	TEST_ASSERT_EQUAL(OS_RES_RESTRICTED, res);
}

void test_MemFree_alloc_free_single(void)
{
	OsResult_t res = OS_RES_ERROR;
	void *ptr = NULL;
	U8_t *ptr_cpy = NULL;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();

	ptr = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_NOT_NULL(ptr);
	ptr_cpy = ptr;
	
	memset(ptr, 1, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = MemFree(&ptr);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_EQUAL(NULL, ptr);
	TEST_ASSERT_EACH_EQUAL_UINT8(0, ptr_cpy, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_EQUAL(0, TestPoolTable[TestPool].N);
	TEST_ASSERT_EQUAL(TestPoolTable[TestPool].pool_size, TestPoolTable[TestPool].mem_left);
}

void test_MemFree_alloc_three_free_middle(void)
{
	OsResult_t res = OS_RES_ERROR;
	void *ptr = NULL;
	U8_t *ptr_cpy = NULL;
	void *alloc_0 = NULL;
	void *alloc_1 = NULL;
	U32_t alloc_size_0 = 0;
	U32_t alloc_size_1 = 0;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	alloc_0 = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_NOT_NULL(alloc_0);
	alloc_size_0 = MemAllocSizeGet(alloc_0);

	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	ptr = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_NOT_NULL(ptr);
	ptr_cpy = ptr;
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	alloc_1 = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_NOT_NULL(alloc_1);
	alloc_size_1 = MemAllocSizeGet(alloc_1);
	

	memset(ptr, 1, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	memset(alloc_0, 1, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	memset(alloc_1, 1, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = MemFree(&ptr);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_EQUAL(NULL, ptr);
	TEST_ASSERT_EACH_EQUAL_UINT8(0, ptr_cpy, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_EACH_EQUAL_UINT8(1, alloc_0, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_EACH_EQUAL_UINT8(1, alloc_1, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_EQUAL(alloc_size_0, MemAllocSizeGet(alloc_0));
	TEST_ASSERT_EQUAL(alloc_size_1, MemAllocSizeGet(alloc_1));
	TEST_ASSERT_EQUAL(2, TestPoolTable[TestPool].N);
}

void test_MemAlloc_alloc_free_max(void)
{
	OsResult_t res = OS_RES_ERROR;
	void *ptrs[TEST_POOL_SIZE / MEM_BLOCK_SIZE] = {NULL};
	void *ptr_cpy = NULL;
	int i = 0;
	U32_t n_alloc = 0;
	U32_t mem_left = 0;
	
	LogError_Ignore();
	
	do {		
		OsCritSectBegin_Ignore();
		OsCritSectEnd_Ignore();
		
		ptrs[i] = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);

		if(ptrs[i] == NULL) {
			break;
		}
		
		memset(ptrs[i], 1, TEST_ALLOC_SIZE_SINGLE_BLOCK);
		
		i++;
	} while(1);
	
	TEST_ASSERT_EQUAL(TEST_POOL_SIZE / MEM_BLOCK_SIZE, i);
	n_alloc = TestPoolTable[TestPool].N;
	mem_left = TestPoolTable[TestPool].mem_left;
	
	for(i=i-1; i >= 0; i--) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		
		ptr_cpy = ptrs[i];
		
		res = MemFree(&ptrs[i]);
		
		n_alloc--;
		mem_left += TEST_ALLOC_SIZE_SINGLE_BLOCK + MEM_ALLOC_SIZE_SIZE_BYTES;
		
		TEST_ASSERT_EQUAL(OS_RES_OK, res);
		TEST_ASSERT_EQUAL(NULL, ptrs[i]);
		TEST_ASSERT_EACH_EQUAL_UINT8(0, ptr_cpy, TEST_ALLOC_SIZE_SINGLE_BLOCK);
		TEST_ASSERT_EQUAL(n_alloc, TestPoolTable[TestPool].N);
		TEST_ASSERT_EQUAL(mem_left, TestPoolTable[TestPool].mem_left);	
	}
	
	TEST_ASSERT_EQUAL(0, TestPoolTable[TestPool].N);
	TEST_ASSERT_EQUAL(TestPoolTable[TestPool].pool_size, TestPoolTable[TestPool].mem_left);	
}
