#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(int fd, char *name, int printdefault, int printline, int printword, int printchar)
{
  int i, n;
  int l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  while((n = read(fd, buf, sizeof(buf))) > 0){
    for(i=0; i<n; i++){
      c++;
      if(buf[i] == '\n')
        l++;
      if(strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      else if(!inword){
        w++;
        inword = 1;
      }
    }
  }
  if(n < 0){
    printf("wc: read error\n");
    exit(1);
  }
  if (printdefault) {
    printf("%d %d %d %s\n", l, w, c, name);
    return;
  }
  if (printline) {
    printf("%d ", l);
  }
  if (printword) {
    printf("%d ", w);
  }  
  if (printchar) {
    printf("%d ", c);
  }
  printf("%s\n", name);
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    wc(0, "", 1, 0, 0, 0);
    exit(0);
  }
  int printdefault = 1;
  int printline = 0;
  int printword = 0;
  int printchar = 0;
  for(i = 1; i < argc; i++){
    if (argv[i][0] == '-') {
      printdefault = 0;
      for(int j = 1; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == 'l') {
          printline = 1;
        } else if (argv[i][j] == 'w') {
          printword = 1;
        } else if (argv[i][j] == 'c') {
          printchar = 1;
        }
      }
    }
  }
  for(i = 1; i < argc; i++){
    if (argv[i][0] == '-') {
      continue;
    }
    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i], printdefault, printline, printword, printchar);
    close(fd);
  }
  exit(0);
}
