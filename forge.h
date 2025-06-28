#pragma once

#include "rules.h"
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void loading();
void build_mm(RuleList *rl);
void build(RuleList *rl);
RuleList *parse(FILE *blacksmith);
