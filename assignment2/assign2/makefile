all : make_my_files

make_my_files : cleanup test_assign 

cleanup:
	rm -rf storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o test_assign *.bin

test_assign : test_assign2_1.c
	gcc -c buffer_mgr.c -o buffer_mgr.o
	gcc -c buffer_mgr_stat.c -o buffer_mgr_stat.o
	gcc -c dberror.c -o dberror.o
	gcc -c storage_mgr.c -o storage_mgr.o
	gcc test_assign2_1.c storage_mgr.o dberror.o buffer_mgr_stat.o buffer_mgr.o -o test_assign
