#pragma once
#include "Settings.h"
#include "Table.h"
#include "Index.h"

#define MAX_INPUT_SIZE 100
#define INITIAL_ARGS_SIZE 2

typedef enum {
    CMD_HELLO,
    CMD_HELP,
    CMD_EXIT,
    CMD_PRINT,
    CMD_INSERT,
    CMD_SEARCH,
    CMD_DELETE,
    CMD_CLEAR,
    CMD_SHOW,
    CMD_IBIJAU,
    CMD_SELECT,
    CMD_SORT,
    CMD_EXPORT,
    CMD_COUNT,
    CMD_STATS,
    CMD_STRUCTURE,
    CMD_MODIFY
} CommandCode;

// Structures pour les commandes
typedef struct {
    char* name;
    char* description;
    char* argList;
    char* example;
    int argCount;
    int optionAgrCount;
} Command;

typedef struct {
    Command* commands;
    int commandCount;
} Commands;

typedef struct {
    char* name;
} Select;

typedef struct {
    Select* selects;
    int selectCount;
}Selects;

typedef struct {
    Selects* selections;
    int numberOfSelects;
}Selector;

// Codes clavier
typedef enum {
    SCANCODE_UP = 72,
    SCANCODE_DOWN = 80,
    KEY_ENTER = 13,
    KEY_ESC = 27
}KeyCode;

// Modes d'interface
typedef enum {
    MODE_WRITE,
    MODE_SELECT
}Mode;

typedef enum {
    COLOR_BLUE = 1,
    COLOR_GREEN,
    COLOR_LIGHT_BLUE,
    COLOR_RED,
    COLOR_PURPLE,
    COLOR_YELLOW,
    COLOR_WHITE,
    COLOR_GREY,
    COLOR_BLACK_BACKGROUND_WHITE = 240,
} Color;

// Fonctions d'interface utilisateur
void ui_displayWelcome(void);
void ui_clearScreen(void);
void ui_displayError(const char* message);
void ui_displaySuccess(const char* message);
void ui_displayCommandNotFound(void);
void ui_displayArguments(Command* cmd);
void ui_displayExample(Command* cmd);

// Fonctions de gestion des commandes
Commands* commands_create_list(void);
bool commands_add(Commands* commands, Command* cmd);
Command* command_create(const char* name, const char* description, const char* argList, const char* exemple, int argCount, int optionArgCount);
void commands_destroy(Commands* commands);
int commands_getIndex(const Commands* commands, const char* commandName);
void commands_displayHelp(const Commands* cmd, const char* args, int argc);
Command* commands_get(const Commands* commands, int index);

// Fonctions de gestion des sélections
Selects* selects_create(const char** names, int count);
void selects_destroy(Selects* selects);
void selects_displayMenu(const Selects* selects, int currentSelection);
Selector* selector_create(Selects* selects);
void selector_destroy(Selector* selector);
int selector_handleInput(Selector* selector, Mode* mode);

// Fonctions de gestion des arguments
char** args_separation(const char* input, int* argc);
void args_destroy(char** args, int argc);
void args_display(char** args, int argc);

// Fonctions de commandes spécifiques
void cmd_hello(void);
void cmd_exit(void);
void cmd_insert(Table* table, char** args, int argc, const Commands* commands);
void cmd_search(Table* table, char** args, int argc, const Commands* commands);
void cmd_delete(Table* table, char** args, int argc, const Commands* commands);
void cmd_print(Table* table);
void cmd_print_index(Table* table, int indexNum);
void cmd_show(Table* table);
void cmd_selectTable(Table* table, char** args, int argc, const Commands* commands);
void cmd_displayIbijau(void);
void cmd_sort(Table* table, char** args, int argc, const Commands* commands);
void cmd_export(Table* table, char** args, int argc, const Commands* commands);
void cmd_count(Table* table, char** args, int argc, const Commands* commands);
void cmd_stats(Table* table, char** args, int argc, const Commands* commands);
void cmd_structure(Table* table);

// Fonction principale de traitement des commandes
bool handle_command(Table* table, const char* command, char** args, int argc, const Commands* commands, Mode* mode);
RequestOp str_to_op(const char* op);

void search_configFile(char* folderPath, char* csvPath);