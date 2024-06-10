#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define P_READ (0)
#define P_WRITE (1)

typedef unsigned long uint64;

void worker(int p2parent[2], int part)
{
  printf("prime number found: %d\n", part);
  int p2child[2];
  int child_initiated = 0;
  int i;

  while (read(p2parent[P_READ], &i, sizeof(i)) != 0)
  {
    if (i == -1)
    {
      if (child_initiated)
      {
        write(p2child[P_WRITE], &i, sizeof(i));
      }
      break;
    }

    if (i % part == 0)
    {
      continue;
    }

    if (!child_initiated)
    {
      pipe(p2child);
      int pid = fork();

      if (pid == 0)
      {
        worker(p2child, i);
        break;
      }
      else
      {
        write(p2parent[P_WRITE], &i, sizeof(i));
        child_initiated = 1;
      }
    }

    write(p2child[P_WRITE], &i, sizeof(i));
  }

  if (child_initiated)
  {
    close(p2child[P_READ]);
    close(p2child[P_WRITE]);
  }
}

int main(int argc, char *argv[])
{
  uint64 max = atoi(argv[1]);

  if (argc != 2)
  {
    exit(1);
  }

  int p[2];
  pipe(p);
  int pid = fork();

  if (pid == 0)
  {
    worker(p, 2);
  }
  else
  {
    for (int i = 2; i <= max; i++)
    {
      write(p[P_WRITE], &i, sizeof(i));
    }
    int i = -1;
    write(p[P_WRITE], &i, sizeof(-1));
  }

  close(p[P_READ]);
  close(p[P_WRITE]);

  wait(0);
  exit(0);
}
