/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN and Matthieu LE BERRE.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "Settings.h"
#include "Table.h"
#include "Index.h"
#include "UI.h"


int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    bool quit = false;
    ui_displayWelcome();

    Table *table = Table_createFromCSV("../data/psittamulgiformes.csv", "../data");
    while (!quit)
    {

        char buffer[512];
        printf("> ");
        fgets(buffer, 512, stdin);

        if (strcmp(buffer, "exit\n") == 0) quit = 1;
    }

    Table_destroy(table);

    return 0;
}