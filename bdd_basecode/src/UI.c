#include "UI.h"

#define MAX_INPUT_SIZE 100

void hello() {
    printf("Hello World!\n");
}

void help(Commands* commandsStruct) {
    printf("Available commands:\n");
    for (int i = 0; i < commandsStruct->commandCount; i++) {
        printf("%s: %s\n", commandsStruct->commands[i].name, commandsStruct->commands[i].description);
        if (commandsStruct->commands[i].argCount != 0) {
            printf("Arguments:\n");
            for (int j = 0; j < commandsStruct->commands[i].argCount; j++) {
			    printf("    %s\n", commandsStruct->commands[i].arg[j].name);
		    }
        }
        printf("\n");
    }
}

void exit_program() {
    printf("Exiting the program. Goodbye!\n");
    exit(0);
}

void printFile(Select* select) {
    printf("Printing file %s\n", select->name);

}

void insert_into_table(char *input, char ** argv, int argc, Commands* commandsStruct) {
    printf("Inserting into BDD\n");
    Command command = commandsStruct->commands[get_command_number(input, commandsStruct)];
    if (argc < 3) {
        printf("No arguments for this command. You must specify %d arguments.\n", command.argCount);
        return;
    } else if (argc > 3) {
		printf("Too many arguments for this command. You must specify %d arguments.\n", command.argCount);
		return;
	}

    printf("arg count: %d\n", command.argCount);

    for (int i = 0; i < command.argCount; i++) {
		printf("Argument %s: %s\n", command.arg[i].name, argv[i]);
	}
}

void delete_into_table(char *input, char ** argv, int argc, Commands* commandsStruct) {
	printf("Deleting from BDD\n");
	Command command = commandsStruct->commands[get_command_number(input, commandsStruct)];
	if (argc < 3) {
		printf("No arguments for this command. You must specify %d arguments.\n", command.argCount);
		return;
	} else if (argc > 3) {
		printf("Too many arguments for this command. You must specify %d arguments.\n", command.argCount);
		return;
	}

	printf("arg count: %d\n", command.argCount);

	for (int i = 0; i < command.argCount; i++) {
		printf("Argument %s: %s\n", command.arg[i].name, argv[i]);
	}
}

void displaySelectMenu(Selects* selects, int currentSelection) {
    system("cls");
    printf("Press the up and down arrow keys to navigate, enter to select, and escape to return to the previous menu.\n");
    printf("Select an option:\n");
    for (int i = 0; i < selects->selectCount; i++) {
        if (i == currentSelection) {
            printf("-> %s\n", selects->selects[i].name);
        }
        else {
            printf("   %s\n", selects->selects[i].name);
        }
    }
}

int selectMode(Selector* selector, Mode* mode) {
    Selects* selects = selector->selections;
    int selectCount = selects->selectCount;
    int currentSelection = 0;
    displaySelectMenu(selects, currentSelection);

    while (1) {
        int ch = _getch();

        if (ch == 224) {
            ch = _getch();

            if (ch == SCANCODE_UP) {
                currentSelection = (currentSelection - 1 + selectCount) % selectCount;
            }

            else if (ch == SCANCODE_DOWN) {
                currentSelection = (currentSelection + 1) % selectCount;
            }

            displaySelectMenu(selects, currentSelection);
        }

        else if (ch == KEY_ENTER) {
            return currentSelection;
        }

        else if (ch == KEY_ESC) {
            *mode = MODE_WRITE;
            return -1;
        }
    }
}

Selects *create_selects(char* names[], int count) {
	Selects* selects = calloc(1, sizeof(Selects));
	if (!selects) {
		printf("Memory allocation failed.\n");
		return NULL;
	}
	selects->selectCount = count;
	selects->selects = calloc(selects->selectCount, sizeof(Select));
	if (!selects->selects) {
		printf("Memory allocation failed.\n");
		free(selects);
		return NULL;
	}

    for (int i = 0; i < count; i++) {
		selects->selects[i].name = names[i];
	}
	
	return selects;
}

Selector* create_selector(Selects* selects, int count) {
	Selector* selector = calloc(1, sizeof(Selector));
	if (!selector) {
		printf("Memory allocation failed.\n");
		return NULL;
	}
    selector->selections = selects;
    selector->numberOfSelects = count;

	return selector;
}

int get_command_number(char* command, Commands* commandsStruct) {
	for (int i = 0; i < commandsStruct->commandCount; i++) {
		if (strcmp(command, commandsStruct->commands[i].name) == 0) {
			return i;
		}
	}
	return -1;
}