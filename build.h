#pragma once

#include "rules.h"
#include <stdio.h>

typedef struct {
  int child_num;
  char *name;
  struct BuildItem *parent;
  struct BuildItem **children;
} BuildItem;

typedef struct {
  int size;
  int capacity;
  char **data;
} BuildQueue;

void build(RuleList *rl);
void compile(char *str);
void compile_go(char *str);
BuildQueue *init_bq(int capacity);
BuildItem *init_i();
void bq_append(BuildQueue *bq, BuildItem *item);
void free_bq(BuildQueue *bq);
void form_queue(RuleList *rl);
void build(RuleList *rl);
void compile(char *str);
