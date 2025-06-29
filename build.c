#include "build.h"
#include "rules.h"
#include <stdio.h>

BuildQueue init_bq(int capacity) {
  BuildQueue *bq = malloc(sizeof(BuildQueue));
  if (!bq)
    return NULL;

  bq->data = malloc(sizeof(char *) * capacity);
  if (!bq->data)
    return NULL;

  bq->size = 0;
  bq->capacity = capacity;
  return bq;
}

void bq_append(BuildQueue *bq, char **item) {
  if (bq->capacity <= bq->size) {
    int new_capacity = bq->capacity * 2;
    char **new_data = realloc(bq->data, new_capacity * sizeof(char *));

    if (!new_data) {
      perror("Appending build item failed.\n");
      return;
    }

    bq->capacity *= 2;
    bq->data = new_data;
  }

  bq->data[bq->size++] = item;
}

void free_bq(BuildQueue *bq) {
  if (!bq) {
    perror("Build Queue does not exist.\n");
    return;
  }

  if (bq->data && bq->size) {
    for (int i = 0; i < bq->size; i++) {
      free(bq->data[i]);
    }
    free(bq->data);
  }
  free(bq);
}

void form_queue(RuleList *rl) {
  BuildQueue *bq = malloc(sizeof(BuildQueue));
  for (int i = 0; i < rl->size; i++) {
    Rule *dp = rl->data[i];

    for (int j = 0; j < dp->needs.dep_count; j++) {
      bq_append(b, dp->needs.deps[j]);
    }

    free(dp);
  }
}

void build(RuleList *rl) {}

void compile(char *str) { system(str); }

void compile_go(char *str) { system(str); }
