
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

  if (argstr(0, username, MAX_USERNAME) < 0 || argstr(1, password, MAX_PASSWORD) < 0)
    return -1;

  int result = usrauthenticate(username, password);
  printf("login: result: %d\n", result);
  if (result < 0)
  {
    printf("login: authentication failed\n");
    return -1;
  }

  struct proc *p = myproc();
  p->uid = result;
  return 0;
}

int sys_addusr(void)
{
  struct proc *p = myproc();
  if (p->uid != 0)
  {
    printf("addusr: only root can add user\n");
    return -1;
  }

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

int sys_getuid(void)
{
  struct proc *p = myproc();
  return p->uid;
}
