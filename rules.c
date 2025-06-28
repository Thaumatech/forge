#include "rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RuleList *init_list(int capacity, char *lang) {
  RuleList *list = malloc(sizeof(RuleList));
  if (!list)
    return NULL;

  list->data = malloc(sizeof(Rule *) * capacity);
  if (!list->data) {
    free_list(list);
    return NULL;
  }

  list->lang = strdup(lang);
  if (!list->lang) {
    free_list(list);
    return NULL;
  }

  list->size = 0;
  list->capacity = capacity;
  return list;
}

Rule *init_rule() {
  Rule *rule = malloc(sizeof(Rule));
  if (!rule) {
    printf("Mallocation failed for init_rule\n");
    return NULL;
  }

  return rule;
}

void append(RuleList *list, Rule *rule) {
  if (list->capacity <= list->size) {
    int new_capacity = list->capacity * 2;
    Rule **new_data = realloc(list->data, new_capacity * sizeof(Rule *));

    if (!new_data) {
      printf("Appending rule failed! Blame realloc not me\n");
      return;
    }

    list->capacity *= 2;
    list->data = new_data;
  }

  list->data[list->size++] = rule;
}

void free_list(RuleList *list) {
  if (!list)
    return;

  if (list->data && list->size) {
    for (int i = 0; i < list->size; i++) {
      Rule *r = list->data[i];

      if (r->needs.deps) {
        for (int j = 0; j < r->needs.dep_count; j++) {
          free(r->needs.deps[j]);
        }
        free(r->needs.deps);
      }

      if (r->imbue.flags)
        free(r->imbue.flags);
      if (r->action)
        free(r->action);
      if (r->target)
        free(r->target);

      free(r);
    }
    free(list->data);
  }
  if (list->lang) {
    free(list->lang);
  }
  free(list);
}

void init_imbue(Rule *rule, char *flags) {
  if (rule->imbue.flags) {
    printf("Rule already contains imbue flags!\n");
    return;
  }
  char *new_flags = strdup(flags);
  if (!new_flags)
    return;
  rule->imbue.flags = new_flags;
}

void init_needs(Rule *rule, char *deps) {
  if (!deps)
    return;

  int count = 0;
  int cap = 8;

  rule->needs.deps = malloc(cap * sizeof(char *));
  if (!rule->needs.deps) {
    perror("Failed to initialize rule's dependencies\n");
    return;
  }

  int start = 0;
  int i = 0;

  while (1) {
    if (deps[i] == ' ' || deps[i] == '\n' || deps[i] == '\0') {
      if (start < i) {
        int len = i - start;
        char *word = malloc(len + 1);
        if (!word) {
          perror("Failed to allocate dep\n");
          return;
        }

        for (int j = 0; j < len; j++) {
          word[j] = deps[start + j];
        }
        word[len] = '\0';

        if (count >= cap) {
          int new_cap = cap * 2;
          char **new_deps = realloc(rule->needs.deps, new_cap * sizeof(char *));
          if (!new_deps) {
            perror("Failed to allocate new dep\n");
            return;
          }
          rule->needs.deps = new_deps;
        }

        rule->needs.deps[count++] = word;
      }
      if (deps[i] == '\0')
        break;
      start = i + 1;
    }
    i++;
  }

  rule->needs.dep_count = count;
}

void init_action_target(Rule *rule, char *s) {
  int space = 0;
  while (s[space] != '\0' && s[space] != ' ')
    space++;

  if (s[space] != ' ') {
    perror("Syntax Error when initializing action / target.");
    return;
  }

  char action[space + 1];
  strncpy(action, s, space);
  action[space] = '\0';

  char *target = s + space + 1;
  size_t target_len = strlen(target);
  if (target_len > 0 && target[target_len - 1] == ':') {
    target[target_len - 1] = '\0';
  }

  rule->action = strdup(action);
  rule->target = strdup(target);
}
