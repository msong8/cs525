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
dberror.c
dberror.h
storage_mgr.c
storage_mgr.h
test_assign1_1.c
test_helper.h
Makefile
test_customize.c


=========================
#   Mile Stone  #
=========================
1. Finishes implementing all the interface functions in storage_mgr.cases;
2. Passes the test cases for "create open and close methods", and "create, open, and close a page file";
3. Edits a customized test case for "create, open, and close a multi-page file ", and passes the test.


=========================
# Installation Instuctions  #
=========================
1. In the terminal, type in "make", the makefile script will generate two outputs "test_assign" and "test_customize";
2. Type in "./test_assign" to run the test_assign1_1.c test file for testing;
3. Type in "./test_customize" to run the test_customize.c test file for testing;
4. Type in "make remove" to remove all the generated files.

==========================================================
# Function descriptions: of all additional functions  #
==========================================================
N/A


==========================================================
# Additional error codes  #
==========================================================
#define RC_FILE_EXIST -1
#define RC_MEM_ALLOCATION_FAIL -2


==========================================================
# Data structure #
==========================================================
We use a linked list to maintain a list of currently opened file handles in the memory.
----------------------------
typedef struct SM_Handle {
	SM_FileHandle *fHandle;
	struct SM_Handle *next;
}SM_Handle;
SM_Handle *head = NULL;
SM_Handle *end = NULL;
---------------------------
1.Whenever a file is opened, we added the related SM_FileHandle pointer into the list;
2.Whenever a file is closed, the related SM_FileHandle pointer is removed from the list;
3.Whenever there is a block related operation (e.g. readBlock, writeBlock, appendEmptyBlock, getBlockPos), we check
if the passed-in SM_FileHandle pointer is in the list yet, if yes, then we proceed, otherwise, the function will return
the error "RC_FILE_HANDLE_NOT_INIT" .

==========================================================
# Extra Credit, Additional Files, Test cases #
==========================================================
The given test file verifies the correctenss of the single page file.
We added another file "test_customize.c" to verify the correctness of the multi-page file.
In particular, 
1. We create a empty page file first;
2. Ensure the capacity of the file with 4 pages using "TEST_CHECK(ensureCapacity(4,&fh))", i.e., appending another 3 empty pages behind;
3. Read the current page (PageNum = 0)into handle and verify if the current page is empty;
4. Read the next page (PageNum = 1) into handle and verify if that page is empty;
5. Write the strings into the PageNum 1 and then read back the page content into memory to compair/verify the strings operated correctly;
6. Read teh last page (PageNum = 3) into handle and verify if that page is empty;
7. Write the strings into the PageNum 1 and then read back the page content into memory to compair/verify the strings operated correctly.

