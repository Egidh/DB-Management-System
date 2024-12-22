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
} Command;


// Codes clavier
typedef enum {
    SCANCODE_UP = 72,
    SCANCODE_DOWN = 80,
    KEY_ENTER = 13,
    KEY_ESC = 27
}KeyCode;

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


void print_color(const char* text, Color color);
void ui_displayWelcome(void);

///@brief Affiche les statistiques principales de la table
///@param table La table concernée
void cmd_stats(Table* table);

///@brief oui
///@param table La table concernée
void cmd_structure(Table* table);