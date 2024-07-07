#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    fprintf(2, "usage: chmod <path> <mode>\n");
    exit(1);
  }

  if (argc > 4)
  {
    fprintf(2, "chmod: too many arguments\n");
    exit(1);
  }

  if (strlen(argv[2]) != 3)
  {
    fprintf(2, "chmod: mode must be 3 digits\n");
    exit(1);
  }

  ushort mode = 0;
  for (int i = 0; i < 3; i++)
  {
    if (argv[2][i] < '0' || argv[2][i] > '7')
    {
      fprintf(2, "chmod: mode must be in octal\n");
      exit(1);
    }
    int d = argv[2][i] - '0';

    // convert it to binary as each digit is an octat
    mode |= d << 3 * (2 - i);
  }

  if (chmod(argv[1], mode) < 0)
  {
    fprintf(2, "chmod: failed to change mode\n");
    exit(1);
  }

  exit(0);
}
