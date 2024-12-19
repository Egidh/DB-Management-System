/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN and Matthieu LE BERRE.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "Settings.h"
#include "Table.h"
#include "Index.h"
#include "UI.h"


//load from file
#if 0
int main(int argc, char** argv)
{
    Table* table = Table_load("psittamulgiformes.tbl", "../data/intro_1");

    //Entry* cherry = Entry_create(table);
    //strcpy(cherry->values[0], "Cherry");
    //strcpy(cherry->values[1], "Quokka");
    //strcpy(cherry->values[2], "36");

    //Table_insertEntry(table, cherry);

    Filter filter = { 1, OP_EQ, "Quokka", "Pepsi" };
    SetEntry* results = SetEntry_create();
    Table_search(table, &filter, results);

    Table_printSearchResult(results, table);

    Table_debugPrint(table);

    SetEntry_destroy(results);
    Table_destroy(table);

    return EXIT_SUCCESS;
}
#endif

//create from scratch
#if 0
int main(int argc, char** argv)
{
    //Table* table = Table_load("psittamulgiformes.tbl", "../data/intro/");
    Table* table = Table_createFromCSV("../data/intro_1/psittamulgiformes.csv", "../data/intro_1");

    /*Entry* cherry = Entry_create(table);
    strcpy(cherry->values[0], "Cherry");
    strcpy(cherry->values[1], "Quokka");
    strcpy(cherry->values[2], "36");

    Table_insertEntry(table, cherry);

    Filter filter = {0, OP_EQ, "Cherry", ""};
    SetEntry* entry = SetEntry_create();
    Table_search(table, &filter, entry);

    Entry* result = Entry_create(table);
    if (entry->root)
        Table_readEntry(table, result, entry->root->data);
    else printf("not found\n");

    Entry_print(result);

    Table_debugPrint(table);

    Entry_destroy(result);
    Entry_destroy(cherry);
    SetEntry_destroy(entry);*/
    Table_destroy(table);

    return EXIT_SUCCESS;
}
#endif

#if 0
// Exemple de main() :
// Création d'une table à partir d'un CSV.
int main(int argc, char** argv)
{
    char *folderPath = "../data/intro";
    char *csvPath = "../data/intro/psittamulgiformes.csv";

    Table *table = Table_createFromCSV(csvPath, folderPath);
    Table_debugPrint(table);
    Table_destroy(table); table = NULL;

    return EXIT_SUCCESS;
}
#endif

#if 0
// Exemple de main() :
// Recherche dans une table sur un attribut indexé
int main(int argc, char** argv)
{
    char *folderPath = "../data/intro";
    char *tblFilename = "psittamulgiformes.tbl";

    // Ouverture de la table
    Table *table = Table_load(tblFilename, folderPath);
    Table_debugPrint(table);

    // Création du filtre de la recherche
    Filter filter = { 0 };
    filter.attributeIndex = 1;
    filter.key1 = "Ibijau";
    filter.requestOp = OP_EQ;

    // Recherche
    SetEntry *result = SetEntry_create();
    Table_search(table, &filter, result);

    // Affichage du résultat
    printf("Result count = %d\n", SetEntry_size(result));

    SetEntryIter *it = SetEntryIter_create(result);
    Entry *entry = Entry_create(table);
    while (SetEntryIter_isValid(it))
    {
        printf("----\n");
        EntryPointer entryPtr = SetEntryIter_getValue(it);
        Table_readEntry(table, entry, entryPtr);
        Entry_print(entry);

        SetEntryIter_next(it);
    }
    SetEntryIter_destroy(it); it = NULL;
    Entry_destroy(entry); entry = NULL;

    // Libération de la mémoire
    SetEntry_destroy(result); result = NULL;
    Table_destroy(table); table = NULL;

    return EXIT_SUCCESS;
}
#endif

#if 1
int main() {
    Table* table = Table_createFromCSV("../data/intro_1/psittamulgiformes.csv", "../data/intro_1");

    SetConsoleOutputCP(CP_UTF8);
    char input[MAX_INPUT_SIZE];

    SetConsoleTitle("BBD");

    COORD consoleSize = { 0 };
    consoleSize = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));
    SMALL_RECT windowSize = { 0, 0, consoleSize.X - 1, consoleSize.Y - 1 };
    DWORD nFont = 0;
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
    GetConsoleFontSize(GetStdHandle(STD_OUTPUT_HANDLE), nFont);

    printf("╭──────────────────────Welcome To The BBD──────────────────────╮\n");
    printf("Type 'help' for a list of commands.\n");

    Argument *insert_arg = calloc(table->attributeCount, sizeof(Argument));
    if (!insert_arg) {
		printf("Memory allocation failed.\n");
		return 1;
	}

    for (int i = 0; i < table->attributeCount; i++) {
		insert_arg[i].name = table->attributes[i].name;
		insert_arg[i].value = calloc(1, MAX_INPUT_SIZE);
		if (!insert_arg[i].value) {
			printf("Memory allocation failed.\n");
			return 1;
		}
	}

    Command commands[] = {
        {"hello", "Say hello!"},
        {"help", "Displays a list of commands"},
        {"exit", "Exits the program"},
        {"print", "Print a file"},
		{"create", "Create a table from a CSV file"},
		{"load", "Load a table from a file"},
        {"insert", "Insert an entry into the table", insert_arg, table->attributeCount},
		{"search", "Search for entries in the table"},
		{"delete", "Delete an entry from the table"},
		{"update", "Update an entry in the table"},
		{"print", "Print the table"},
		{"createIndex", "Create an index on an attribute"},
		{"deleteIndex", "Delete an index on an attribute"},
		{"printIndex", "Print an index"},
		{"select", "Select a mode to display data"},
		{"debug", "Debug print the table"},
        {"clear", "Clear the console"},
    };
    int commandCount = sizeof(commands) / sizeof(Command);

    Commands* commandsStruct = calloc(1, sizeof(Commands));
    if (!commandsStruct) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    commandsStruct->commands = commands;
    commandsStruct->commandCount = commandCount;

    char *names[] = { "Table", "Data", "Index 0", "Index 1", "Index 2" };
    int count = sizeof(names) / sizeof(char*);
    Selects* selects = create_selects(names, count);

    int countSelects = selects->selectCount;
    Selector* selector = create_selector(selects, countSelects);

    Mode mode = MODE_WRITE;

    while (1) {
        if (mode == MODE_WRITE) {
            printf("> ");
            if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
                break;
            }
            input[strcspn(input, "\n")] = 0;


            char** argv = calloc(INITIAL_SIZE, sizeof(char*));
            if (argv == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }

            int argc = 0;
            int capacity = INITIAL_SIZE;

            char* strToken = strtok(input, " ");
            char * command = strToken;
            strToken = strtok(NULL, " ");
            while (strToken != NULL) {
                if (argc >= capacity) {
                    capacity *= 2;
                    argv = realloc(argv, capacity * sizeof(char*));
                    if (argv == NULL) {
                        fprintf(stderr, "Memory reallocation failed\n");
                        return 1;
                    }
                }

                argv[argc++] = strToken;
                strToken = strtok(NULL, " ");
            }

            for (int i = 0; i < argc; i++) {
                printf("Argument %d: %s\n", i, argv[i]);
            }

            if (strcmp(command, "help") == 0) {
                help(commandsStruct);
            }
            else if (strcmp(command, "hello") == 0) {
                hello();
            }
            else if (strcmp(command, "exit") == 0) {
                exit_program();
            }
            else if (strcmp(command, "print") == 0) {
                mode = MODE_SELECT;
            }
            else if (strcmp(command, "insert") == 0) {

                insert_into_table(command, argv, argc, commandsStruct);
            }
            else {
                printf("Command not found. Type 'help' for a list of commands.\n");
            }

            free(argv);
        }
        else if (mode == MODE_SELECT) {
            int currentSelection = selectMode(selector, &mode);
            system("cls");
            printf("╭──────────────────────Welcome To The BBD──────────────────────╮\n");
            printf("Type 'help' for a list of commands.\n");
            if (currentSelection != -1) {
                printf("Current mode: %s\n", selects->selects[currentSelection].name);
            }

            if (currentSelection == 0) {
				Table_debugPrint(table);
			}
			else if (currentSelection == 1) {
			}
			else if (currentSelection == 2) {
                Index *index = table->attributes[0].index;
                if (index)
				    Index_debugPrint(index, 0, index->rootPtr);
			}
			else if (currentSelection == 3) {
				Index *index = table->attributes[1].index;
				if (index)
				    Index_debugPrint(index, 0, index->rootPtr);
			}
			else if (currentSelection == 4) {
                Index *index = table->attributes[2].index;
                if (index)
				    Index_debugPrint(index, 0, index->rootPtr);
            }

            mode = MODE_WRITE;
        }
    }

    free(commandsStruct);
    free(selects->selects);
    free(selects);
    free(selector);

    return 0;
}
#endif