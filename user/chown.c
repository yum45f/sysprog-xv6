#include "kernel/types.h"
#include "kernel/uman.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

int val_uname(char *username)
{
  if (strlen(username) > MAX_USERNAME)
  {
    printf("username too long\n");
    return -1;
  }

  for (int i = 0; i < strlen(username); i++)
  {
    if (!((username[i] >= 'A' && username[i] <= 'Z') ||
          (username[i] >= 'a' && username[i] <= 'z') ||
          (username[i] >= '0' && username[i] <= '9')))
    {
      printf("username contains invalid characters\n");
      return -1;
    }
  }

  return 0;
}

int val_path(char *path)
{
  if (strlen(path) > DIRSIZ)
  {
    printf("path too long\n");
    return -1;
  }

  // check existence of path
  struct stat st;

  if (stat(path, &st) < 0)
  {
    printf("path does not exist\n");
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    fprintf(2, "usage: chown <path> <user>\n");
    exit(1);
  }

  if (argc > 4)
  {
    fprintf(2, "chown: too many arguments\n");
    exit(1);
  }

  if (val_uname(argv[2]) < 0)
  {
    exit(1);
  }

  if (val_path(argv[1]) < 0)
  {
    exit(1);
  }

  if (chown(argv[1], argv[2]) < 0)
  {
    fprintf(2, "chown: failed to change owner\n");
    exit(1);
  }

  exit(0);
}
