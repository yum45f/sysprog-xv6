#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    fprintf(2, "usage: adduser <username> <password>\n");
    exit(1);
  }

  if (argc > 4)
  {
    fprintf(2, "adduser: too many arguments\n");
    exit(1);
  }

  if (addusr(argv[1], argv[2]) < 0)
  {
    fprintf(2, "adduser: failed to add user\n");
    exit(1);
  }

  exit(0);
}
