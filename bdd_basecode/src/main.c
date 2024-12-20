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
int main(void) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitle("BBD - Base de Données");
    
    Table* table = Table_createFromCSV("../data/intro_1/psittamulgiformes.csv", "../data/intro_1");
    if (!table) {
        ui_displayError("Impossible de créer la table depuis le fichier CSV");
        return 1;
    }

    Commands* cmds = commands_create_list();
    if (!cmds) {
        ui_displayError("Impossible de créer les commandes");
        Table_destroy(table);
        return 1;
    }

    Command* hello = command_create("hello", "Dire bonjour !", "aucuns arguments", 0, 0);
    commands_add(cmds, hello);
    
    Command *help = command_create("help", "Affiche la liste des commandes", "aucuns arguments", 0, 0);
    commands_add(cmds, help);
    
    Command *exit = command_create("exit", "Quitter le programme", "aucuns arguments", 0, 0);
    commands_add(cmds, exit);
    
    Command *print = command_create("print", "Afficher la table", "aucuns arguments", 0, 0);
    commands_add(cmds, print);
    
    Command *insert = command_create("insert", "Insérer une entrée dans la table", "insert <attribut1> <attribut2> <attribut3> ...", 3, 0);
    commands_add(cmds, insert);
    
    Command *search = command_create("search", "Rechercher des entrées dans la table", "search <attribut> <opérateur> <valeur> [valeur2]", 3, 1);
    commands_add(cmds, search);
    
    Command *delete = command_create("delete", "Supprimer une entrée de la table", "delete <index>", 1, 0);
    commands_add(cmds, delete);
    
    Command *clear = command_create("clear", "Effacer l'écran", "aucuns arguments", 0, 0);
    commands_add(cmds, clear);

    Command *show = command_create("show", "Affiche les tables", "aucuns arguments", 0, 0);
    commands_add(cmds, show);

    const char* selectNames[] = {
        "Table",
        "Data",
        "Index 0",
        "Index 1",
        "Index 2"
    };
    Selects* selects = selects_create(selectNames, sizeof(selectNames) / sizeof(char*));
    if (!selects) {
        ui_displayError("Impossible de créer les sélections");
        commands_destroy(cmds);
        Table_destroy(table);
        return 1;
    }

    Selector* selector = selector_create(selects);
    if (!selector) {
        ui_displayError("Impossible de créer le sélecteur");
        selects_destroy(selects);
        commands_destroy(cmds);
        Table_destroy(table);
        return 1;
    }

    char input[MAX_INPUT_SIZE];
    Mode mode = MODE_WRITE;
    bool running = true;

    ui_displayWelcome();

    while (running) {
        if (mode == MODE_WRITE) {
            printf("> ");

            if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
                break;
            }
            input[strcspn(input, "\n")] = 0;

            int argc = 0;
            char** argv = args_separation(input, &argc);
            if (!argv || argc == 0) {
                continue;
            }

            char* command = argv[0];
            char** cmdArgs = argv[1];
            argc--;

            if (!handle_command(table, command, cmdArgs, argc, cmds, &mode)) {
                running = false;
            }

            args_destroy(argv, argc + 1);
        }
        else if (mode == MODE_SELECT) {
            int selection = selector_handleInput(selector, &mode);
            if (selection >= 0) {
                ui_clearScreen();
                ui_displayWelcome();
                printf("Mode actuel : %s\n", selects->selects[selection].name);

                switch (selection) {
                case 0:
                    cmd_print(table);
                    break;
                case 2:
                case 3:
                case 4:
                    cmd_print_index(table, selection - 2);
                    break;
                }
            }
            mode = MODE_WRITE;
        }
    }

    selector_destroy(selector);
    selects_destroy(selects);
    commands_destroy(cmds);
    Table_destroy(table);

    return 0;
}
#endif