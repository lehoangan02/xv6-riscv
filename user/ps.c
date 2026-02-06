#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/getproc.h"

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

int
main(int argc, char *argv[])
{
  struct procinfo ptable[64];
  struct procinfo *p;
  int err;
  
  static char *states[] = {
  [UNUSED]    "unused",
  [USED]      "used",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };

  err = getprocs(ptable);
  
  if(err < 0){
    printf("Error getting process info\n");
    exit(1);
  }

  printf("PID\tSTATE\tSIZE\tNAME\n");

  for(int i = 0; i < 64; i++){
    p = &ptable[i];
    
    if(p->state != UNUSED){
      printf("%d\t%s\t%d\t%s\n", p->pid, states[p->state], (int)p->sz, p->name);
    }
  }

  exit(0);
}