/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN and Matthieu LE BERRE.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "Settings.h"
#include "Table.h"


//load from file
#if 0
int main(int argc, char** argv)
{
    Table* table = Table_load("psittamulgiformes.tbl", "../data/intro");
    //Table* table = Table_load("students.tbl", "../data/students");

    Filter filter = { 0, OP_EQ, "Cherry", "19" };
    SetEntry* results = SetEntry_create();
    Table_search(table, &filter, results);

    Table_removeEntry(table, results->root->data);
    SetEntry_destroy(results);

    results = SetEntry_create();
    Table_search(table, &filter, results);
    Table_printSearchResult(results, table);


    Entry* arthur = Entry_create(table);
    strcpy(arthur->values[0], "Arthur");
    strcpy(arthur->values[1], "Quokka");
    strcpy(arthur->values[2], "193");

    Table_insertEntry(table, arthur);
    Entry_destroy(arthur);

    SetEntry_destroy(results);
    Table_destroy(table);

    return EXIT_SUCCESS;
}
#endif

//create from scratch
#if 1
int main(int argc, char** argv)
{
    //Table* table = Table_createFromCSV("../data/students/students.csv", "../data/students");
    Table* table = Table_createFromCSV("../data/intro/psittamulgiformes.csv", "../data/intro");

    Entry* cherry = Entry_create(table);
    strcpy(cherry->values[0], "Cherry");
    strcpy(cherry->values[1], "Quokka");
    strcpy(cherry->values[2], "36");

    Table_insertEntry(table, cherry);

    Entry* light = Entry_create(table);
    strcpy(light->values[0], "Light");
    strcpy(light->values[1], "Quokka");
    strcpy(light->values[2], "25");

    Table_insertEntry(table, light);


    Filter filter = { 0, OP_EQ, "Cherry", "19" };
    SetEntry* results = SetEntry_create();
    Table_search(table, &filter, results);

    Table_removeEntry(table, results->root->data);
    SetEntry_destroy(results);

    results = SetEntry_create();
    Table_search(table, &filter, results);
    Table_printSearchResult(results, table);


    Entry* arthur = Entry_create(table);
    strcpy(arthur->values[0], "Arthur");
    strcpy(arthur->values[1], "Quokka");
    strcpy(arthur->values[2], "193");

    Table_insertEntry(table, arthur);
    Entry_destroy(arthur);


    Entry* daniil = Entry_create(table);
    strcpy(daniil->values[0], "Daniil");
    strcpy(daniil->values[1], "Alcoolique");
    strcpy(daniil->values[2], "175");

    Table_insertEntry(table, daniil);
    Entry_destroy(daniil);

    SetEntry_destroy(results);

    Entry_destroy(cherry);
    Entry_destroy(light);


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