#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/fcntl.h>
#include <syscall.h>


void die (const char *msg)
{
  perror(msg);
  exit(errno);
}

void attack()
{
  int rc;
  
  // mkdir("dir", 0777);
  syscall(SYS_getpid, SYS_mkdir, "dir", 0777); // 引数部分に SYS_mkdir とその引数を与えておく
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

    // レジスタの内容を変更してシステムコールを変更する
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
