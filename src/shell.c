#include <stdio.h>
#define _GNU_SOURCE

#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./command.h"
#include "./prompt.h"
#include "./runner.h"

char *builtins[] = {"cd", "export"};
int num_strings = sizeof(builtins) / sizeof(builtins[0]);

int command_is_builtin(char *str) {
  for (int i = 0; i < num_strings; i++) {
    if (strcmp(str, builtins[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

int main() {

  while (1) {
    char *line = NULL;
    char *prompt_str = prompt();

    line = readline(prompt_str);
    if (line == NULL) {
      exit(EXIT_SUCCESS);
    }
    add_history(line);

    struct Command *commands;
    int length = parse_piped_command(line, &commands);
    if (length > 0) {
      if (length == 1 && command_is_builtin(commands[0].command[0])) {
        if (strcmp(commands->command[0], "cd") == 0) {
          struct Command command = commands[0];
          if (command.amount_of_args == 2) {
            char *dir = strcmp(command.command[1], "~") == 0
                            ? home_dir()
                            : command.command[1];
            int err = chdir(dir);

            if (err == -1) {
              perror("chdir");
            }
          }
        } else {
          if (strcmp(commands->command[0], "export") == 0) {
            printf("Invalid calling of cd");
          }
        }
      } else {
        run_piped(commands, length);
      }
    }

    free(commands);
    free(prompt_str);
    free(line);
  }
  return 0;
}
