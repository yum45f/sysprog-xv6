#include "kernel/types.h"
#include "kernel/uman.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  struct user u;
  if (getusr(&u) < 0)
  {
    fprintf(2, "whoami: failed to get user\n");
    exit(1);
  }

  fprintf(1, "uid: %d; username: %s\n", u.uid, u.username);
  exit(0);
}
