#define _GNU_SOURCE
#include "./command.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int exec_subprocess(struct Command c, int read_fd, int write_fd) {
  if (read_fd != -1) {
    if (dup2(read_fd, STDIN_FILENO) == -1) {
      perror("dup2");
      return -1;
    };
    close(read_fd); // Close the read_fd after duplicating it
  }

  if (write_fd != -1) {
    if (dup2(write_fd, STDOUT_FILENO) == -1) {
      perror("dup2");
      return -1;
    };
    close(write_fd); // Close the read_fd after duplicating it
  }

  for (int i = 0; i < c.amount_of_env_vars; i++) {
    setenv(c.env_vars[i].key, c.env_vars[i].value, 1);
  }

  int err = execvp(c.command[0], c.command);

  free_command(c);
  return err;
}

int fork_subprocess(struct Command c, int read_fd, int write_fd) {
  int err_pipe[2];

  // Create a pipe
  if (pipe(err_pipe) == -1) {
    perror("pipe");
    return -1;
  }

  pid_t pid = fork();
  if (pid != -1) {
    if (pid == 0) {
      int err = exec_subprocess(c, read_fd, write_fd);
      close(err_pipe[0]);

      write(err_pipe[1], &err, sizeof(int));
      return -1;
    }
    free_command(c);
    if (read_fd != -1)
      close(read_fd);
    if (write_fd != -1)
      close(write_fd);

    close(err_pipe[1]);

    int err = 0;
    int n = read(err_pipe[0], &err, sizeof(int));

    if (n == sizeof(int) && err == -1) {
      return -1;
    }
    return pid;

  } else {
    perror("fork");
    return -1;
  }
}

int run_piped(struct Command *commands, int len) {

  int pipefds[2 * (len - 1)];
  int i;

  // Create the necessary pipes
  for (i = 0; i < len - 1; i++) {
    if (pipe(pipefds + i * 2) == -1) {
      perror("pipe");
      return -1;
    }
  }
  int *pids = malloc(len * sizeof(int));

  for (int i = 0; i < len; i++) {
    int read_fd = i == 0 ? -1 : pipefds[(i - 1) * 2];
    int write_fd = i == len - 1 ? -1 : pipefds[i * 2 + 1];
    int fpid = fork_subprocess(commands[i], read_fd, write_fd);
    if (fpid == -1) {
      printf("Failed starting process: %s\n", commands[i].command[0]);
      return -1;
    }
    pids[i] = fpid;
  }
  for (int i = 0; i < len; i++) {
    waitpid(pids[i], NULL, 0);
  }
  free(pids);

  return 0;
}
