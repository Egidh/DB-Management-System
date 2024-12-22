#include "UI.h"


//
//bool handle_command(Table *table, Command cmd, char **args)
//{
//    switch (cmd) {
//    case CMD_HELLO:
//        cmd_hello();
//        break;
//
//    case CMD_HELP:
//        commands_displayHelp(args);
//        break;
//
//    case CMD_EXIT:
//        cmd_exit();
//        return false;
//
//    case CMD_INSERT:
//        cmd_insert(table, args, argc, commands);
//        break;
//
//    case CMD_SEARCH:
//        cmd_search(table, args, argc, commands);
//        break;
//
//    case CMD_DELETE:
//        cmd_delete(table, args, argc, commands);
//        break;
//
//    case CMD_CLEAR:
//        ui_clearScreen();
//        ui_displayWelcome();
//        break;
//
//    case CMD_SHOW:
//		cmd_show(table);
//		break;
//
//    case CMD_IBIJAU:
//		cmd_displayIbijau();
//		break;
//
//    case CMD_SELECT:
//        cmd_selectTable(table, args, argc, commands);
//        break;
//
//    case CMD_SORT:
//        cmd_sort(table, args, argc, commands);
//		break;
//    
//    case CMD_EXPORT:
//		cmd_export(table, args, argc, commands);
//		break;
//
//    case CMD_COUNT:
//        cmd_count(table, args, argc, commands);
//        break;
//
//    case CMD_STATS:
//		cmd_stats(table, args, argc, commands);
//		break;
//
//    case CMD_STRUCTURE:
//		cmd_structure(table);
//		break;
//
//    //case CMD_MODIFY:
//    //    cmd_modify();
//    //    break;
//
//    default:
//        ui_displayError("Commande non implémentée");
//        break;
//    }
//
//    return true;
//}
//
