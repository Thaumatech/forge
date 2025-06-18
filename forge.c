#include "forge.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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

int main() {
  FILE *blacksmith = fopen("Blacksmith", "r");

  if (blacksmith == NULL) {
    printf("Blacksmith file does not exist. \n");
    return 1;
  }
  char *line = NULL;
  size_t len = 0;

  static int line_num = 0;

  bool syntax_error = false;
  bool got_lang = false;

  char *cached_line = NULL;

  char *trimmed = NULL;

  RuleList *rule_list = init_list(8, "c");

  while (1) {
    if (cached_line) {
      trimmed = chomp(cached_line);
      cached_line = NULL;
    } else {
      if (getline(&line, &len, blacksmith) == -1)
        break;

      line_num++;

      if (line[0] == '\n' || STARTSWITH(line, "//"))
        continue;

      trimmed = chomp(line);
    }

    if (!got_lang && STARTSWITH(trimmed, "for ")) {
      trimmed += 4;
      char *language = NULL;

      if (STREQ(trimmed, "c") || STREQ(trimmed, "architect")) {
        language = "c";
      } else if (STREQ(trimmed, "go") || STREQ(trimmed, "tinkerer")) {
        language = "go";
      } else {
        syntax_error = true;
        break;
      }

      size_t lang_len = strlen(language);

      rule_list->lang = malloc(lang_len + 1);
      if (!rule_list->lang) {
        perror("Allocating language into memory failed.\n");
        return 1;
      }
      strcpy(rule_list->lang, language);

      got_lang = true;
      continue;
    }

    if (STARTSWITH(trimmed, "forge ") || STARTSWITH(trimmed, "smelt ")) {
      Rule *new_rule = init_rule();
      init_action_target(new_rule, trimmed);

      while (getline(&line, &len, blacksmith) != -1) {
        line_num++;

        if (line[0] == '\n' || STARTSWITH(line, "//"))
          continue;

        trimmed = chomp(line);

        if (STARTSWITH(trimmed, "forge ") || STARTSWITH(trimmed, "smelt ")) {
          if (!new_rule->needs.deps) {
            printf(
                "Dependencies are mandetory. Please provide needs before line "
                "%d's forge.\n",
                line_num);
            return 1;
          }

          append(rule_list, *new_rule);

          cached_line = strdup(trimmed);
          if (!cached_line) {
            printf("Failed to cache line. Line: %d.\n", line_num);
            return 1;
          }
          break;
        } else if (STARTSWITH(trimmed, "scrap ")) {
          cached_line = strdup(trimmed);
          if (!cached_line) {
            printf("Failed to cache line. Line: %d.\n", line_num);
            return 1;
          }
          break;
        }

        if (STARTSWITH(trimmed, "needs ")) {
          trimmed += 6;
          init_needs(new_rule, trimmed);
        } else if (STARTSWITH(trimmed, "imbue ")) {
          trimmed += 6;
          init_imbue(new_rule, trimmed);
        } else {
          syntax_error = true;
          break;
        }

        if (!got_lang) {
          rule_list->lang = strdup("c");
          got_lang = true;
        }
      }
    } else if (STARTSWITH(trimmed, "scrap ")) {
      // TO DO
    } else {
      syntax_error = true;
      break;
    }
    if (syntax_error) {
      break;
    }
  }

  printf("Language: %s\n", rule_list->lang);
  for (int i = 0; i < rule_list->size; i++) {
    printf("Rule %d\n", i + 1);
    printf("------------------\n");
    printf("Action: %s\n", rule_list->data[i].action);
    printf("Target: %s\n", rule_list->data[i].target);
    printf("Needs: \n");
    for (int j = 0; j < rule_list->data[i].needs.dep_count; j++) {
      printf(" - %s\n", rule_list->data[i].needs.deps[j]);
    }
    printf("------------------\n");
    printf("Imbue: %s\n", rule_list->data[i].imbue.flags);
    printf("------------------\n");
  }

  if (syntax_error) {
    printf("Syntax error! Line %d\n", line_num);
    return 1;
  }

  free_list(rule_list);
  fclose(blacksmith);
  return 0;
}
