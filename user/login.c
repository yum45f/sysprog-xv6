#include "kernel/types.h"
#include "kernel/uman.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char *argv[] = {"sh", 0};

int cutnl(char *s)
{
  for (int i = 0; i < sizeof(s); i++)
  {
    if (s[i] == '\n')
    {
      s[i] = '\0';
      return 0;
    }
  }
  return -1;
}

int main(void)
{
  char username[MAX_USERNAME];
  char password[MAX_PASSWORD];

  while (1)
  {
    int r = reloadusrs();
    if (r < 0)
    {
      printf("failed to load users\n");
      exit(1);
    }

    if (r == 1)
    {
      printf("no users found, adding root user\n");

      printf("root username: ");
      gets(username, 32);

      printf("root password: ");
      gets(password, 64);

      cutnl(username);
      cutnl(password);

      if (addusr(username, password) < 0)
      {
        printf("failed to add user root\n");
        exit(1);
      }

      printf("root user added\n");

      if (reloadusrs() < 0)
      {
        printf("failed to reload users\n");
        exit(1);
      }

      printf("users reloaded\n");

      if (login(username, password) < 0)
      {
        printf("failed to login\n");
        continue;
      }

      break;
    }

    printf("login\n");
    printf("username: ");
    gets(username, 32);

    printf("password: ");
    gets(password, 64);

    // strip newline
    cutnl(username);
    cutnl(password);

    if (login(username, password) < 0)
    {
      printf("failed to login\n");
      continue;
    }
    printf("login success\n");
    break;
  }

  printf("success, welcome %s\n", username);
  exec("sh", argv);
  exit(0);
}
