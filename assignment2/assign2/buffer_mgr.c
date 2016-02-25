#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>



typedef struct BM_BufferPool {
	char *pageFile;
	int numPages;
	ReplacementStrategy strategy;
	void *mgmtData;
} BM_BufferPool;



typedef struct BM_PageHandle {
	PageNumber pageNum;
	char *data;
} BM_PageHandle;


typedef struct BP_mgmt {
	int * FrameContents; // the pointer that points to the array of PageNumbers
	bool * DirtyFlags;
	int * FixCounts;
	int  NumReadIO;
	int  NumWriteIO;
	char * PagePool;  //pointer to point to the acutal page_frame in the memeory pool
	int AvailablePool;
	Bookkeeping4Swap *HEAD;
	Bookkeeping4Swap *TAIL;
}BP_mgmt;



typedef struct Bookkeeping4Swap {
	int pool_index; /* the position of the page frame in the pool */
	int page_index; /* the position of the page in the page file */
	struct Bookkeeping4Swap * next;
	struct Bookkeeping4Swap * prev;
}Bookkeeping4Swap;






#define MAKE_BP_mgmt() \
((BP_mgmt *)malloc (sizeof(BP_mgmt)))
#define MAKE_Bookkeeping4swap() \
((Bookkeeping4Swap *)malloc (sizeof(Bookkeeping4Swap)))
#define head(bm) \
(bm->mgmtData->HEAD)
#define tail(bm) \
(bm->mgmtData->TAIL)



void init_BP_mgmt (BP_mgmt * mgmtData, int pool_size){
	mgmtData->FrameContents = (int *)malloc(pool_size*sizeof(int));
	mgmtData->DirtyFlags = (bool *)malloc (pool_size*sizeof(bool));
	mgmtData->FixCounts = (int *)malloc(pool_size*sizeof(int));
	mgmtData->NumReadIO = 0;
	mgmtData->NumWriteIO = 0;
	mgmtData->PagePool = (char *)malloc(pool_size*PAGE_SIZE*sizeof(char));
	mgmtData->AvailablePool = pool_size;
	mgmtData->HEAD = NULL;
	mgmtData->TAIL = NULL;
	for (int i=0; i< pool_size; i++){
		*(mgmtData->FrameContents + i) = NO_PAGE;
		*(mgmtData->DirtyFlags + i) = false;
		*(mgmtData->FixCounts) = 0;
	}
}

void destroy_BP_mgmt(BP_mgmt * mgmtData){
	free(mgmtData->FrameContents);
	free(mgmtData->DirtyFlags);
	free(mgmtData->FixCounts);
	fee(msmgtData->PagePool);
	mgmtData->FrameContents = NULL;
	mgmtData->DirtyFlags = NULL;
	mgmtData->FixCounts = NULL;
	mgmtData->NumReadIO = NULL;
	mgmtData->NumWriteIO = NULL;
	mgmtData->PagePool = NULL;
	mgmtData->HEAD = NULL;
	mgmtData->TAIL = NULL;

}

void insert_into_bookkeepinglist(int page_index, int pool_index, BM_BufferPool *const bm){
	Bookkeeping4Swap *handle = MAKE_Bookkeeping4swap();
	handle->page_index = page_index;
	handle->pool_index = pool_index;
	handle->next = NULL;
	if(head(bm) == NULL){
		head(bm) = handle;
		tail(bm) = handle;
		handle->prev = NULL;
	} else {
		tail(bm)->next = handle;
		handle->prev = tail;
		tail(bm) = handle;
	}
	*(bm->mgmtData->FrameContents+pool_index) = page_index;
}

RC check_in_cache(int page_index, BM_BufferPool *const bm){
	Bookkeeping4Swap *current = NULL;
	current = head(bm);
	while (current->next!=NULL){
		if (current->page_index == page_index){
			return RC_OK;
		}
	}
	return RC_PAGE_NOT_FOUND_IN_CACHE;
}




int pageindex_mapto_poolindex(int page_index, BM_BufferPool *const bm){
	Bookkeeping4Swap *current = NULL;
	current = head(bm);
	while (current->next!=NULL){
		if (current->page_index == page_index){
			return current->pool_index;
		}
	}
}

int applyRSPolicy(ReplacementStrategy policy, int pageNum, BM_BufferPool *const bm, SM_FileHandle fHandle){

	if (policy == RS_FIFO){
		/* Check to see if the candidate page_frame has the fix_count =0 */
		Bookkeeping4Swap * candidate = head(bm);
		while (*(bm->mgmtData->FixCounts+candidate->pool_index)>0 && candicate != NULL){
			candidate = candidate->next;
		}
		if (candicate == NULL) {
			return RC_ALL_PAGE_RESOURCE_OCCUPIED;
		} else {
			if(candicate != tail(bm)){
				tail(bm)->next = candidate;
				
				candidate->next->prev = candicate->prev;
				if(candidate == head(bm)){
					head(bm) = head(bm)->next;
				} else {
					candidate->prev->next = candidate->next;
				}
				candidate->prev = tail(bm);				
				tail(bm) = tail(bm)->next;
				tail(bm)->next = NULL;
			}
			if(*(bm->mgmtData->DirtyFlags+tail(bm)->pool_index)== true){
				char * memory = bm->mgmtData->PagePool + tail(bm)->pool_index*PAGE_SIZE*sizeof(char);
				int old_pageNum = tail(bm)->page_index;
				writeBlock(old_pageNum, &fh, memory);
				*(bm->mgmtData->DirtyFlags+tail(bm)->pool_index)= false; /* set dirty flag to false after flashing into disk */
				bm->mgmtData->NumWriteIO++;
			}
			tail(bm)->page_index = pageNum;
			*(bm->mgmtData->FrameContents+tail(bm)->pool_index) = pageNum;
			return tail(bm)->pool_index;
		}
	}

}

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		const int numPages, ReplacementStrategy strategy, 
		void *stratData)
{
/*assign values to BM_BufferPool structure.*/
	bm->pageFile = (char *)pageFileName;
	bm->numPages = numPages;
	bm->strategy = strategy;
	bm->mgmtData = stratData;
	(BP_mgmt)bm->mgmtData = MAKE_BP_mgmt();
	init_BP_mgmt(bm->mgmtData,bm->numPages);
	return RC_OK;
}

RC shutdownBufferPool(BM_BufferPool *const bm){
	for (int i=0; i< bm->numPages; i++){
		if (*(bm->mgmtData->FixCounts+i)> 0) {
			
			return RC_FAIL_SHUTDOWN_POOL;
				
		}
    }
	forceFlushPool(bm);
	destroy(bm->mgmtData);
	free(bm->mgmtData);
	bm->mgmtData = NULL;
	return RC_OK;
}


RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page){
	int pool_index = pageindex_mapto_poolindex(page->pageNum, bm);
	*(bm->mgmtData->DirtyFlags+pool_index) = true;
	return RC_OK;
}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){
	int pool_index = pageindex_mapto_poolindex(page->pageNum, bm);
	*(bm->mgmtData->FixCounts+pool_index)--;
	if (*(bm->mgmtData->FixCounts+pool_index)<0){
		return RC_UNPIN_FAIL;
	}
	return RC_OK;

}
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page){
	/* open the page file in the disk first */
	
	int pageNum = page->pageNum;
	int pool_index = pageindex_mapto_poolindex(pageNum,bm);
	if (*(bm->mgmtData->FixCounts+pool_index)> 0){
		return RC_FAIL_FORCE_PAGE_DUETO_PIN_EXIT;
	} else {
		SM_FileHandle fh;
		openPageFile (bm->pageFile, &fh);
		char * memory = page->data;
		writeBlock(pageNum, &fh, memory);
		*(bm->mgmtData->DirtyFlags+pool_index)= false; /* set dirty flag to false after flashing into disk */
		bm->mgmtData->NumWriteIO++;
		closePageFile(&fh);	
		return RC_OK;
	}
}

RC forceFlushPool(BM_BufferPool *const bm){
	/* open the page file in the disk first */
	SM_FileHandle fh;
	char * memory;
	int page_index;
	openPageFile (bm->pageFile, &fh);
	for (int i=0; i< bm->numPages; i++){
		if (*(bm->mgmtData->DirtyFlags+i)== true) {
			page_index = *(bm->mgmtData->FrameContents+i);
			memory = bm->mgmtData->PagePool + i*PAGE_SIZE*sizeof(char);
			writeBlock(page_index, &fh, memory);
			*(bm->mgmtData->DirtyFlags+page_index)= false; /* set dirty flag to false after flashing into disk */
			bm->mgmtData->NumWriteIO++;			
		}		
	}
	closePageFile(&fh);	
	return RC_OK;		
}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
	    const PageNumber pageNum){
		
	/* open the page file in the disk first */
	SM_FileHandle fh;
	openPageFile (bm->pageFile, &fh);
	/* The request page_index is larger than the actual size of page file, this could cause segment false */
	if (fh->totalNumPages < (pageNum+1)){
		ensureCapacity(pageNum+1,&fh);
	}
	
	page->pageNum = pageNum;
	if (check_in_cache(pageNum,bm)==0){	
		int pool_index = pageindex_mapto_poolindex(pageNum,bm);
		*(bm->mgmtData->FixCounts+pool_index)++;
		page->data = (bm->mgmtData->PagePool)+pool_index*PAGE_SIZE*sizeof(char);
		return RC_OK;
	} else {
		
		if (bm->mgmtData->AvailablePool>0){
			int pool_index = bm->numPages - bm->mgmtData->AvailablePool;
			insert_into_bookkeepinglist(pageNum, pool_index ,bm);
			bm->mgmtData->AvailablePool--;
			*(bm->mgmtData->FixCounts+pool_index)++;
			page->data = (bm->mgmtData->PagePool)+pool_index*PAGE_SIZE*sizeof(char);
			
		} else {
			/* It is time to swap out the old page from cache */
			int pool_index = applyRSPolicy(bm->ReplacementStrategy,pageNum, bm, fh);
			*(bm->mgmtData->FixCounts+pool_index)++;	
			page->data = (bm->mgmtData->PagePool)+pool_index*PAGE_SIZE*sizeof(char);
		}
		writeBlock(page->pageNum, &fh, page->data);
		bm->mgmtData->NumReadIO++;
	}
	closePageFile(&fh);		
		
}

// Statistics Interface
PageNumber *getFrameContents (BM_BufferPool *const bm){
	return bm->mgmtData->FrameContents;
}
bool *getDirtyFlags (BM_BufferPool *const bm){
	return bm->mgmtData->DirtyFlags;
}
int *getFixCounts (BM_BufferPool *const bm){
	return bm->mgmtData->FixCounts;
}
int getNumReadIO (BM_BufferPool *const bm){
	return bm->mgmtData->NumReadIO;
}
int getNumWriteIO (BM_BufferPool *const bm){
	return bm->mgmtData->NumWriteIO;
}