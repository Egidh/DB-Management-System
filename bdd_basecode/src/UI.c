#include "UI.h"

void printf_utf8(const wchar_t* message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;

    WriteConsoleW(hConsole, message, wcslen(message), &written, NULL);
}


int Table_findAttribute(Table* table, const char* attributeName) {
    for (int i = 0; i < table->attributeCount; i++) {
        if (strcmp(table->attributes[i].name, attributeName) == 0) {
            return i;
        }
    }

    return -1;
}

void ui_displayColoredText(const char* text, Color color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
	printf("%s", text);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ui_displayWelcome(void) {
    printf_utf8(L"──────────────────────Bienvenue Dans l'iBDD (ibijauBDD)──────────────────────\n");
    printf("Tapez 'help' pour la liste des commandes.\n\n");
}

void ui_clearScreen(void) {
    system("cls");
}

void ui_displayError(const char* message) {
    char buffer[256];
    sprintf(buffer, "Erreur : %s\n", message);
    ui_displayColoredText(buffer, COLOR_RED);
}

void ui_displaySuccess(const char* message) {
    char buffer[256];
    sprintf(buffer, "Succès : %s\n", message);
	ui_displayColoredText(buffer, COLOR_GREEN);
}

void ui_displayCommandNotFound(void) {
    char buffer[256];
    sprintf(buffer, "Commande non trouvée.\n");
	ui_displayColoredText(buffer, COLOR_BLUE);
}

void ui_displayArguments(Command* cmd) {
    char buffer[256];
    sprintf(buffer, "La commande requiert %d arguments.\n", cmd->argCount);
    ui_displayColoredText(buffer, COLOR_YELLOW);
}

void ui_displayExample(Command* cmd) {
	char buffer[256];
	sprintf(buffer, "Exemple : %s\n", cmd->example);
	ui_displayColoredText(buffer, COLOR_BLUE);
}

void ui_displayTable(char* title, char** header, int headerSize, char*** content, int contentSize) {  
   printf("Table : %s\n", title);  

   char **rows = calloc(contentSize + 1, sizeof(char*));  
   if (!rows) {  
       ui_displayError("Erreur d'allocation de mémoire");  
       return;  
   }  

   int *maxRowSize = calloc(headerSize, sizeof(int));  

   for (int i = 0; i < headerSize; i++) {  
       rows[0] = header[i];  
       for (int j = 0; j < contentSize; j++) {  
           rows[j + 1] = content[j][i];  
       }  

       maxRowSize[i] = max_length_of_a_column_in_the_table_for_the_display_table_function(rows, contentSize);
   }  

   for (int i = 0; i < headerSize; i++) {  
       printf("+");  
       for (int j = 0; j < maxRowSize[i] + 2; j++) {  
           printf("-");  
       }  
   }  

   printf("+\n");  

   for (int i = 0; i < headerSize; i++) {  
       printf("| %-*s ", maxRowSize[i], header[i]);  
   }  

   printf("|\n");  

   for (int i = 0; i < headerSize; i++) {  
       printf("|");  
       for (int j = 0; j < maxRowSize[i] + 2; j++) {  
           printf("-");  
       }  
   }  

   printf("|\n");  

   for (int i = 0; i < contentSize; i++) {  
       for (int j = 0; j < headerSize; j++) {  
           printf("| %-*s ", maxRowSize[j], content[i][j]);  
       }  
       printf("|\n");  
   }  

   for (int i = 0; i < headerSize; i++) {  
       printf("+");  
       for (int j = 0; j < maxRowSize[i] + 2; j++) {  
           printf("-");  
       }  
   }  

   printf("+\n");  
}  

int max_length_of_a_column_in_the_table_for_the_display_table_function(char** column, int columnSize) {  
   int max = 0;  
   for (int i = 0; i < columnSize; i++) {  
       if (strlen(column[i]) > max) {  
           max = strlen(column[i]);  
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
    ui_displayWelcome();
    printf_utf8(L"Utilisez les flèches ↑↓ pour naviguer, Entrée pour sélectionner, Échap pour revenir.\n\n");
    printf_utf8(L"Sélectionnez une option :\n");

    for (int i = 0; i < selects->selectCount; i++) {
        if (i == currentSelection) {
            char buffer[256];
            sprintf(buffer, " > %s\n", selects->selects[i].name);
            ui_displayColoredText(buffer, COLOR_BLACK_BACKGROUND_WHITE);
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

    for (int i = 0; i < argc; i++) {
        size_t argLength = strlen(args[i]);
		if (argLength > table->attributes[i].size) {
			printf("Erreur : %s trop long, sa taille maximale est de %d\n", args[i], table->attributes[i].size);
			return;
		}
	}

    Entry *newEntry = Entry_create(table);
    for (int i = 0; i < table->attributeCount; i++)
         strcpy(newEntry->values[i], args[i]);
    
    Table_insertEntry(table, newEntry);
    
    Entry_destroy(newEntry);

    printf("Entree inseree avec succes\n");
}

Combination get_operator(char* arg)
{
    if (strcmp(arg,"OR") == 0) return OR;
    if (strcmp(arg, "AND") == 0) return AND;
    if (strcmp(arg, "WITHOUT") == 0) return WITHOUT;
    else return BAD_OPERATION;
}

int verif_filter(char** args, int argc, Filter* filter, int simple)
{
    if (filter->attributeIndex == -1)
    {
        printf("\nErreur: %s Attribut invalide\n", args[0]);
        return 1;
    }

    if (filter->requestOp == -1)
    {
        printf("\nErreur : %s operation non reconnue\n", args[1]);
        return 1;
    }

    if (filter->requestOp == OP_BETW && simple == 1) 
    {
        if (argc >= 4)
            filter->key2 = args[3];
        else
        {
            printf("\nArgument manquant pour BETW !\nAttribut BETW key_1 key_2\n");
            return 1;
        }
    }
    if (filter->requestOp == OP_BETW && simple == 0)
    {
        if (argc == 8)
            filter->key2 = args[7];
        else if (argc >= 9)
            filter->key2 = args[8];
        else
        {
            printf("\nArgument manquant pour BETW !\nAttribut BETW key_1 key_2\n");
            return 1;
        }
    }
    return 0;
}

RequestOp str_to_op(const char* op) {
    if (strcmp(op, "LT") == 0) return OP_LT;
    if (strcmp(op, "LE") == 0) return OP_LEQ;
    if (strcmp(op, "EQ") == 0) return OP_EQ;
    if (strcmp(op, "GE") == 0) return OP_GEQ;
    if (strcmp(op, "GT") == 0) return OP_GT;
    if (strcmp(op, "BETW") == 0) return OP_BETW;
    return -1;
}

void cmd_search(Table* table, char** args, int argc, const Commands* commands) {
    if (!table || !args || !commands) {
        ui_displayError("Arguments invalides pour la recherche");
        return;
    }

    int mode = 0;

    Filter filter = { Table_findAttribute(table, args[0]), str_to_op(args[1]), args[2], 0};
    Filter secondaryFilter;

    int verif = verif_filter(args, argc, &filter, 1);
    if (verif)

        return;
    Combination comb;

    if (filter.requestOp != OP_BETW && argc > 3)
    {
        if (argc >= 7)
        {
            mode = 1;
            comb = get_operator(args[3]);
            if (comb == BAD_OPERATION)
            {
                printf("Operateur non valable %s.\n", args[3]);
                return;
            }
            secondaryFilter.attributeIndex = Table_findAttribute(table, args[4]);
            secondaryFilter.requestOp = str_to_op(args[5]);
            secondaryFilter.key1 = args[6];
            int check = verif_filter(args, argc, &secondaryFilter, 0);
            if (check) return;
        }
        else
        {
            printf("Erreur : Saisie invalide\n");
            return;
        }
    }
    else if (filter.requestOp == OP_BETW && argc > 4)
    {
        if (argc >= 8)
        {
            mode = 1;
            comb = get_operator(args[4]);
            if (comb == BAD_OPERATION)
            {
                printf("Operateur non valable %s\n", args[3]);
                return;
            }
            secondaryFilter.attributeIndex = Table_findAttribute(table, args[5]);
            secondaryFilter.requestOp = str_to_op(args[6]);
            secondaryFilter.key1 = args[7];
            int check = verif_filter(args, argc, &secondaryFilter, 0);
            if (check) return;
        }
        else
        {
            printf("Erreur : Saisie invalide\n");

            return;
        }
    }
    verif_filter(args, argc, &secondaryFilter, 0);

    printf("\nRecherche dans la table...\n\n");

    SetEntry* result = SetEntry_create();

    switch (mode)
    {
    case 0:
        Table_search(table, &filter, result);
        break;
    case 1:
        Table_combinationSearch(table, &filter, &secondaryFilter, comb, result);
        break;
    default:
        break;
    }


    printf("%d entree trouvees :\n\n", SetEntry_size(result));

    char **header = calloc(table->attributeCount, sizeof(char*));
    for (int i = 0; i < table->attributeCount; i++)
	{
		header[i] = table->attributes[i].name;
	}

    char ***entries = calloc(SetEntry_size(result), sizeof(char**));
    SetEntryIter *iter = SetEntryIter_create(result);

	for (int i = 0; i < SetEntry_size(result); i++)
	{
		entries[i] = calloc(table->attributeCount, sizeof(char*));
		Entry *entry = Entry_create(table);
		Table_readEntry(table, entry, SetEntryIter_getValue(iter));
		for (int j = 0; j < table->attributeCount; j++)
		{
			entries[i][j] = entry->values[j];
		}
		SetEntryIter_next(iter);
	}

    ui_displayTable("Resultat de la recherche", header, table->attributeCount, entries, SetEntry_size(result));

    SetEntry_destroy(result);
    SetEntryIter_destroy(iter);
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
        char buffer[256];
        sprintf(buffer, "Attribut invalide : %s.\n", attribute);
        ui_displayError(buffer);
        return;
    }

    Filter removeFilter = { idx, OP_EQ, value, "" };
    SetEntry* results = SetEntry_create();
    Table_search(table, &removeFilter, results);

    int entryRemovedCount = 0;
    SetEntryIter *resultsIter = SetEntryIter_create(results);
    
    if (!results->root)
    {
        char buffer[256];
        sprintf(buffer, "Pas d'entree correspondante : %s", value);
		ui_displayError(buffer);
        return;
    }

    while (SetEntryIter_isValid(resultsIter))
    {
        Table_removeEntry(table, resultsIter->curr->data);
        entryRemovedCount++;
        SetEntryIter_next(resultsIter);
    }

    SetEntry_destroy(results);
    SetEntryIter_destroy(resultsIter);

    char buffer[256];
    sprintf(buffer, "%d entree(s) supprimee(s) avec succes.\n", entryRemovedCount);
	ui_displaySuccess(buffer);
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

void cmd_displayIbijau(void) {
    printf_utf8(L"███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████\n"
                L"███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████\n"
                L"███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████\n"
                L"███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████\n"
                L"██████████████████████████████████████████████████▓▓▒▓▓▓▓▓▓▒▒▒▒▓▓▓▓▒▒▒▒▒▒▓▓▓███████████████████████████████████████████████████\n"
                L"█████████████████████████████████████████████▓▓▒▓▒▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▒▓▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓████████████████████████████████████████████\n"
                L"█████████████████████████████████████████▓▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▒▓▓▓▓▓▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▓▓▓████████████████████████████████████\n"
                L"██████████████████████████████████████▓▓▓▓▒▒▒▒▓▒▒▓▓▓▒▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▒▒▒▓▒▓▓▓███████████████████████████████████\n"
                L"████████████████████████████████████▓▓▓█▓▒▒▒▓▓▓▓▓▓▓▓▓▒▒▓▓▓▒▓▓▒▓▓▒▓█▓▒▒▒▒▒▒▒▒▓▓▓▓▒▓▓▓▓▒▒▓▒▓▓▓▓██████████████████████████████████\n"
                L"█████████████████████████████████████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒░▒▒▒▓▓▓▒▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓█▓▓▓▓▓▓██████████████████████████████████\n"
                L"███████████████████████████████████▓▓█▓▓▓▓▓█▓▒▒▒▒▓▒▒▒▒▒▒▒▓▓▒▓▒▓▒▓▒▒▒▒▒▒▓▒▒▒▓▒▒▒▒▒▓▓▓████▓▓▓▓▓▓█████████████████████████████████\n"
                L"█████████████████████████████████▓▓▓▓▓▒▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▒▒▒▓▓▓▓▓▓▓▓▓▒▒▓▓▒▒▒▒▒▒▓▓▒▓▓▓█▓███████████████████████████████\n"
                L"████████████████████████████████████▓▓▓▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▓▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓███████████████████████████████\n"
                L"██████████████████████████████████▓▓▓▓▓▓▓▒█████▒▒▒▒▒▒▒▓▓▓▓▓█▓▓▓█▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒████▓▓▓▓▓▓███████████████████████████████\n"
                L"█████████████████████████████████▓▓▓▓▓▓▓▓▓█████▓▒▒▒▒▒▓▓▓▓▓▓▓▓▓█▓███████████▓▓▓▓▒▒▒▒░▒▓█████▓▓▓▓▓███████████████████████████████\n"
                L"████████████████████████████████▓██▓▓▓▓▓▓▒▓███▓▒▒▒▒▒▒▓▓▓▓▓██▓▓▓▓█▓██▓███████▓▓▓▒▒▒▒▒▒▓█████▓▓▓▓▓███████████████████████████████\n"
                L"███████████████████████████████████▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓▓▓██▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓██▓▓▓▒▒▒▒▒▒▓███▓▓▓▓▓████████████████████████████████\n"
                L"███████████████████████████████████▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▓▓█▓█████████████████████████████████\n"
                L"██████████████████████████████████▓▓▓▓▓▓▒▓▓▓▓▒▒▒▒▓▓▓▓▒▓▓▓▓▓▓▓▓▒▒▒▓▓▓▓▓▒▒▒▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▓▓▓▓▓▓▓███████████████████████████████\n"
                L"███████████████████████████████▓▓▓▓▓▓▓▓▓▓▒▒▓▓▓▓▓▓▒▒▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▓▓▓▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█████████████████████████████████\n"
                L"█████████████████████████████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▓▓▓▓▓▓▓▓██████████████████████████████████\n"
                L"█████████████████████████████▓▓▓▒▒▒▓▓▓▓▓▓▓▒▒▒▒▓███▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██████████████████████████████\n"
                L"████████████████████████████████▓▓▓▓▓▓▒▓▒▓▓▓████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████▓▓▓▓▓▓▓▓▓▓▓█████████████████████████████\n"
                L"████████████████████████████████▓▓▓▓██▓▓███████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█████▓▓▓▓▓▒▒▓▓▓█████████████████████████████\n"
                L"███████████████████████████████▓▓▓▓██████████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██████████▓▓▓██████████████████████████████\n"
                L"███████████████████████████████▓▓▓▓███████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████████▓█████████████████████████████████\n"
                L"██████████████████████████████▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▓▒▒▓▓▓▓▓▓▓▒▓▓▓█▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████▓▓████████████████████████████████\n"
                L"██████████████████████████████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████████████████████████████████████\n"
                L"██████████████████████████████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓███████████████████████████████████\n"
                L"██████████████████████████████▓▓▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓█▓██▓▓▓▓██▓▓████████████████████████████████\n"
                L"██████████████████████████████▓▒▒▒▒▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▒▒▒▒▒▒▓▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█▓▓▓████████████████████████████████\n"
                L"██████████████████████████████▓▒▒▒▓▓▓▒▓▓▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▓▓▓▒▓███████████████████████████████\n"
                L"█████████████████████████████▓▒▒▒▒▓▓▓▓▓▓▒▓▒▒▒▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▒▓▓▒▓▒▒▒▒▒▒▓▓▓▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▓▓▒▓██████████████████████████████\n"
                L"█████████████████████████████▓▒▒▒▓▓▓▒▒▓▓▓▒▒▒▓▓▓▒▒▒▒▓▓▓▓▒▒▒▒▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▓█████████████████████████████\n"
                L"████████████████████████████▓▒▒▒▒▓▓▒▒▒▓▓▓▒▓▓▓▓▓▒▒▒▒▒▓▓▓▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▒▒▒▓▓▓▒▓▓▓▓▓▓▓▓▓▓▓▓▒▓█████████████████████████████\n"
                L"████████████████████████████▓▒▒▒▓▒▒▒▒▓▓▓▓▒▒▓▓▓▓▓▒▒▒▒▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓▒▒▒▒▒▒▒▒▓▓▒▒▒▒▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓█████████████████████████████\n"
                L"███████████████████████████▓▒▒▒▒▒▒▒▒▒▓▓▓▒▒▒▓▓▒▒▒▒▒▒▒▓▓▒▒▓▓▓▓▒▒▒▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▒▒▓▓▓▓▓▓▓▒▒▓▓▓▒▒▒▒▓█████████████████████████████\n"
                L"███████████████████████████▒▒▒▒▒▒▒▒▒▒▒▓▓▒▒▒▓▒▓▒▒▓▒▒▓▓▓▓▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▓▓▓▓▒▓▓▓▓▓▓▓▓▒▓▓▓▓▒▒▒▓▓███████████████████████████\n"
                L"██████████████████████████▒▒▒▒▒▒▒▒▒▒▒▒▓▒▒▒▓▒▒▓▓▓▓▓▒▓▓▓▓▓▓█▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▓▓▓▓▓▓▓▒▓▓▓▓▒▓▓▓▓▓▓▓▓▓███████████████████████████\n"
                L"██████████████████████████▓▓▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▓▒▓▓▒▒▓▓▒▒▓▓▓▓▓▓▒▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▓▓▓██████████████████████████\n"
                L"█████████████████████████▓▓▓▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▓▓▓▓▒▓▓▓▓▓▓▓▓▒▓▓▓▒▒▒▒▒▒▒▒▓▓▒▓▓▓▓▓▒▓▓▓▓▓▓▒▒▒▓▓██████████████████████████\n"
                L"██████████████████████████▓▒▒▒▒▒▒▒▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▓▓▒▒▓▓▓▓▓▓▓▓▓▓█▓▓▓▓▒▒▒▒▒▓▓▒▒▓▓▒▒▒▒▒▓▓▓▓▓▒▒▓▓██████████████████████████\n"
                L"█████████████████████████▓▒▒▒▒▒▒▒▒▒▓▓▓▒▒▒▒▒▒▒▒▒▓▓▒▓▒▒▒▒▒▒▓▓▒▒▒▓▓▓▓▓▓▓▓▓█▓▓▒▓▒▒▒▒▒▒▒▓▒▒▓▒▒▒▒▒▓▓▒▒▒▓▓▓▓▓█████████████████████████\n"
                L"████████████████████████▓▓▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▓▓▓▒▒▒▒▓▓▓▒▓▓▓▓▓▓▒▓▒▒▒▒▒▒▒▓▓▓▒▒▓▒▒▒▒▒▓▒▒▒▒▒▓▓▓█████████████████████████\n"
                L"███████████████████████▓▓▒▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▓▒▒▒▒▒▓▒▓▓▓▒▒▒▒▒▒▒▓▒▓▓▓▓▓▒▒▒▓▓▒▒▒▒▓▓▓▓▒▒▒▒▓▓▓▒▒▓▓▒▒▒▓▓█████████████████████████\n");
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

   Entry_destroy(entry);
}

void cmd_selectTable(Table* table, char** args, int argc, const Commands* commands) {
    if (!table || !args || !commands) {
        ui_displayError("Arguments invalides pour la sélection de table");
        return;
    }

    if (strcmp(args[0], "*") == 0) {
        printf("Sélection de toutes les tables...\n");
        cmd_show(table);
        return;
    }

    int* indexTab = calloc(argc, sizeof(int));
    if (!indexTab) {
        ui_displayError("Erreur d'allocation de mémoire");
        return;
    }

    for (int i = 0; i < argc; i++) {
        indexTab[i] = Table_findAttribute(table, args[i]);
        if (indexTab[i] == -1) {
            free(indexTab);
            ui_displayError("L'attribut n'existe pas");
            return;
        }
    }

    char*** content = calloc(table->entryCount, sizeof(char**));
    if (!content) {
        free(indexTab);
        ui_displayError("Erreur d'allocation de mémoire");
        return;
    }

    for (int i = 0; i < table->entryCount; i++) {
        content[i] = calloc(argc, sizeof(char*));
        if (!content[i]) {
            for (int j = 0; j < i; j++) {
                free(content[j]);
            }
            free(content);
            free(indexTab);
            ui_displayError("Erreur d'allocation de mémoire");
            return;
        }
    }

    char** header = calloc(argc, sizeof(char*));
    if (!header) {
        for (int i = 0; i < table->entryCount; i++) {
            free(content[i]);
        }
        free(content);
        free(indexTab);
        ui_displayError("Erreur d'allocation de mémoire");
        return;
    }

    Entry* entry = Entry_create(table);
    if (!entry) {
        for (int i = 0; i < table->entryCount; i++) {
            free(content[i]);
        }
        free(content);
        free(header);
        free(indexTab);
        ui_displayError("Erreur de création d'entrée");
        return;
    }

    for (int i = 0; i < table->entryCount; i++) {
        EntryPointer entryPtr = i * table->entrySize;
        Table_readEntry(table, entry, entryPtr);

        for (int k = 0; k < argc; k++) {
            content[i][k] = strdup(entry->values[indexTab[k]]);
        }
    }

    for (int k = 0; k < argc; k++) {
        header[k] = table->attributes[indexTab[k]].name;
    }

    ui_displayTable(table->name, header, argc, content, table->entryCount);

    for (int i = 0; i < table->entryCount; i++) {
        for (int j = 0; j < argc; j++) {
            free(content[i][j]);
        }
        free(content[i]);
    }
    free(content);
    free(header);
    free(indexTab);
    Entry_destroy(entry);
}


void cmd_sort(Table* table, char** args, int argc, const Commands* commands) {
	if (!table || !args || !commands) {
		ui_displayError("Arguments invalides pour la sélection de table");
		return;
	}

	int id = Table_findAttribute(table, args[0]);
	if (id == -1) {
		ui_displayError("L'attribut n'existe pas");
		return;
	}

	Index* index = table->attributes[id].index;
	if (!index) {
		ui_displayError("L'index n'existe pas");
		return;
	}

    for (int i = 0; i < table->attributeCount; i++) {
        printf("+-----------------");
    }
    printf("+\n");

    for (int i = 0; i < table->attributeCount; i++) {
		printf("| %-15s ", table->attributes[i].name);
	}

	printf("|\n");

    for (int i = 0; i < table->attributeCount; i++) {
        printf("+-----------------");
    }
    printf("+\n");

    if (strcmp(args[1], "asc") == 0) {
		Index_sort(index, index->rootPtr, 1);
	} else if (strcmp(args[1], "desc") == 0) {
		Index_sort(index, index->rootPtr, 0);
	} else {
        char *buffer = calloc(100, sizeof(char));
        sprintf(buffer, "Argument %s invalide", args[1]);
		ui_displayError(buffer);
        free(buffer);
		return;
	}

	for (int i = 0; i < table->attributeCount; i++) {
		printf("+-----------------");
	}
	printf("+\n");

	printf("Tri de l'index de l'attribut %s effectué\n", args[0]);
}

void cmd_export(Table* table, char** args, int argc, const Commands* commands) {
    FILE* file = fopen(args[0], "w");
    if (!file) {
        ui_displayError("Impossible de créer le fichier d'export");
        return;
    }

    fprintf(file, "%s;%d\n", table->name, table->attributeCount);
    printf("%s;%d\n", table->name, table->attributeCount);

    for (int i = 0; i < table->attributeCount; i++) {
        fprintf(file, "%s;%lu;%lu\n", table->attributes[i].name, table->attributes[i].size, table->attributes[i].index ? 1 : 0);
        printf("%s;%lu;%lu\n", table->attributes[i].name, table->attributes[i].size, table->attributes[i].index ? 1 : 0);
    }

    fprintf(file, "%d;\n", table->entryCount);
    printf("%d;\n", table->entryCount);

    Entry* entry = Entry_create(table);
    for (int i = 0; i < table->entryCount; i++) {
        EntryPointer entryPtr = i * table->entrySize;
        Table_readEntry(table, entry, entryPtr);

        if (entry->nextFreePtr == VALID_ENTRY) {
            for (int j = 0; j < table->attributeCount; j++) {
                fprintf(file, "%s%s", entry->values[j], j < table->attributeCount - 1 ? ";" : "\n");
                printf("%s%s", entry->values[j], j < table->attributeCount - 1 ? ";" : "\n");
            }
        }
    }

    Entry_destroy(entry);
    fclose(file);
    ui_displaySuccess("Export terminé avec succès");
}

void cmd_count(Table* table, char** args, int argc, const Commands* commands) {
    if (!table) {
        ui_displayError("Table invalide");
        return;
    }

    if (argc == 0) {
        printf("Nombre total d'entrées : %d\n", table->validEntryCount);
        return;
    }

    if (argc < 2) {
        ui_displayError("Usage: count <colonne> <valeur>");
        return;
    }

    int attributeIndex = Table_findAttribute(table, args[0]);
    if (attributeIndex == -1) {
        ui_displayError("Colonne invalide");
        return;
    }

    Filter filter = { attributeIndex, OP_EQ, args[1], NULL };
    SetEntry* results = SetEntry_create();
    Table_search(table, &filter, results);

    printf("Nombre d'entrees ou %s = %s : %d\n", args[0], args[1], SetEntry_size(results));

    SetEntry_destroy(results);
}

void cmd_stats(Table* table, char** args, int argc, const Commands* commands) {
    if (!table) {
        ui_displayError("Table invalide");
        return;
    }

    printf("Statistiques de la table %s :\n", table->name);
    printf("Nombre total d'entrées : %d\n", table->validEntryCount);
    printf("Nombre total d'attributs : %d\n", table->attributeCount);
    printf("Taille d'une entrée : %d octets\n", table->entrySize);
    printf("Indices créés :\n");

    for (int i = 0; i < table->attributeCount; i++) {
        if (table->attributes[i].index) {
            printf("- %s\n", table->attributes[i].name);
        }
    }
}

void cmd_structure(Table* table) {
    if (!table) {
        ui_displayError("Table invalide");
        return;
    }

    printf("Structure de la table %s :\n\n", table->name);
    printf("%-20s %s\n", "Attribut", "Indexé");
    printf("----------------------------------------\n");

    for (int i = 0; i < table->attributeCount; i++) {
        printf("%-20s %s\n",
            table->attributes[i].name,
            table->attributes[i].index ? "Oui" : "Non");
    }
}

void create_configFile(char* folderPath, char* csvPath) {
    if (!folderPath || !csvPath) {
        ui_displayError("Chemins invalides");
        return;
    }

    FILE* file = fopen("config.cfg", "w");
    if (!file) {
        ui_displayError("Impossible de créer le fichier de configuration");
        return;
    }

    fprintf(file, "folderPath=%s\n", folderPath);
    fprintf(file, "csvPath=%s\n", csvPath);
    fclose(file);
}

void load_configFile(char* folderPath, char* csvPath) {
    FILE* file = fopen("config.cfg", "r");
    if (!file) {
        ui_displayError("Impossible de charger le fichier de configuration");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        char* key = strtok(line, "=");
        char* value = strtok(NULL, "\n");
        if (key && value) {
            if (strcmp(key, "folderPath") == 0) {
                strcpy(folderPath, value);
            }
            else if (strcmp(key, "csvPath") == 0) {
                strcpy(csvPath, value);
            }
        }
    }
    fclose(file);
}

void search_configFile(char* folderPath, char* csvPath) {
    const char* DEFAULT_FOLDER = "../data/";
    const char* DEFAULT_CSV = "psittamulgiformes.csv";
    FILE* file = fopen("config.cfg", "r");

    if (!file) {
        char input[256];

        printf("Chemin du dossier\n");
        printf("Appuyez sur Entrée pour utiliser le dossier par défaut [%s]: \n", DEFAULT_FOLDER);
        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = 0;

            if (strlen(input) == 0) {
                strcpy(folderPath, DEFAULT_FOLDER);
            }
            else {
                strcpy(folderPath, input);
            }
        }

        printf("Chemin du fichier CSV\n");
        printf("Appuyez sur Entrée pour utiliser le fichier par défaut [%s]: \n", DEFAULT_CSV);

        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = 0;

            if (strlen(input) == 0) {
                char buffer[256];
                sprintf(buffer, "%s%s", folderPath, DEFAULT_CSV);

                strcpy(csvPath, buffer);
            }
            else {
                char buffer[256];
                sprintf(buffer, "%s%s", folderPath, input);

                strcpy(csvPath, buffer);
            }
        }


        create_configFile(folderPath, csvPath);
        printf("Configuration enregistrée avec succès.\n");
    }
    else {
        fclose(file);
        load_configFile(folderPath, csvPath);
    }
}

//void cmd_modify(Table* table, char** args, int argc)
//{
//    if (!table || !args)
//    {
//        printf("Erreur lors de la modification de l'entree\n");
//        return;
//    }
//
//    Entry* oldEntry = Entry_create(table);
//    Entry* newEntry = Entry_create(table);
//
//    for (int i = 0; i < table->attributeCount; i++)
//    {
//        strcpy(oldEntry->values[i], args[i]);
//        strcpy(newEntry->values[i], args[i + table->attributeCount]);
//    }
// 
//
//
//    Table_modifyEntry(table, newEntry, entryPtr);
//}