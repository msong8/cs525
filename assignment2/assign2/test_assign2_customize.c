#include "storage_mgr.h"
#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"
#include "dberror.h"
#include "test_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// var to store the current test's name
char *testName;

// check whether two the content of a buffer pool is the same as an expected content 
// (given in the format produced by sprintPoolContent)
#define ASSERT_EQUALS_POOL(expected,bm,message)			        \
  do {									\
    char *real;								\
    char *_exp = (char *) (expected);                                   \
    real = sprintPoolContent(bm);					\
    if (strcmp((_exp),real) != 0)					\
      {									\
	printf("[%s-%s-L%i-%s] FAILED: expected <%s> but was <%s>: %s\n",TEST_INFO, _exp, real, message); \
	free(real);							\
	exit(1);							\
      }									\
    printf("[%s-%s-L%i-%s] OK: expected <%s> and was <%s>: %s\n",TEST_INFO, _exp, real, message); \
    free(real);								\
  } while(0)

// test and helper methods
static void testCreatingAndReadingDummyPages (void);
static void createDummyPages(BM_BufferPool *bm, int num);
static void checkDummyPages(BM_BufferPool *bm, int num);

static void testReadPage (void);


static void testClock (void);

// main method
int 
main (void) 
{
  initStorageManager();
  testName = "";
  testCreatingAndReadingDummyPages();
  testReadPage();
  testClock();
}

// create n pages with content "Page X" and read them back to check whether the content is right
void
testCreatingAndReadingDummyPages (void)
{
  BM_BufferPool *bm = MAKE_POOL();
  testName = "Creating and Reading Back Dummy Pages";

  CHECK(createPageFile("testbuffer.bin"));

  createDummyPages(bm, 22);
  checkDummyPages(bm, 20);

  createDummyPages(bm, 100);

  CHECK(destroyPageFile("testbuffer.bin"));

  free(bm);
  TEST_DONE();
}


void 
createDummyPages(BM_BufferPool *bm, int num)
{
  int i;
  BM_PageHandle *h = MAKE_PAGE_HANDLE();

  CHECK(initBufferPool(bm, "testbuffer.bin", 3, RS_FIFO, NULL));
  
  for (i = 0; i < num; i++)
    {
      CHECK(pinPage(bm, h, i));
      sprintf(h->data, "%s-%i", "Page", h->pageNum);
      CHECK(markDirty(bm, h));
      CHECK(unpinPage(bm,h));
    }

  CHECK(shutdownBufferPool(bm));

  free(h);
}

void 
checkDummyPages(BM_BufferPool *bm, int num)
{
  int i;
  BM_PageHandle *h = MAKE_PAGE_HANDLE();
  char *expected = malloc(sizeof(char) * 512);

  CHECK(initBufferPool(bm, "testbuffer.bin", 3, RS_FIFO, NULL));

  for (i = 0; i < num; i++)
    {
      CHECK(pinPage(bm, h, i));

      sprintf(expected, "%s-%i", "Page", h->pageNum);
      ASSERT_EQUALS_STRING(expected, h->data, "reading back dummy page content");

      CHECK(unpinPage(bm,h));
    }

  CHECK(shutdownBufferPool(bm));

  free(expected);
  free(h);
}

void
testReadPage ()
{
  BM_BufferPool *bm = MAKE_POOL();
  BM_PageHandle *h = MAKE_PAGE_HANDLE();
  testName = "Reading a page";

  CHECK(createPageFile("testbuffer.bin"));
  CHECK(initBufferPool(bm, "testbuffer.bin", 3, RS_FIFO, NULL));
  
  CHECK(pinPage(bm, h, 0));
  CHECK(pinPage(bm, h, 0));

  CHECK(markDirty(bm, h));

  CHECK(unpinPage(bm,h));
  CHECK(unpinPage(bm,h));

  CHECK(forcePage(bm, h));

  CHECK(shutdownBufferPool(bm));
  CHECK(destroyPageFile("testbuffer.bin"));

  free(bm);
  free(h);

  TEST_DONE();
}

//Test clock replacment policy for extra credits
void
testClock ()
{
  // expected results
  const char *poolContents[] = { 
    "[0 0],[-1 0],[-1 0],[-1 0]" , 
    "[0 0],[1 0],[-1 0],[-1 0]", 
    "[0 0],[1 0],[2 0],[-1 0]",
	"[0 0],[1 0],[2 0],[3 0]",
	"[0 0],[1 0],[2 0],[3 0]",
	"[0 0],[1 0],[2 0],[3 0]",
	"[0 0],[1 0],[2 0],[3 0]",
	"[0 0],[1 0],[2 0],[3 0]",
	"[4 0],[1 0],[2 0],[3 0]",
	"[4 0],[1 0],[2 0],[3 0]",
	"[4 0],[1 0],[0 0],[3 0]",
	"[4 0],[1 0],[0 0],[3 0]",
	"[4 0],[1 0],[0 0],[2 0]",
	"[3 0],[1 0],[0 0],[2 0]",
	"[3 0],[1 1],[0 0],[2 0]",
	"[3 0],[1x1],[0 0],[2 0]",
	"[3 0],[1x1],[7x0],[2 0]",
	"[3x0],[1x1],[7x0],[2 0]",
	"[3x0],[1x1],[7x0],[9x0]",
	"[3x0],[1x0],[7x0],[9x0]",
	"[3 0],[1 0],[7 0],[9 0]"
  };
  const int requests[] = {0,1,2,3,2,0,3,1,4,1,0,1,2,3,1,1,7,3,9};
  const int numLinRequests = 14;
  const int numChangeRequests = 4;

  int i,j;
  BM_BufferPool *bm = MAKE_POOL();
  BM_PageHandle *h = MAKE_PAGE_HANDLE();
  testName = "Testing CLOCK page replacement";

  CHECK(createPageFile("testbuffer.bin"));

  createDummyPages(bm, 200);

  CHECK(initBufferPool(bm, "testbuffer.bin", 4, RS_CLOCK, NULL));

  // reading some pages linearly with direct unpin and no modifications
  
  for(i = 0; i < numLinRequests; i++)
    {
      pinPage(bm, h, requests[i]);
      unpinPage(bm, h);
      ASSERT_EQUALS_POOL(poolContents[i], bm, "check pool content");
    }

  // pin one page and test remainder
  i = numLinRequests;
  pinPage(bm, h, requests[i]);
  ASSERT_EQUALS_POOL(poolContents[i],bm,"pool content after pin page");

  // read pages and mark them as dirty
  for(i = numLinRequests + 1; i < numLinRequests + numChangeRequests + 1; i++)
    {
      pinPage(bm, h, requests[i]);
      markDirty(bm, h);
      unpinPage(bm, h);
      ASSERT_EQUALS_POOL(poolContents[i], bm, "check pool content");
    }

  // flush buffer pool to disk
  i = numLinRequests + numChangeRequests + 1;
  h->pageNum = 1; // "1" is the index of second page
  unpinPage(bm, h); // unpin the page 
  ASSERT_EQUALS_POOL(poolContents[i],bm,"unpin the second page");
  
  i++;
  forceFlushPool(bm);
  ASSERT_EQUALS_POOL(poolContents[i],bm,"pool content after flush");


  // check number of write IOs
  ASSERT_EQUALS_INT(4, getNumWriteIO(bm), "check number of write I/Os");
  ASSERT_EQUALS_INT(10, getNumReadIO(bm), "check number of read I/Os");
  
  
  CHECK(shutdownBufferPool(bm));
  CHECK(destroyPageFile("testbuffer.bin"));

  free(bm);
  free(h);
  TEST_DONE();
}





