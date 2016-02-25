all : make_my_files

make_my_files : dberror storage_mgr test_assign test_customize 

remove:
	rm dberror.o storage_mgr.o test_assign test_customize

dberror : dberror.c
	gcc -c dberror.c -o dberror.o

storage_mgr : storage_mgr.c
	gcc -c storage_mgr.c -o storage_mgr.o

test_assign : test_assign1_1.c
	gcc test_assign1_1.c storage_mgr.o dberror.o -o test_assign

test_customize : test_customize.c
	gcc test_customize.c storage_mgr.o dberror.o -o test_customize
