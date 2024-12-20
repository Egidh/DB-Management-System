#include "UI.h"

void printf_utf8(const wchar_t* message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;

    WriteConsoleW(hConsole, message, wcslen(message), &written, NULL);
}

void ui_displayWelcome(void) {
    SetConsoleOutputCP(CP_UTF8);
    printf("──────────────────────Welcome To The BBD──────────────────────\n");
    printf("Tapez 'help' pour la liste des commandes.\n\n");
}

void ui_clearScreen(void) {
    system("cls");
}

void ui_displayError(const char* message) {
    fprintf(stderr, "Erreur : %s\n", message);
}

void ui_displaySuccess(const char* message) {
    printf("Succès : %s\n", message);
}

void ui_displayCommandNotFound(void) {
    printf("Commande non trouvée. Tapez 'help' pour la liste des commandes.\n");
}

void ui_displayArguments(Command* cmd) {
    printf("La commande requiert %d arguments.\n", cmd->argCount);
}

void ui_displayTable(char* title, char** header, int headerSize, char*** content, int contentSize) {
    printf("\tTable : %s\n", title);

    int maxRowSize = max_lenght_of_a_collumn_in_the_table_for_the_display_table_function(header, headerSize);

    for (int i = 0; i < headerSize; i++) {
        printf("+------------");
    }

    printf("+\n");

    for (int i = 0; i < headerSize; i++) {
		printf("| %-10s ", header[i]);
	}

    printf("|\n");
    
    for (int i = 0; i < headerSize; i++) {
		printf("|------------");
	}

    printf("|\n");

    for (int i = 0; i < contentSize; i++) {
		for (int j = 0; j < headerSize; j++) {
            printf("| %-10s ", content[i][j]);
		}
		printf("|\n");
    }

    for (int i = 0; i < headerSize; i++) {
        printf("+------------");
    }

    printf("+\n");
}

int max_lenght_of_a_collumn_in_the_table_for_the_display_table_function(char** collumn, int collumnSize) {
	int max = 0;
	for (int i = 0; i < collumnSize; i++) {
		if (strlen(collumn[i]) > max) {
			max = strlen(collumn[i]);
		}
	}
	return max;
}

char** args_separation(const char* input, int* argc) {
    if (!input || !argc) return NULL;

    *argc = 0;
    int capacity = INITIAL_ARGS_SIZE;
    char** args = calloc(capacity, sizeof(char*));
    if (!args) return NULL;

    char* inputCopy = strdup(input);
    if (!inputCopy) {
        free(args);
        return NULL;
    }

    char* token = strtok(inputCopy, " ");
    while (token) {
        if (*argc >= capacity) {
            capacity *= 2;
            char** newArgs = realloc(args, capacity * sizeof(char*));
            if (!newArgs) {
                args_destroy(args, *argc);
                free(inputCopy);
                return NULL;
            }
            args = newArgs;
        }

        args[*argc] = strdup(token);
        if (!args[*argc]) {
            args_destroy(args, *argc);
            free(inputCopy);
            return NULL;
        }

        (*argc)++;
        token = strtok(NULL, " ");
    }

    free(inputCopy);
    return args;
}

void args_destroy(char** args, int argc) {
    if (!args) return;

    for (int i = 0; i < argc; i++) {
        free(args[i]);
    }
    free(args);
}

void args_display(char** args, int argc) {
    printf("Arguments (%d):\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("[%d] : '%s'\n", i, args[i]);
    }
}

Selects* selects_create(const char** names, int count) {
    if (!names || count <= 0) return NULL;

    Selects* selects = calloc(1, sizeof(Selects));
    if (!selects) return NULL;

    selects->selects = calloc(count, sizeof(Select));
    if (!selects->selects) {
        free(selects);
        return NULL;
    }

    selects->selectCount = count;
    for (int i = 0; i < count; i++) {
        selects->selects[i].name = strdup(names[i]);
        if (!selects->selects[i].name) {
            selects_destroy(selects);
            return NULL;
        }
    }

    return selects;
}

void selects_destroy(Selects* selects) {
    if (!selects) return;

    if (selects->selects) {
        for (int i = 0; i < selects->selectCount; i++) {
            free(selects->selects[i].name);
        }
        free(selects->selects);
    }
    free(selects);
}

void selects_displayMenu(const Selects* selects, int currentSelection) {
    ui_clearScreen();
    printf("Utilisez les flèches ↑↓ pour naviguer, Entrée pour sélectionner, Échap pour revenir.\n\n");
    printf("Sélectionnez une option :\n");

    for (int i = 0; i < selects->selectCount; i++) {
        if (i == currentSelection) {
            printf("-> %s\n", selects->selects[i].name);
        }
        else {
            printf("  %s\n", selects->selects[i].name);
        }
    }
}

Selector* selector_create(Selects* selects) {
    if (!selects) return NULL;

    Selector* selector = calloc(1, sizeof(Selector));
    if (!selector) return NULL;

    selector->selections = selects;
    selector->numberOfSelects = selects->selectCount;

    return selector;
}

void selector_destroy(Selector* selector) {
    free(selector);
}

int selector_handleInput(Selector* selector, Mode* mode) {
    if (!selector || !mode) return -1;

    int currentSelection = 0;
    selects_displayMenu(selector->selections, currentSelection);

    while (1) {
        int key = _getch();

        if (key == 224) {  
            key = _getch();
            switch (key) {
            case SCANCODE_UP:
                currentSelection = (currentSelection - 1 + selector->numberOfSelects)
                    % selector->numberOfSelects;
                break;

            case SCANCODE_DOWN:
                currentSelection = (currentSelection + 1) % selector->numberOfSelects;
                break;
            }
            selects_displayMenu(selector->selections, currentSelection);
        }
        else if (key == KEY_ENTER) {
            return currentSelection;
        }
        else if (key == KEY_ESC) {
            *mode = MODE_WRITE;
            return -1;
        }
    }
}

void cmd_hello(void) {
    printf("Bonjour !\n");
}

void cmd_exit(void) {
    printf("Au revoir !\n");
    exit(0);
}

void cmd_insert(Table* table, char** args, int argc, const Commands* commands) {
    if (!table || !args || !commands) {
        ui_displayError("Arguments invalides pour l'insertion");
        return;
    }

    Entry *newEntry = Entry_create(table);
    for (int i = 0; i < table->attributeCount; i++)
         strcpy(newEntry->values[i], args[i]);
    
    Table_insertEntry(table, newEntry);
    
    Entry_destroy(newEntry);

    printf("Entree inseree avec succes\n");
}

void cmd_search(Table* table, char** args, int argc, const Commands* commands) {
    if (!table || !args || !commands) {
        ui_displayError("Arguments invalides pour la recherche");
        return;
    }
    printf("Recherche dans la table...\n");

    for (int i = 0; i < argc; i++) {
        printf("%s\n", args[i]);
    }

    printf("%d\n", str_to_op(args[1]));

    Filter filter = { atoi(args[0]), str_to_op(args[1]), args[2], 0};

    if (strcmp(args[1], "OP_BETW") == 0) {
        filter.key2 = args[3];
    }

    SetEntry* result = SetEntry_create();
    Table_search(table, &filter, result);

    printf("Result count = %d\n", SetEntry_size(result));
}

RequestOp str_to_op(const char* op) {
    if (strcmp(op, "OP_LT") == 0) return OP_LT;
    if (strcmp(op, "OP_LEQ") == 0) return OP_LEQ;
    if (strcmp(op, "OP_EQ") == 0) return OP_EQ;
    if (strcmp(op, "OP_GEQ") == 0) return OP_GEQ;
    if (strcmp(op, "OP_GT") == 0) return OP_GT;
    if (strcmp(op, "OP_BETW") == 0) return OP_BETW;
    return -1;
}

void cmd_delete(Table* table, char** args, int argc, const Commands* commands) {
    if (!table || !args || !commands) {
        ui_displayError("Arguments invalides pour la suppression");
        return;
    }

    char* attribute = (char*)calloc(MAX_NAME_SIZE, sizeof(char));
    assert(attribute);
    char* value = (char*)calloc(MAX_NAME_SIZE, sizeof(char));
    assert(value);

    strncpy(attribute, args[0], MAX_NAME_SIZE);
    strncpy(value, args[1], MAX_NAME_SIZE);
    int idx = -1;

    for (int i = 0; i < table->attributeCount; i++)
    {
        if (strcmp(table->attributes[i].name, attribute) == 0)
        {
            idx = i;
            break;
        }
    }

    if (idx == -1)
    {
        printf("Attribut invalide : %s.\n", attribute);
        return;
    }

    Filter removeFilter = { idx, OP_EQ, value, "" };
    SetEntry* results = SetEntry_create();
    Table_search(table, &removeFilter, results);

    int entryRemovedCount = 0;
    SetEntryIter *resultsIter = SetEntryIter_create(results);
    if (!results->root)
    {
        printf("Pas d'entree correspondante : %s", value);
    }
    while (SetEntryIter_isValid(resultsIter))
    {
        Table_removeEntry(table, resultsIter->curr->data);
        entryRemovedCount++;
        SetEntryIter_next(resultsIter);
    }

    SetEntry_destroy(results);
    SetEntryIter_destroy(resultsIter);

    printf("Suppression de %d elements reussie\n", entryRemovedCount);
}

void cmd_print(Table* table) {
    if (!table) {
        ui_displayError("Table invalide");
        return;
    }

    Table_debugPrint(table);
}

void cmd_print_index(Table* table, int indexNum) {
    if (!table || indexNum < 0 || indexNum >= table->attributeCount) {
        ui_displayError("Index invalide");
        return;
    }

    Index* index = table->attributes[indexNum].index;
    if (index) {
        Index_debugPrint(index, 0, index->rootPtr);
    }
    else {
        ui_displayError("L'index n'existe pas");
    }
}

void cmd_show(Table* table) {  
   if (!table) {  
       ui_displayError("Table invalide");  
       return;  
   }  

   char **header = calloc(table->attributeCount, sizeof(char*));  
   if (!header) {  
       ui_displayError("Erreur d'allocation de mémoire");  
       return;  
   }  

   for (int i = 0; i < table->attributeCount; i++) {  
       header[i] = table->attributes[i].name;  
   }  

   char ***content = calloc(table->validEntryCount, sizeof(char**));  
   if (!content) {  
       ui_displayError("Erreur d'allocation de mémoire");  
       return;  
   }  

   int length = table->validEntryCount;
   for (int i = 0; i < length; i++) {  
       content[i] = calloc(table->attributeCount, sizeof(char*));  
       if (!content[i]) {  
           ui_displayError("Erreur d'allocation de mémoire");  
           return;  
       }  
   }  

   length = table->validEntryCount;
   Entry* entry = Entry_create(table);
   for (int i = 0, k=0; i < length; i++) {
       EntryPointer entryPtr = i * table->entrySize;  
       Table_readEntry(table, entry, entryPtr);
       if (entry->nextFreePtr != VALID_ENTRY)
       {
           length++;
           continue;
       }
       for (int j = 0; j < table->attributeCount; j++) {  
           content[k][j] = strdup(entry->values[j]); 
       }  
       k++;
   }  

   ui_displayTable(table->name, header, table->attributeCount, content, table->validEntryCount);  

   for (int i = 0; i < table->validEntryCount; i++) {  
       for (int j = 0; j < table->attributeCount; j++) {  
           free(content[i][j]);  
       }  
       free(content[i]);  
   }  
   free(content);  
   free(header);  
}