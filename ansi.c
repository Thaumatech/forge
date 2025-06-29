

#include <stdio.h>

void hide_cursor() { printf("[?25l"); }

void show_cursor() { printf("[?25h"); }
