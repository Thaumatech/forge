#include "debug.h"
#include "helper.h"
#include "rules.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <unistd.h>

void loading() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  bool anim = true;

  const char *up[] = {
      "        ____         ",  " ______/____\\_____   ",
      "/|                |\\ ", "||       /\\       || ",
      "||       \\/       || ", "\\|________________|/ ",
      "        |  |__       ",  "       /|__|  \\______",
      "      (              ",  "      (              ",
      "      (______________",  "        |  |         ",
      "        |  |         ",  "        |__|         ",
      "        \\__/         ",
  };

  //   int uplen = term_cols - 5;

  const char *strike[] = {
      "      _______                     ",
      "     /_______\\   |      |         ",
      "    |         |  |      |         ",
      "    |         | /       |       _ ",
      "    |         |--|      |------/ \\",
      "    |   < >   |__|      |______\\_/",
      "    |         | \\_/\\_/\\_/         ",
      "    |         |                   ",
      "    |_________|                   ",
      "_____\\_______/_____________       ",
  };

  //   int strikelen = term_cols - 17;

  while (1) {
    sleep(1);
    system("clear");
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    if (anim) {
      int padding_y = (w.ws_row / 2) - (sizeof(up) / sizeof(up[0]));
      int padding_x = (w.ws_col / 2) - (strlen(up[0]) / 2);

      int height = (sizeof(up) / sizeof(up[0]));

      for (int i = 0; i < padding_y; i++) {
        printf("\n");
      }
      for (int i = 0; i < height; i++) {
        for (int j = 0; j < padding_x; j++) {
          printf(" ");
        }
        printf("%s\n", up[i]);
      }
    } else {
      int padding_y = (w.ws_row / 2) - (sizeof(strike) / sizeof(strike[0]));
      int padding_x = (w.ws_col / 2) - (strlen(strike[0]) / 2);

      int height = (sizeof(strike) / sizeof(strike[0]));

      for (int i = 0; i < padding_y; i++) {
        printf("\n");
      }
      for (int i = 0; i < padding_y; i++) {
        for (int j = 0; j < padding_x; j++) {
          printf(" ");
        }
        printf("%s\n", strike[i]);
      }
    }
    anim = !anim;
  }
}

void build(RuleList *rl) {
  pid_t p = fork();
  if (p < 0) {
    perror("Fork Failed\n");
    return;
  }
  if (p == 0) {
    printf("HELLO CHILD FORK\n");

    loading();
    return;
  }
  if (p > 0) {
    printf("HELLO PARENT FORK\n");
    return;
  }
}

RuleList *parse(FILE *blacksmith) {
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
        return NULL;
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
            printf("Dependencies are mandetory. Please provide needs before "
                   "line "
                   "%d's forge.\n",
                   line_num);
            return NULL;
          }

          append(rule_list, new_rule);

          cached_line = strdup(trimmed);
          if (!cached_line) {
            printf("Failed to cache line. Line: %d.\n", line_num);
            return NULL;
          }
          break;
        } else if (STARTSWITH(trimmed, "scrap ")) {
          cached_line = strdup(trimmed);
          if (!cached_line) {
            printf("Failed to cache line. Line: %d.\n", line_num);
            return NULL;
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

  if (syntax_error) {
    printf("Syntax error! Line %d\n", line_num);
    return NULL;
  }

  return rule_list;
}

int main() {
  FILE *blacksmith = fopen("Blacksmith", "r");

  if (blacksmith == NULL) {
    printf("Blacksmith file does not exist. \n");
    return 1;
  }

  RuleList *rule_list = parse(blacksmith);

  if (!rule_list) {
    return 1;
  }

  build(rule_list);

  free_list(rule_list);
  fclose(blacksmith);
  return 0;
}
