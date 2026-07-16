#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "kernel.h"

void keyboard_install();

// Blocking read of a single character typed on the keyboard.
char keyboard_getchar();

// Non-blocking: returns true and fills *c if a character is ready.
bool keyboard_poll(char* c);

#endif
