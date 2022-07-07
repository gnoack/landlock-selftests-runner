#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

/*
 * The runner tool starts up in an environment where all the Landlock
 * tests are copied into the root directory, and establishes an
 * environment where they can actually run correctly.
 *
 * That means:
 * - Move all binaries into a subdirectory
 * - chdir into the subdirectory.
 */
int main(int argc, char *argv[]) {
  int wstatus = 0;
  int codes[100];  // XXX: Fixed size buffer

  if (mkdir("embed", 0700) < 0) {
    err(1, "mkdir(embed)");
  }
  for (int i=1; i<argc; i++) {
    char *src = argv[i];
    char *dst = malloc(100);  // XXX: Fixed size buffer, check return
    strcpy(dst, "embed/");
    strcat(dst, src);

    if (rename(src, dst) < 0) {
      err(1, "rename(%s, %s)", src, dst);
    }
    free(dst);
  }
  if (rename("true", "embed/true") < 0) {
    err(1, "rename(true, embed/true)");
  }
  if (chdir("embed") < 0) {
    err(1, "chdir(embed)");
  }

  for (int i=1; i<argc; i++) {
    char *binary = argv[i];
    char *cmd[] = {binary, NULL};
    int pid = fork();
    if (pid == -1) {
      err(1, "fork()");
    } else if (pid == 0) {
      if (execve(binary, cmd, NULL) < 0) {
        err(1, "execve(%s)", binary);
      }
      return 0;  // should be unreachable
    }
    wait(&wstatus);
    codes[i] = WEXITSTATUS(wstatus);
  }

  int code = 0;
  for (int i=1; i<argc; i++) {
    char *binary = argv[i];
    printf("# %s: exit %d\n", binary, codes[i]);
    if (codes[i] != 0) {
      code = codes[i];
    }
  }
  if (code == 0) {
    puts("#");
    puts("#  ____                              _ ");
    puts("# / ___| _   _  ___ ___ ___  ___ ___| |");
    puts("# \\___ \\| | | |/ __/ __/ _ \\/ __/ __| |");
    puts("#  ___) | |_| | (_| (_|  __/\\__ \\__ \\_|");
    puts("# |____/ \\__,_|\\___\\___\\___||___/___(_)");
  }
  return code;
}
