#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(void)
{
  char buf[512];

  while (1)
  {
    int n = read(0, buf, sizeof(buf));
    if (n == 0)
    {
      break;
    }
    write(1, buf, n);
  }

  exit(0);
}
