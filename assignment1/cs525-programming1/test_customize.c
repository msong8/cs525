#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

/* test output files */
#define TESTPF "test_pagefile.bin"

/* prototypes for test functions */

static void testMultiPageContent(void);

/* main function running all tests */
int
main (void)
{
  testName = "";
  
  initStorageManager();

  testMultiPageContent();

  return 0;
}




/* Try to create, open, and close a multi-page file */
void
testMultiPageContent(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "test multi-page content";

  ph = (SM_PageHandle) malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile (TESTPF));
  TEST_CHECK(openPageFile (TESTPF, &fh));
  printf("created and opened file\n");
  
  //ensure the capacity of the file with 4 pages
  TEST_CHECK(ensureCapacity(4,&fh));
  // read the current page into handle
  TEST_CHECK(readCurrentBlock(&fh, ph));
  // the page should be empty (zero bytes)
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in current page of freshly initialized page");
  printf("current block was empty with the page number =%d\n", getBlockPos(&fh));
    
  // read the next page into handle
  TEST_CHECK(readNextBlock(&fh, ph));
  // the page should be empty (zero bytes)
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in next page of freshly initialized page");
  printf("the next block was empty\n");
  printf("the updated page number is =%d\n",getBlockPos(&fh));
	
  // change ph to be a string and write that one to disk
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = (i % 10) + '0';
  TEST_CHECK(writeCurrentBlock(&fh, ph));
  printf("writing current block with the page number =%d\n", getBlockPos(&fh));

  // read back the page containing the string and check that it is correct
  TEST_CHECK(readCurrentBlock (&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  printf("reading back current block into memory with page number =%d\n", getBlockPos(&fh));
 
  // read the last page into handle
  TEST_CHECK(readLastBlock(&fh, ph));
  // the page should be empty (zero bytes)
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in last page of freshly initialized page");
  printf("last block was empty with the page number =%d\n", getBlockPos(&fh));


  // change ph to be a string and write that one to disk
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = (i % 10) + '0';
  TEST_CHECK(writeCurrentBlock(&fh, ph));
  printf("writing current block with the page number =%d\n", getBlockPos(&fh));

  // read back the page containing the string and check that it is correct
  TEST_CHECK(readCurrentBlock (&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  printf("reading back current block into memory with page number =%d\n", getBlockPos(&fh));



  // destroy new page file
  TEST_CHECK(destroyPageFile (TESTPF));  
  
  TEST_DONE();
}
