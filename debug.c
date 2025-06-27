#include "debug.h"
#include "rules.h"
#include <stdio.h>
#include <unistd.h>

void print_list(RuleList *rule_list) {
  printf("Language: %s\n", rule_list->lang);
  for (int i = 0; i < rule_list->size; i++) {
    printf("Rule %d\n", i + 1);
    printf("------------------\n");
    printf("Action: %s\n", rule_list->data[i]->action);
    printf("Target: %s\n", rule_list->data[i]->target);
    printf("Needs: \n");
    for (int j = 0; j < rule_list->data[i]->needs.dep_count; j++) {
      printf(" - %s\n", rule_list->data[i]->needs.deps[j]);
    }
    printf("------------------\n");
    printf("Imbue: %s\n", rule_list->data[i]->imbue.flags);
    printf("------------------\n");
  }

  sleep(20);
}
