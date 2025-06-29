#pragma once

#include "rules.h"
#include <stdio.h>

typedef struct {
  int size;
  int capacity;
  char **data;
} BuildQueue;

void build(RuleList *rl);
void compile(char *str);
void compile_go(char *str);
