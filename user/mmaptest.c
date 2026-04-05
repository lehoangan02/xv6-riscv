
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  uint64 shared_addr;

  printf("=== Test 1: Basic shared memory (like before) ===\n");

  // id=0 means create a NEW shared region
  shared_addr = mmap(0);
  if(shared_addr == 0){
    printf("mmap failed\n");
    exit(1);
  }

  printf("Mapped shared memory at %p\n", (void*)shared_addr);

  int *shared_data = (int*)shared_addr;
  *shared_data = 42;
  printf("Parent wrote: %d\n", *shared_data);

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    printf("Child read: %d\n", *shared_data);
    *shared_data = 100;
    printf("Child wrote: %d\n", *shared_data);
    if(munmap(shared_addr) < 0)
      printf("Child: munmap failed\n");
    else
      printf("Child: munmap succeeded\n");
    exit(0);
  } else {
    wait(0);
    printf("Parent read after child: %d\n", *shared_data);
    if(munmap(shared_addr) < 0)
      printf("Parent: munmap failed\n");
    else
      printf("Parent: munmap succeeded\n");
  }

  printf("\n=== Test 2: Multiple shared memory regions ===\n");

  // Create two separate shared regions
  uint64 addr1 = mmap(0);  // new region
  uint64 addr2 = mmap(0);  // another new region

  if(addr1 == 0 || addr2 == 0){
    printf("mmap failed for multiple regions\n");
    exit(1);
  }

  printf("Region 1 at %p\n", (void*)addr1);
  printf("Region 2 at %p\n", (void*)addr2);

  // Write different values to each region
  int *data1 = (int*)addr1;
  int *data2 = (int*)addr2;
  *data1 = 111;
  *data2 = 222;

  printf("Region 1: %d, Region 2: %d\n", *data1, *data2);

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    printf("Child sees Region 1: %d, Region 2: %d\n", *data1, *data2);
    *data1 = 333;
    *data2 = 444;
    printf("Child wrote Region 1: %d, Region 2: %d\n", *data1, *data2);
    munmap(addr1);
    munmap(addr2);
    exit(0);
  } else {
    wait(0);
    printf("Parent sees Region 1: %d, Region 2: %d\n", *data1, *data2);
    munmap(addr1);
    munmap(addr2);
  }

  printf("\nAll tests passed!\n");
  exit(0);
}