/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN and Matthieu LE BERRE.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "Settings.h"
#include "Table.h"
#include "Index.h"
#include "UI.h"


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
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitle("BDD - Base de Données");

    ui_displayWelcome();

    char* folderPath = "../data/";
	char *csvPath = "../data/psittamulgiformes.csv";

    Table *table = NULL;

    search_configFile(folderPath, csvPath);
    printf("folderPath : %s\n", folderPath);
    printf("csvPath : %s\n", csvPath);

    char input[MAX_INPUT_SIZE];
    Mode mode = MODE_WRITE;
    bool running = true;

    char buffer[100];
    sprintf(buffer, "%s%s", folderPath, "/psittamulgiformes.tbl");
    FILE *tbl = fopen(buffer, "r");
    
    if (tbl == NULL) {
		table = Table_createFromCSV(csvPath, folderPath);
		if (!table) {
			ui_displayError("Impossible de créer la table");
			return 1;
		}
		ui_displaySuccess("Table créée avec succès");
		Table_destroy(table);
	}
	else {
        const char* selectLoadOrCreate[] = {
        "Load",
        "Create"
        };

        Selects* selectsLoadOrCreate = selects_create(selectLoadOrCreate, sizeof(selectLoadOrCreate) / sizeof(char*));
        if (!selectsLoadOrCreate) {
            ui_displayError("Impossible de créer les sélections");
            Table_destroy(table);
            return 1;
        }

        Selector* selectorLoadOrCreate = selector_create(selectsLoadOrCreate);
        if (!selectorLoadOrCreate) {
            ui_displayError("Impossible de créer le sélecteur");
            selects_destroy(selectsLoadOrCreate);
            Table_destroy(table);
            return 1;
        }

        while (true) {
            int selection = selector_handleInput(selectorLoadOrCreate, &mode);
            if (selection >= 0) {
                ui_clearScreen();
                ui_displayWelcome();

                switch (selection) {
                case 0:
                    table = Table_load("psittamulgiformes.tbl", folderPath);
                    break;
                case 1:
                    table = Table_createFromCSV(csvPath, folderPath);
                    break;
                }
                if (!table) {
                    ui_displayError("Impossible de charger la table");
                    break;
                }
                else {
                    ui_displaySuccess("Table chargée avec succès");
                    break;
                }
            }
        }
        selector_destroy(selectorLoadOrCreate);
        selects_destroy(selectsLoadOrCreate);
	}


    Commands* cmds = commands_create_list();
    if (!cmds) {
        ui_displayError("Impossible de créer les commandes");
        Table_destroy(table);
        return 1;
    }

    Command* hello = command_create("hello", "Dire bonjour !", "aucuns arguments", NULL, 0, 0);
    commands_add(cmds, hello);

    Command *help = command_create("help", "Affiche la liste des commandes", "help <commande> ou -s pour simplifier l'affichage", "help -s", 0, 1);
    commands_add(cmds, help);
    
    Command *exit = command_create("exit", "Quitter le programme", "aucuns arguments", NULL, 0, 0);
    commands_add(cmds, exit);
    
    Command *print = command_create("print", "Afficher la table", "aucuns arguments", NULL, 0, 0);
    commands_add(cmds, print);
    
    Command *insert = command_create("insert", "Insérer une entrée dans la table", "insert <attribut1> <attribut2> <attribut3> ...", "insert Jo Ibis 10", table->attributeCount, 0);
    commands_add(cmds, insert);
    
    Command *search = command_create("search", "Rechercher des entrées dans la table", "search <attribut> <opérateur> <valeur> [valeur2]", "search Prenom < Jo (<> pour between)", 3, 6);
    commands_add(cmds, search);
    
    Command *delete = command_create("delete", "Supprimer une  ou plusieurs entrées de la table, selon un attribut", "delete <attribut> <valeur> ", "delete Prenom Jo", 2, 0);
    commands_add(cmds, delete);
    
    Command *clear = command_create("clear", "Effacer l'écran", "aucuns arguments", NULL, 0, 0);
    commands_add(cmds, clear);

    Command *show = command_create("show", "Affiche les tables", "aucuns arguments", NULL, 0, 0);
    commands_add(cmds, show);

    Command *ibijau = command_create("ibijau", "Ils sont pas bo", "aucuns arguments", NULL, 0, 0);
    commands_add(cmds, ibijau);

    Command *select = command_create("select", "Sélectionner une table", "select <attribut>", "select Prenom", 1, 0);
    commands_add(cmds, select);

    Command* sort = command_create("sort", "Trier la table", "sort <attribut> <asc|desc>", "sort Prenom asc", 2, 0);
	commands_add(cmds, sort);

    Command* export = command_create("export", "Exporter la table dans un fichier CSV", "export <chemin>", "export ../data/students/students.csv", 1, 0);
    commands_add(cmds, export);

	Command *count = command_create("count", "Compter le nombre d'entrées dans la table", "count <attribut> <valeur>", "count Prenom Jo", 2, 0);
    commands_add(cmds, count);

	Command *stats = command_create("stats", "Afficher les statistiques de la table", "aucuns arguments", NULL, 0, 0);
    commands_add(cmds, stats);

	Command *structure = command_create("structure", "Afficher la structure de la table", "aucuns arguments", NULL, 0, 0);
    commands_add(cmds, structure);

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

    char autocomplete[100] = { 0 };
    int index = 0;

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
            char** cmdArgs = argv + 1;
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