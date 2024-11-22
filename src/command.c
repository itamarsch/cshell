#define _GNU_SOURCE
#include "./command.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int count_words(const char *str) {
  int in_word = 0;
  int word_count = 0;

  while (*str) {
    if (isspace(*str)) {
      in_word = 0; // We're outside a word
    } else if (!in_word) {
      in_word = 1; // We've encountered the start of a new word
      word_count++;
    }
    str++;
  }

  return word_count;
}
int count_char(const char *haystack, char needle) {

  int pipes = 0;
  while (*haystack) {
    if (*haystack == needle) {
      pipes++;
    }
    haystack++;
  }
  return pipes;
}
int count_pipes(const char *str) { return count_char(str, '|'); }

void print_command(struct Command c) {
  for (int i = 0; i < c.amount_of_env_vars; i++) {
    printf("%s=%s ", c.env_vars[i].key, c.env_vars[i].value);
  }

  for (int i = 0; i < c.amount_of_args; i++) {
    printf("%s ", c.command[i]);
    i++;
  }

  printf("NULL\n");
}

int parse_piped_command(char *line, struct Command **c) {
  int amount_of_comands_in_pipline = count_pipes(line) + 1;
  struct Command *commands =
      malloc(amount_of_comands_in_pipline * sizeof(struct Command));

  int i = 0;

  char *command;

  while ((command = strsep(&line, "|")) != NULL) {
    struct Command parsed_command = {0};
    int command_len = strlen(command);
    char *command_cpy =
        malloc((command_len + 1) * sizeof(char)); // +1 for null terminator
    if (command_cpy == NULL) {
      perror("malloc");
      free(commands);
      return -1;
    }

    strcpy(command_cpy, command);
    int err = parse_command(command_cpy, &parsed_command);
    if (err < 0) {
      printf("Invalid piping\n");
      free(commands);
      return -1;
    }
    commands[i++] = parsed_command;
  }

  *c = commands;

  return amount_of_comands_in_pipline;
}

int parse_envvar(char *arg, struct EnvVar *envvar) {
  char *saveptr;
  envvar->key = strtok_r(arg, "=", &saveptr);
  envvar->value = strtok_r(NULL, "=", &saveptr);
  return 0;
}

int parse_command(char *command, struct Command *c) {
  int num_words = count_words(command);
  if (num_words < 1) {
    return -1;
  }

  // Allocate memory for storing the tokens (Plus one for NULL terminator
  // of args)
  char **args = malloc((num_words + 1) * sizeof(char *));
  if (args == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  args[num_words] = NULL;

  char *saveptr;
  int i = 0;
  int ended_envvars = 0;
  int amount_of_envvars = 0;
  char *arg = strtok_r(command, " ", &saveptr);
  while (arg != NULL) {
    if (!ended_envvars) {
      switch (count_char(arg, '=')) {
      case 0:
        ended_envvars = 1;
        break;
      case 1:
        amount_of_envvars++;
        break;
      default:
        return -1;
      }
    }

    args[i] = arg;
    arg = strtok_r(NULL, " ", &saveptr);
    i++;
  }
  struct EnvVar *env_vars = NULL;
  if (amount_of_envvars != 0) {
    env_vars = malloc(amount_of_envvars * sizeof(struct EnvVar));
    for (int i = 0; i < amount_of_envvars; i++) {
      struct EnvVar env_var = {0};
      char *saveptr;
      env_var.key = strtok_r(args[i], "=", &saveptr);
      env_var.value = strtok_r(NULL, "=", &saveptr);

      env_vars[i] = env_var;
    }
  }
  c->command = &args[amount_of_envvars];
  c->amount_of_args = num_words - amount_of_envvars;
  c->env_vars = env_vars;
  c->amount_of_env_vars = amount_of_envvars;
  c->full_command = command;
  c->full_args = args;
  return 0;
}

void free_command(struct Command command) {
  free(command.full_command);
  free(command.full_args);
  free(command.env_vars);
}
