#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

typedef char *dirname_t[DIRSIZ];

// return 0 if success, 1 if error
int concat(char *s1, char *s2, char *result)
{
  int i, j;
  char *buf = malloc(strlen(s1) + strlen(s2) + 1);

  for (i = 0; s1[i] != '\0'; i++)
  {
    buf[i] = s1[i];
  }

  for (j = 0; s2[j] != '\0'; j++)
  {
    buf[i + j] = s2[j];
  }

  buf[i + j] = '\0';

  strcpy(result, buf);
  free(buf);

  return 0;
}

// return 0 if suceed to get, 1 if error, 2 if root
int current_dirname(dirname_t *dirname)
{
  struct dirent de;

  int fd = open(".", 0);
  if (fd < 0)
  {
    fprintf(2, "pwd: cannot open .\n");
    return 1;
  }

  int n = read(fd, &de, sizeof(de));
  close(fd);
  if (n < sizeof(de))
  {
    fprintf(2, "pwd: read error\n");
    close(fd);
    return 1;
  }
  ushort inum = de.inum;

  chdir("..");
  fd = open(".", 0);
  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == inum)
    {
      if (de.name[0] == '.')
      {
        strcpy((char *)dirname, "/");
        return 2;
      }

      strcpy((char *)dirname, de.name);
      chdir(de.name);
      return 0;
    }
  }
  return 1;
}

int main(void)
{
  // assuming that the depth of the directory is less than 10
  dirname_t *parents = malloc(sizeof(dirname_t) * 10);
  int p_count = 0;
  int fd;
  struct dirent de;

  if ((fd = open("..", 0)) < 0)
  {
    fprintf(2, "pwd: cannot open ..\n");
    exit(1);
  }

  if (read(fd, &de, sizeof(de)) != sizeof(de))
  {
    fprintf(2, "pwd: cannot read ..\n");
    close(fd);
    exit(1);
  }

  dirname_t buf;

  while (1)
  {
    int n = current_dirname(&buf);
    if (n == 2)
    {
      break;
    }
    else if (n == 1)
    {
      exit(1);
    }

    printf("buf: %s\n", buf);
    memcpy(parents[p_count], buf, sizeof(buf));
    p_count++;
    chdir("..");
  }

  write(1, "/", 1);
  for (int i = 0; i < p_count; i++)
  {
    write(1, parents[p_count - i - 1], sizeof(parents[p_count - i - 1]));
    if (i != p_count - 1)
    {
      write(1, "/", 1);
    }
  }
  write(1, "\n", 1);
  free(parents);

  close(fd);
  exit(0);
}
