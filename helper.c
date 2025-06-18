#include "helper.h"
#include <ctype.h>
#include <strings.h>

char *chomp(char *s) {
  while (isspace(*s))
    s++;

  size_t len_s = strlen(s);
  while (len_s > 0 && isspace(s[len_s - 1])) {
    s[--len_s] = '\0';
  }

  return s;
}
