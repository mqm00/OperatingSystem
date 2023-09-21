#include "types.h"
#include "defs.h"

void test1(void){

	cprintf("test1");
	exit();
}

int sys_test1(void){
	test1();
	return 0;
}
