
struct Command {
  char **command;
  int amount_of_args;
  struct EnvVar *env_vars;
  int amount_of_env_vars;

  char *full_command;
  char **full_args;
};

struct EnvVar {
  char *key;
  char *value;
};
int parse_command(char *command, struct Command *c);

void print_command(struct Command c);
void free_command(struct Command command);

int parse_piped_command(char *line, struct Command **c);
