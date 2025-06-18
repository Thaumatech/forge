#pragma once
// rules.h

#define STARTSWITH(str, prefix) (strncmp((str), (prefix), strlen(prefix)) == 0)
#define STREQ(a, b) (strcmp((a), (b)) == 0)

// Dependencies such as object files or plain C files.
typedef struct {
  int dep_count;
  char **deps;
} Needs;

// Imbue, for custom flags when compiling a C program.
typedef struct {
  char *flags;
} Imbue;

// A Rule, meaning a chunk of text in the Blacksmith file.
// E.g:
//
// forge main.c
//    needs main.o, foo.o
//    imbue -Wall -v
typedef struct {
  char *action;
  char *target;
  Needs needs;
  Imbue imbue;
} Rule;

// List of all Rules. Also stores the language that the
// rest of the code will be trying to compile.
typedef struct {
  int size;
  int capacity;
  char *lang;
  Rule **data;
} RuleList;

// Returns pointer to rule list
RuleList *init_list(int capacity, char *lang);

// Returns pointer to rule
Rule *init_rule();

// Appends a Rule to the Rule List
void append(RuleList *list, Rule *rule);

// Free the entire Rule Lists and all of its contents
void free_list(RuleList *list);

// Sets the Rule's flags to the passed string of flags
void init_imbue(Rule *rule, char *flags);

// Parses the deps string into a list of dependencies, the
// delimiters being ' ' (space), then initializes and sets
// The Rule's needs' dependencies and it's count to their
// appropriate values.
void init_needs(Rule *rule, char *deps);

// Takes the first five characters out of s (forge, smelt, scrap keywords)
// and then sets the rule's action property to that newly
// made word.
// ****** UPDATE LATER TO TRAVERSE UNTIL A SPACE ******
void init_action_target(Rule *rule, char *s);
