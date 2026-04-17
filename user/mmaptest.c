
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  uint64 shared_addr;
  int shmid = 0;

  printf("=== Test 1: Basic shared memory (like before) ===\n");

  // shmid=0 means create a NEW shared region; kernel writes back new id.
  shared_addr = mmap(&shmid);
  if(shared_addr == 0){
    printf("mmap failed\n");
    exit(1);
  }

  printf("Mapped shared memory at %p (id=%d)\n", (void*)shared_addr, shmid);

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
  int id1 = 0;
  int id2 = 0;
  uint64 addr1 = mmap(&id1);  // new region
  uint64 addr2 = mmap(&id2);  // another new region

  if(addr1 == 0 || addr2 == 0){
    printf("mmap failed for multiple regions\n");
    exit(1);
  }

  printf("Region 1 at %p (id=%d)\n", (void*)addr1, id1);
  printf("Region 2 at %p (id=%d)\n", (void*)addr2, id2);

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
    // Attach to parent's first region using id1 and verify it is the same page.
    int attach_id = id1;
    uint64 addr1_alias = mmap(&attach_id);
    if(addr1_alias == 0)
      printf("Child attach by id failed\n");
    else {
      int *alias_data = (int *)addr1_alias;
      printf("Child attach read Region 1 via alias: %d\n", *alias_data);
      if(addr1_alias != addr1)
        munmap(addr1_alias);
    }
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