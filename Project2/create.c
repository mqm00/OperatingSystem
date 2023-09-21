/* clone and verify that address space is shared */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int global = 1;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   exit(); \
}

void* worker();

int
main(int argc, char *argv[])
{
   thread_t tid = 0;
   int result = thread_create(&tid, worker, NULL);
   if(result == -1){
    printf(2, "thread Error\n");
   }
   printf(1, "tid = %d\n", tid);
   assert(tid > 0);
   while(global != 5);
   printf(1, "TEST PASSED\n");
   exit();
}

void*
worker() {
   assert(global == 1);
   global = 5;
   return 0;
}
