#include "kernel/types.h"
#include "kernel/uman.h"
#include "user/user.h"

int validate_username(char *username)
{
  if (strlen(username) > MAX_USERNAME)
  {
    printf("username too long\n");
    return -1;
  }

  for (int i = 0; i < strlen(username); i++)
  {
    // A-Za-z0-9 are allowed
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

int validate_password(char *password)
{
  if (strlen(password) > MAX_PASSWORD)
  {
    printf("password too long\n");
    return -1;
  }

  for (int i = 0; i < strlen(password); i++)
  {
    // A-Za-z0-9 are allowed
    if (!((password[i] >= 'A' && password[i] <= 'Z') ||
          (password[i] >= 'a' && password[i] <= 'z') ||
          (password[i] >= '0' && password[i] <= '9')))
    {
      printf("password contains invalid characters\n");
      return -1;
    }
  }

  return 0;
}

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

  if (validate_username(argv[1]) < 0)
  {
    exit(1);
  }

  if (validate_password(argv[2]) < 0)
  {
    exit(1);
  }

  if (addusr(argv[1], argv[2]) < 0)
  {
    fprintf(2, "adduser: failed to add user\n");
    exit(1);
  }

  exit(0);
}
