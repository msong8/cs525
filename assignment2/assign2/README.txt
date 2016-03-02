=========================
#   Personal Information   #
=========================
1)Miao Song   msong8@hawk.iit.edu,   A20242394: representative
2)Chris Dani     cdani@hawk.iit.edu       A20372828
3)Nikhita Kataria  nkataria@hawk.iit.edu  A20327878
4)Jonathan Yang   jyang28@hawk.iit.edu     A20143892

=========================
#   File List   #
=========================
README.txt
buffer_mgr.c
buffer_mgr.h
buffer_mgr_stat.c
buffer_mgr_stat.h
dberror.c
dberror.h
dt.h
storage_mgr.c
storage_mgr.h
test_assign2_1.c
test_assign2_customize.c
test_helper.h
Makefile



=========================
#   Mile Stone  #
=========================
1. Completed the task breakdown for teammembers- Feb. 20
2. Completed the task implementation - Feb. 24
3. Completed the function header editing - Feb. 25
4. Completed the debugging and passed the test case - Feb. 27
5. Compelted the Clock replacment algorithm for extra credit and Readme file - Mar. 1


=========================
# Installation Instuctions  #
=========================
1. In the terminal, type in "make", the makefile script will generate two outputs "test_assign" and "test_customize_clock";
2. Type in "./test_assign" to run the test_assign2_1.c test file for testing;
3. Type in "./test_customize_clock" to run the test_assign2_customize.c test file for testing.


==========================================================
# Function descriptions: of all additional functions  #
==========================================================
1)
/************************************************************************
Function Name: init_BP_mgmt
Description:
	Initializes the variables of the BP_mgmt * mgmtData object with the 
	passed in pool_size.
Parameters:
	BP_mgmt * mgmtData, int pool_size
Return:
	None/Void
Author:
	Miao Song
HISTORY:
	Date		Name		Content
	2016-02-24	Miao Song	Written code
	2016-02-25	Jon Yang	Added function header comment
************************************************************************/

void init_BP_mgmt (BP_mgmt *mgmtData, int pool_size){}

2)
/************************************************************************
Function Name: destroy_BP_mgmt
Description:
	Deallocates memory assigned to the BP_mgmt * mgmtData object and 
	resets all other variables to NULL.
Parameters:
	BP_mgmt * mgmtData
Return:
	None/Void
Author:
	Miao Song
HISTORY:
	Date		Name		Content
	2016-02-24	Miao Song	Written code
	2016-02-25	Jon Yang	Added function header comment
************************************************************************/
void destroy_BP_mgmt(BP_mgmt * mgmtData){}

3)
/************************************************************************
Function Name: insert_into_bookkeepinglist
Description:
	This function inserts a frame's page and pool index into the 
	buffer pool's mgmtData linked list for internal tracking.
Parameters:
	int page_index, int pool_index, BM_BufferPool *const bm
Return:
	None/Void
Author:
	Miao Song
HISTORY:
	Date		Name		Content
	2016-02-24	Miao Song	Written code
	2016-02-25	Jon Yang	Added function header comment
************************************************************************/

void insert_into_bookkeepinglist(int page_index, int pool_index, BM_BufferPool *const bm){}

4)
/************************************************************************
Function Name: check_in_cache
Description:
	Checks the buffer pool's mgmtData linked list for the frame with the 
	passed in page index.
Parameters:
	int page_index, BM_BufferPool *const bm
Return:
	Returns RC_OK if frame is found
	Returns RC_PAGE_NOT_FOUND_IN_CACHE if frame is not found
Author:
	Miao Song
HISTORY:
	Date		Name		Content
	2016-02-24	Miao Song	Written code
	2016-02-25	Jon Yang	Added function header comment
************************************************************************/
RC check_in_cache(int page_index, BM_BufferPool *const bm){
	Bookkeeping4Swap *current = NULL;
	current = head(bm);
	while (current!=NULL){
		if (current->page_index == page_index){
			return RC_OK;
		}
		current = current->next;
	}
	return RC_PAGE_NOT_FOUND_IN_CACHE;
}

5)
/************************************************************************
Function Name: pageindex_mapto_poolindex
Description:
	Iterates over the buffer pool's mgmtData linked list until a match 
	is found with the passed in page_index and that object's pool index 
	is returned.
Parameters:
	int page_index, BM_BufferPool *const bm
Return:
	current->pool_index
Author:
	Miao Song
HISTORY:
	Date		Name		Content
	2016-02-24	Miao Song	Written code
	2016-02-25	Jon Yang	Added function header comment
************************************************************************/


int pageindex_mapto_poolindex(int page_index, BM_BufferPool *const bm)

6)
/************************************************************************
Function Name: applyRSPolicy
Description:
	Applies the replacement policy designated in the function parameters.
Parameters:
	ReplacementStrategy policy, int pageNum, BM_BufferPool *const bm, SM_FileHandle fHandle
Return:
	Returns tail(bm)->pool_index
	Returns RC_ALL_PAGE_RESOURCE_OCCUPIED if candidate == null while iterating over list
Author:
	Miao Song
HISTORY:
	Date		Name		Content
	2016-02-24	Miao Song	Written code
	2016-02-25	Jon Yang	Added function header comment
	2016-03-01	Miao Song	Added support of Clock replacement policy
************************************************************************/
int applyRSPolicy(ReplacementStrategy policy, int pageNum, BM_BufferPool *const bm, SM_FileHandle fHandle){}

7)
/************************************************************************
Function Name: adjustOrderInCacheByLRU
Description:
	Adjust the cache order by LRU 
Parameters:
	int page_index, BM_BufferPool *const bm
Return:
	RC_OK
Author:
	Miao Song
HISTORY:
	Date		Name		Content
	2016-02-24	Miao Song	Written code
	2016-02-25	Jon Yang	Added function header comment
************************************************************************/
RC adjustOrderInCacheByLRU(int page_index, BM_BufferPool *const bm){}





==========================================================
# Additional error codes  #
==========================================================
#define RC_FILE_EXIST -1
#define RC_MEM_ALLOCATION_FAIL -2

#define RC_PAGE_NOT_FOUND_IN_CACHE 5
#define RC_UNPIN_FAIL 6
#define RC_FAIL_SHUTDOWN_POOL 7
#define RC_FAIL_FORCE_PAGE_DUETO_PIN_EXIT 8
#define RC_ALL_PAGE_RESOURCE_OCCUPIED -3

==========================================================
# Data structure #
==========================================================
1)We defined the data structure to instore all the tracking information needed per bufferpool.
This data structure (i.e., BP_mgmt) cooresponds to "void *mgmtData" element in the "BM_BufferPool" structure

----------------------------------------
typedef struct BP_mgmt {
	int * FrameContents; /* the pointer that points to the array of PageNumbers */
	bool * DirtyFlags; /* the pointer that points to the array of DirtyFlags */
	int * FixCounts;  /* the pointer that points to the array of FixCounts */
	int  NumReadIO;  /* the number of ReadIO */
	int  NumWriteIO; /* the number of WriteIO */
	char * PagePool;  /* pointer to point to the acutal page_frame in the memeory pool*/
	int *Flag4Clock;  /* pointer to point to flag array for clock memory swapping algorithm*/
	int AvailablePool; /* the number of empty/availabe page frames left in the memory pool */
	Bookkeeping4Swap *HEAD;
	Bookkeeping4Swap *TAIL;
	Bookkeeping4Swap *CURRENT_HANDLE;
}BP_mgmt;

--------------------------------------------

2)We defined a double linked list to organize the cached-pages in the buffer pool for different memory replacement policy, 
each element in the double linked list is defined in the datastruture as below:
------------------------------------------
typedef struct Bookkeeping4Swap {
	int pool_index; /* the position of the page frame in the pool */
	int page_index; /* the position of the page in the page file */
	struct Bookkeeping4Swap * next;
	struct Bookkeeping4Swap * prev;
}Bookkeeping4Swap;
---------------------------------------------
Different replacement policy can choose the candicate victim page to be swapped out by interation through the linked list according 
to their individual rules.

==========================================================
# Extra Credits, Additional Files, Test Cases #
==========================================================
Extra Credits: Implemented the Clock replacement policy
Additional Files: test_assign2_customize.c
Test Cases: specifically test the correctness of clock algorithm  by an example in test_assign2_customize.c 
