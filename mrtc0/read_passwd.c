#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>


void die(const char *msg)
{
  perror(msg);
  exit(errno);
}

struct my_file_handle {
  unsigned int handle_bytes;
  int handle_type;
  unsigned char f_handle[8];
};

int main()
{
  int fd1, fd2;
  char buf[0x1000];

  struct my_file_handle h = {
    .handle_bytes = 8,
    .handle_type = 1,
    // 57690 = E1 5A
    .f_handle = {0x5a, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
  };

  // $ mount
  // /dev/sda1 on /etc/hosts type ext4 (rw,relatime,data=ordered)
  if ((fd1 = open("/etc/hosts", O_RDONLY)) < 0)
    die("failed to open");

  if ((fd2 = open_by_handle_at(fd1, (struct file_handle *)&h, O_RDONLY)) < 0)
    die("failed to open_by_handle_at");

  memset(buf, 0, sizeof(buf));
  if (read(fd2, buf, sizeof(buf) - 1) < 0)
    die("failed to read");

  fprintf(stderr, "%s", buf);
  close(fd2);
  close(fd1);
  return 0;
}
