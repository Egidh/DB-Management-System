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

    Table *table = Table_createFromCSV("../data/psittamulgiformes.csv", "../data");

    bool quit = false;
    ui_displayWelcome();
    char buffer[512];

    while (!quit)
    {
        printf("> ");
        fgets(buffer, 512, stdin);
        
        Command cmd = str_to_cmd(buffer);
    }

    Table_destroy(table);

    return 0;
}