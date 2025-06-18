#include "forge.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

void chomp(char *s) {
  size_t len_s = strlen(s);
  if (len_s > 0 && s[len_s - 1] == '\n')
    s[len_s - 1] = '\0';
}

int main() {
  FILE *blacksmith = fopen("Blacksmith", "r");

  if (blacksmith == NULL) {
    printf("Blacksmith file does not exist. \n");
    return 1;
  }
  char *line = NULL;
  size_t len = 0;

  int line_num = 0;

  bool syntax_error = false;
  bool got_lang = false;

  RuleList *rule_list = init_list(8, "c");

  while (getline(&line, &len, blacksmith) != -1) {
    line_num++;
    char *trimmed = line;

    if (line[0] == '\n' || STARTSWITH(line, "//"))
      continue;

    while (isspace(*trimmed))
      trimmed++;

    chomp(trimmed);

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
    }

    if (STARTSWITH(trimmed, "forge ") || STARTSWITH(trimmed, "smelt ") ||
        STARTSWITH(trimmed, "scrap ")) {
      Rule *new_rule = init_rule();
      init_action_target(new_rule, trimmed);

      while (getline(&line, &len, blacksmith) != -1) {
        line_num++;
        trimmed = line;

        if (line[0] == '\n' || STARTSWITH(line, "//"))
          continue;

        while (isspace(*trimmed))
          trimmed++;

        chomp(trimmed);

        if (STARTSWITH(trimmed, "forge ") || STARTSWITH(trimmed, "smelt ") ||
            STARTSWITH(trimmed, "scrap ")) {
          break;
        }

        if (!got_lang) {
          rule_list->lang = malloc(1);
          rule_list->lang[0] = 'c';
          got_lang = true;
        }
      }
    }

    printf("%s\n", rule_list->lang);

    if (syntax_error) {
      printf("Syntax error! Line %d", line_num);
      return 1;
    }

    free_list(rule_list);
    fclose(blacksmith);
    return 0;
  }
