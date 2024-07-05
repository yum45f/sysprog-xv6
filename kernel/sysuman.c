
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "defs.h"
#include "proc.h"
#include "uman.h"

int sys_login(void)
{
  char username[MAX_USERNAME];
  char password[MAX_PASSWORD];

  struct user u;
  if (argstr(0, username, MAX_USERNAME) < 0 || argstr(1, password, MAX_PASSWORD) < 0)
    return -1;

  if (usrauthenticate(username, password) == 0)
  {
    getusr(username, &u);
    myproc()->uid = u.uid;
    return 0;
  }

  return -1;
}

int sys_addusr(void)
{
  char username[MAX_USERNAME];
  char password[MAX_PASSWORD];

  if (argstr(0, username, MAX_USERNAME) < 0 || argstr(1, password, MAX_PASSWORD) < 0)
    return -1;

  return addusr(username, password);
}

// return 0 on success
// return -1 on failure
// rerturn 1 if it is newly initialized
int sys_reloadusrs(void)
{
  return loadusrs();
}
