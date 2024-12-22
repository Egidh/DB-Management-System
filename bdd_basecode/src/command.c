#include "UI.h"

void commands_destroy(Commands* commands) {
    if (!commands) return;

    if (commands->commands) {
        for (int i = 0; i < commands->commandCount; i++) {
            free(commands->commands[i].name);
            free(commands->commands[i].description);
            free(commands->commands[i].argList);
            free(commands->commands[i].example);
        }
        free(commands->commands);
    }
    free(commands);
}

int commands_getIndex(const Commands* commands, const char* commandName) {
    if (!commands || !commandName) return -1;

    for (int i = 0; i < commands->commandCount; i++) {
        if (strcmp(commands->commands[i].name, commandName) == 0) {
            return i;
        }
    }
    return -1;
}

void commands_displayHelp(const Commands* cmd, const char** args, int argc) {
    printf("Commandes disponibles :\n\n");

    if (argc) {
        if (strcmp(args[0], "-s") == 0) {
            for (int i = 0; i < cmd->commandCount; i++) {
			    printf("%s: %s\n", cmd->commands[i].name, cmd->commands[i].description);
		    }
            return;
        }

        int cmdIndex = commands_getIndex(cmd, args[0]);

        printf("%s: %s\n", cmd->commands[cmdIndex].name, cmd->commands[cmdIndex].description);

        if (cmd->commands[cmdIndex].example != NULL) {
			char buffer[256];
			sprintf(buffer, "   - Exemple : %s\n", cmd->commands[cmdIndex].example);
			ui_displayColoredText(buffer, COLOR_BLUE);
		}

        if (cmd->commands[cmdIndex].argCount > 0) {
            char buffer[256];
            sprintf(buffer, "   - Arguments requis : %d\n", cmd->commands[cmdIndex].argCount);
            ui_displayColoredText(buffer, COLOR_YELLOW);
        }

        if (cmd->commands[cmdIndex].optionAgrCount > 0) {
            char buffer[256];
            sprintf(buffer, "   - Arguments optionnels : %d\n", cmd->commands[cmdIndex].optionAgrCount);
            ui_displayColoredText(buffer, COLOR_YELLOW);
        }

        if (cmd->commands[cmdIndex].argCount == 0 && cmd->commands[cmdIndex].optionAgrCount == 0) {
            char buffer[256];
            sprintf(buffer, "   - Aucun argument requis.\n");
            ui_displayColoredText(buffer, COLOR_PURPLE);
        }

        if (cmd->commands[cmdIndex].optionAgrCount > 0 || cmd->commands[cmdIndex].argCount > 0) {
            char buffer[256];
            sprintf(buffer, "   - Liste des arguments : %s\n", cmd->commands[cmdIndex].argList);
            ui_displayColoredText(buffer, COLOR_GREEN);
        }

		return;
    }


    for (int i = 0; i < cmd->commandCount; i++) {
        printf("%s: %s\n", cmd->commands[i].name, cmd->commands[i].description);

        if (cmd->commands[i].example != NULL) {
            char buffer[256];
            sprintf(buffer, "   - Exemple : %s\n", cmd->commands[i].example);
            ui_displayColoredText(buffer, COLOR_BLUE);
        }

        if (cmd->commands[i].argCount > 0) {
            char buffer[256];
            sprintf(buffer, "   - Arguments requis : %d\n", cmd->commands[i].argCount);
            ui_displayColoredText(buffer, COLOR_YELLOW);
        }
        if (cmd->commands[i].optionAgrCount > 0) {
            char buffer[256];
			sprintf(buffer, "   - Arguments optionnels : %d\n", cmd->commands[i].optionAgrCount);
			ui_displayColoredText(buffer, COLOR_YELLOW);
        }
        
        if (cmd->commands[i].argCount == 0 && cmd->commands[i].optionAgrCount == 0) {
            char buffer[256];
            sprintf(buffer, "   - Aucun argument requis.\n");
            ui_displayColoredText(buffer, COLOR_PURPLE);
        }

        if (cmd->commands[i].optionAgrCount > 0 || cmd->commands[i].argCount > 0) {
            char buffer[256];
            sprintf(buffer, "   - Liste des arguments : %s\n", cmd->commands[i].argList);
			ui_displayColoredText(buffer, COLOR_GREEN);
		}

        printf("\n");
    }
}

Command* commands_get(const Commands* commands, int index) {
    if (!commands || index < 0 || index >= commands->commandCount) {
        return NULL;
    }
    return &commands->commands[index];
}

bool handle_command(Table* table, const char* command, char** args, int argc,
    const Commands* commands, Mode* mode) {
    int cmdIndex = commands_getIndex(commands, command);
    if (cmdIndex < 0) {
        ui_displayCommandNotFound();
        return true;
    }


    Command* cmd = commands_get(commands, cmdIndex);
    if (!cmd) {
        ui_displayError("Commande invalide");
        return true;
    }

    if (argc < cmd->argCount) {
        ui_displayError("Nombre d'arguments insuffisant");
        ui_displayArguments(cmd);
        ui_displayExample(cmd);
        return true;
    }

    if (argc > (cmd->argCount + cmd->optionAgrCount)) {
        ui_displayError("Trop d'arguments");
        ui_displayArguments(cmd);
        ui_displayExample(cmd);
        return true;
    }

    if (cmd->argCount == 0 && cmd->optionAgrCount == 0) {
		if (argc > 0) {
			ui_displayError("Cette commande ne prend pas d'arguments");
            ui_displayExample(cmd);
			return true;
		}
	}

    switch (cmdIndex) {
    case CMD_HELLO:
        cmd_hello();
        break;

    case CMD_HELP:
        commands_displayHelp(commands, args, argc);
        break;

    case CMD_EXIT:
        cmd_exit();
        return false;

    case CMD_PRINT:
        *mode = MODE_SELECT;
        break;

    case CMD_INSERT:
        cmd_insert(table, args, argc, commands);
        break;

    case CMD_SEARCH:
        cmd_search(table, args, argc, commands);
        break;

    case CMD_DELETE:
        cmd_delete(table, args, argc, commands);
        break;

    case CMD_CLEAR:
        ui_clearScreen();
        ui_displayWelcome();
        break;

    case CMD_SHOW:
		cmd_show(table);
		break;

    case CMD_IBIJAU:
		cmd_displayIbijau();
		break;

    case CMD_SELECT:
        cmd_selectTable(table, args, argc, commands);
        break;

    case CMD_SORT:
        cmd_sort(table, args, argc, commands);
		break;
    
    case CMD_EXPORT:
		cmd_export(table, args, argc, commands);
		break;

    case CMD_COUNT:
        cmd_count(table, args, argc, commands);
        break;

    case CMD_STATS:
		cmd_stats(table, args, argc, commands);
		break;

    case CMD_STRUCTURE:
		cmd_structure(table);
		break;
    //case CMD_MODIFY:
    //    cmd_modify();
    //    break;

    default:
        ui_displayError("Commande non implémentée");
        break;
    }

    return true;
}

Command* command_create(const char* name, const char* description, const char* argList, const char* exemple, int argCount, int optionArgCount) {
    Command* cmd = calloc(1, sizeof(Command));
    if (!cmd) {
        return NULL;
    }

    cmd->name = name;
    cmd->description = description;
    cmd->example = exemple;
    cmd->argList = argList;
    cmd->argCount = argCount;
    cmd->optionAgrCount = optionArgCount;

    if (!cmd->name || !cmd->description || !cmd->argList) {
        if (cmd->name) free(cmd->name);
        if (cmd->description) free(cmd->description);
        if (cmd->argList) free(cmd->argList);
        free(cmd);
        return NULL;
    }

    return cmd;
}

Commands* commands_create_list() {
    Commands* cmds = malloc(sizeof(Commands));
    if (!cmds) {
        return NULL;
    }

    cmds->commands = NULL; 
    cmds->commandCount = 0;

    return cmds;
}

bool commands_add(Commands* commands, Command* cmd) {
    if (!commands || !cmd) {
        return false;
    }

    Command* newCommands = realloc(commands->commands, sizeof(Command) * (commands->commandCount + 1));
    if (!newCommands) {
        return false;
    }

    commands->commands = newCommands;

    memcpy(&commands->commands[commands->commandCount], cmd, sizeof(Command));
    commands->commandCount++;

    return true;
}