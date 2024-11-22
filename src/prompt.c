
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int is_prefix(const char *prefix, const char *str) {
  size_t len_prefix = strlen(prefix);

  size_t len_str = strlen(str);

  // If the prefix is longer than the string, it cannot be a prefix
  if (len_prefix > len_str) {
    return 0;
  }

  // Compare the first len_prefix characters of str with prefix
  return strncmp(prefix, str, len_prefix) == 0;
}

char *_home_dir = NULL;
char *home_dir(void) {
  if (_home_dir == NULL) {

    char *user = getenv("USER");

    int len = snprintf(NULL, 0, "/home/%s", user);
    _home_dir = malloc((len + 1) * sizeof(char));

    snprintf(_home_dir, len + 1, "/home/%s", user);
  }

  return _home_dir;
}
char *prompt() {
  char *cwd = getcwd(NULL, 0);
  char *full_cwd = cwd;
  int home_dir_len = strlen(home_dir());

  if (is_prefix(home_dir(), cwd)) {
    cwd[home_dir_len - 1] = '~';
    cwd = &cwd[home_dir_len - 1];
  }

  int len = snprintf(NULL, 0, "%s>", cwd);
  char *prompt = malloc((len + 1) * sizeof(char));
  snprintf(prompt, len + 1, "%s>", cwd);
  free(full_cwd);
  return prompt;
}
