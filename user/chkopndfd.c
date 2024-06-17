#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define P_READ (0)

void printb(uint16 v)
{
  int i;
  for (i = 15; i >= 0; i--)
  {
    if (v & (1 << i))
    {
      printf("1");
    }
    else
    {
      printf("0");
    }
  }
  printf("\n");
}

void main(void)
{
  uint16 opnd = opndfd();
  printb(opnd);

  int fd = open(".", P_READ);
  printf("opend fd: %d\n", fd);

  int fd2 = open("..", P_READ);
  printf("opend fd: %d\n", fd2);

  int fd3 = open("../..", P_READ);
  printf("opend fd: %d\n", fd3);

  close(fd);
  printf("closed fd: %d\n", fd);

  uint16 opnd2 = opndfd();
  printb(opnd2);

  exit(0);
}
