#pragma once
#include "Settings.h"

#define MAX_INPUT_SIZE 100
#define INITIAL_SIZE 2

typedef struct argument {
	char* name;
	char* value;
} Argument;

typedef struct command {
    char* name;
    char* description;
    Argument* arg;
    int argCount;
} Command;

typedef struct commands {
    Command* commands;
    int commandCount;
} Commands;

typedef struct select {
    char* name;
} Select;

typedef struct selects {
    Select* selects;
    int selectCount;
} Selects;

typedef struct selector {
    Selects* selections;
    int numberOfSelects;
} Selector;

typedef enum {
    SCANCODE_UP = 72,
    SCANCODE_DOWN = 80,
    KEY_ENTER = 13,
    KEY_ESC = 27
} KeyCode;

typedef enum {
    MODE_WRITE,
    MODE_SELECT
} Mode;

void hello();
void help(Commands* commandsStruct);
void exit_program();
void displaySelectMenu(Selects* selects, int currentSelection);
int selectMode(Selector* selector, Mode* mode);
void printFile(Select* select);
Selects* create_selects(char* names[], int count);
Selector* create_selector(Selects* selects, int count);
void insert_into_table(char *input, char** argv, int argc, Commands* commandsStruct);
void delete_into_table(char* input, char** argv, int argc, Commands* commandsStruct);