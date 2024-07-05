
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "stat.h"
#include "fs.h"
#include "file.h"
#include "uman.h"
#include "defs.h"

struct user users[MAX_USERS];

// just create a file name /users if it does not exist
// return 1 if it is newly created
// return 0 if it already exists
// return -1 on failure
int create_savefile()
{
  begin_op();
  struct dirent de;
  struct inode *ip, *root;
  root = namei("/");
  if (root == 0)
  {
    end_op();
    printf("failed to get root inode\n");
    return -1;
  }

  ilock(root);

  for (int off = 0; off < root->size; off += sizeof(de))
  {
    if (readi(root, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
    {
      iunlockput(root);
      end_op();
      printf("failed to read root inode\n");
      return -1;
    }

    if (de.inum == 0)
      continue;

    if (strncmp(de.name, "users", DIRSIZ) == 0)
    {
      iunlockput(root);
      end_op();
      return 0;
    }
  }

  printf("creating users savefile\n");
  ip = ialloc(root->dev, T_FILE);
  if (ip == 0)
  {
    iunlockput(root);
    end_op();
    printf("failed to ialloc\n");
    return -1;
  }

  ilock(ip);

  ip->major = 0;
  ip->minor = 0;
  ip->nlink = 1;
  ip->size = 0;
  ip->uid = 0;
  ip->type = T_FILE;

  iupdate(ip);

  if (dirlink(root, "users", ip->inum) < 0)
  {
    iput(ip);
    iunlockput(root);
    end_op();
    printf("failed to dirlink\n");
    return -1;
  }

  iunlockput(root);

  for (int i = 0; i < MAX_USERS; i++)
  {
    users[i].uid = 0;
    users[i].status = USER_INACTIVE;
    memset(users[i].username, 0, MAX_USERNAME);
    memset(users[i].password, 0, MAX_PASSWORD);
  }

  int n = writei(ip, 0, (uint64)users, 0, sizeof(struct user) * MAX_USERS);
  if (n != sizeof(struct user) * MAX_USERS)
  {
    iunlockput(ip);
    end_op();
    printf("failed to write initial users\n");
    return -1;
  }

  printf("initial users written\n");

  iunlock(ip);
  end_op();

  printf("users savefile created\n");

  return 1;
}

int saveusrs()
{
  int r = create_savefile();
  if (r < 0)
  {
    return -1;
  }
  struct inode *ip = namei("/users");
  if (ip == 0)
  {
    printf("failed to create or open savefile\n");
    return -1;
  }

  begin_op();
  ilock(ip);

  int n = writei(ip, 0, (uint64)users, 0, sizeof(struct user) * MAX_USERS);
  if (n != sizeof(struct user) * MAX_USERS)
  {
    iunlockput(ip);
    end_op();
    return -1;
  }

  iunlockput(ip);
  end_op();

  return 0;
}

// load users from the savefile
// return 0 on success
// return -1 on failure
// return 1 if it is newly initialized
int loadusrs()
{
  int r = create_savefile();
  if (r < 0)
  {
    return -1;
  }

  struct inode *ip = namei("/users");
  begin_op();

  ilock(ip);

  int n = readi(ip, 0, (uint64)users, 0, sizeof(struct user) * MAX_USERS);
  if (n != sizeof(struct user) * MAX_USERS)
  {
    iunlockput(ip);
    end_op();
    return -1;
  }

  iunlockput(ip);
  end_op();

  for (int i = 0; i < MAX_USERS; i++)
  {
    if (users[i].status == USER_ACTIVE)
    {
      printf("user: %s\n", users[i].username);
    }
  }

  return r;
}

int getusr(char username[MAX_USERNAME], struct user *user)
{
  if (loadusrs() < 0)
  {
    return -1;
  }

  for (int i = 0; i < MAX_USERS; i++)
  {
    if (users[i].status == USER_INACTIVE)
      continue;

    if (strncmp(users[i].username, username, MAX_USERNAME) == 0)
    {
      printf("user getusr %s found\n", username);
      *user = users[i];
      return 0;
    }
  }

  return -1;
}

int usrauthenticate(char username[MAX_USERNAME], char password[MAX_PASSWORD])
{
  printf("authenticating user %s\n", username);
  struct user *u = (struct user *)kalloc();
  if (getusr(username, u) < 0)
  {
    kfree((char *)u);
    return -1;
  }
  if (u == 0)
  {
    kfree((char *)u);
    return -1;
  }
  printf("user usrauthenticate %s found\n", u->username);

  printf("username: %s\n", u->username);
  printf("password: %s\n", u->password);

  printf("username: %s\n", username);
  printf("password: %s\n", password);

  if (strncmp(u->password, password, MAX_PASSWORD) == 0)
  {
    kfree((char *)u);
    return 0;
  }

  kfree((char *)u);
  return -1;
}

// add a new user to the user table
// return the uid of the new user
int addusr(char username[MAX_USERNAME], char password[MAX_PASSWORD])
{
  if (strlen(username) > MAX_USERNAME || strlen(password) > MAX_PASSWORD)
  {
    return -1;
  }

  if (loadusrs(users) < 0)
  {
    return -1;
  }

  for (int i = 0; i < MAX_USERS; i++)
  {
    if (users[i].uid == 0)
    {
      struct user *u = &users[i];
      u->uid = i;
      u->status = USER_ACTIVE;
      strncpy(u->username, username, MAX_USERNAME);
      strncpy(u->password, password, MAX_PASSWORD);

      if (saveusrs(users) < 0)
      {
        return -1;
      }
      printf("user %s added\n", username);
      return u->uid;
    }
  }

  return -1;
}

// delete a user from the user table
// return the uid of the deleted user
int delusr(char *username)
{
  struct user users[MAX_USERS];
  loadusrs(users);

  for (int i = 0; i < MAX_USERS; i++)
  {
    if (strncmp(users[i].username, username, MAX_USERNAME) == 0)
    {
      users[i].status = USER_INACTIVE;

      if (saveusrs(users) < 0)
      {
        return -1;
      }
      return users[i].uid;
    }
  }

  return -1;
}

int getusername(int uid, char *username)
{
  struct user users[MAX_USERS];
  loadusrs(users);

  for (int i = 0; i < MAX_USERS; i++)
  {
    if (users[i].uid == uid)
    {
      strncpy(username, users[i].username, MAX_USERNAME);
      return 0;
    }
  }

  return -1;
}
