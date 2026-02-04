#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

int
readline(int fd, char *buf, int maxlen)
{
  int n;
  char c;
  int i = 0;

  /* Read one character at a time from fd */
  while((n = read(fd, &c, 1)) > 0){
    buf[i] = c;
    /* Look for the newline character */
    if (c == '\n'){
      /* We are at the end of the line, so stop reading */
      break;
    }
    i += 1;
    /* We don't want to read more characters than we have room */
    if(i >= (maxlen - 1)){
      /* We can't recover, so just print a message and exit */
      fprintf(2, "readline() - line too long\n");
      exit(-1);
    }
  }
  /* This is a little tricky. If read() returns 0 AND we didn't
     read previous characters for this line, then we want to return 0.
     Also, if read returns a value less than 0, we want to return this
     error condition. */
  if(((n == 0) && (i == 0)) || (n < 0))
    return n;

  /* Add the null terminator to the end for the string buffer */
  i += 1;
  buf[i] = '\0';
  return i;
}

void
print_width_int(int x, int width)
{
  int tmp = x;
  int digits = 0;

  if (tmp == 0)
    digits = 1;
  else {
    while (tmp > 0) {
      digits++;
      tmp /= 10;
    }
  }

  for (int i = 0; i < width - digits; i++)
    printf(" ");

  printf("%d", x);
}

void
cat(int fd, int lineNumber)
{
  if (lineNumber) {
    int lineCount = 1;
    int n;
    while((n = readline(fd, buf, sizeof(buf))) > 0) {

      print_width_int(lineCount++, 6);
      printf("  %s", buf);
    }
    if(n < 0){
      fprintf(2, "cat: read error\n");
      exit(1);
    }
    return;
  }
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  if(n < 0){
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0, 0);
    exit(0);
  }
  int lineNumber = 0;
  // check for -n option
  for(i = 1; i < argc; i++){
    if (argv[i][0] == '-' && argv[i][1] == 'n') {
      lineNumber = 1;
      break;
    }
  }
  for(i = 1; i < argc; i++){
    if (argv[i][0] == '-' && argv[i][1] == 'n') {
      continue;
    }
    if((fd = open(argv[i], O_RDONLY)) < 0){
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd, lineNumber);
    close(fd);
  }
  exit(0);
}

