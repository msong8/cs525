all : make_my_files

make_my_files : cleanup test_assign test_customize_clock

cleanup:
	rm -rf storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o test_assign test_customize_clock *.bin

test_assign : test_assign2_1.c
	gcc -c buffer_mgr.c -o buffer_mgr.o
	gcc -c buffer_mgr_stat.c -o buffer_mgr_stat.o
	gcc -c dberror.c -o dberror.o
	gcc -c storage_mgr.c -o storage_mgr.o
	gcc test_assign2_1.c storage_mgr.o dberror.o buffer_mgr_stat.o buffer_mgr.o -o test_assign
	
test_customize_clock : test_assign2_customize.c
	gcc -c buffer_mgr.c -o buffer_mgr.o
	gcc -c buffer_mgr_stat.c -o buffer_mgr_stat.o
	gcc -c dberror.c -o dberror.o
	gcc -c storage_mgr.c -o storage_mgr.o
	gcc test_assign2_customize.c storage_mgr.o dberror.o buffer_mgr_stat.o buffer_mgr.o -o test_customize_clock
