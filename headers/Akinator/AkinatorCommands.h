// COMMAND (ID, NAME, DESCRIPTION, CALLBACK)

#include "TreeDefinitions.h"
COMMAND (0, QUIT, akinator->locale->quitCommand, {
    DestroyAkinator (&akinator);
    RETURN 0;
})

COMMAND (1, LOAD_DATABASE, akinator->locale->loadDatabaseCommand, {
    char filename [FILENAME_MAX] = "";
    scanf ("%s", filename);

    DestroyAkinator  (&akinator);
    if (InitAkinator (&akinator, filename, akinator.locale) != NO_AKINATOR_ERRORS) {
        printf ("%s\n", akinator.locale->initializationError);
    }
})

COMMAND (2, NEW_DATABASE, akinator->locale->newDatabaseCommand, {
    DestroyAkinator (&akinator);
    InitAkinator    (&akinator, "", akinator.locale);
})

COMMAND (3, SAVE_DATABSE, akinator->locale->saveDatabaseCommand, {
    char filename [FILENAME_MAX] = "";
    scanf ("%s", filename);

    FILE *outputFile = fopen (filename, "w");

    if (!outputFile) {
        printf ("%s\n", akinator.locale->databaseSaveError);
        continue;
    }

    Tree::PrintTree (&akinator.databaseTree, Tree::PREFIX_PRINT, outputFile);

    fclose (outputFile);
})

COMMAND (4, GUESS_NODE, akinator->locale->guessNodeCommand, {
    GuessNode (&akinator);
})

COMMAND (5, GET_DESCRIPTION, akinator->locale->getDescriptionCommand, {
    GetNodeDescription (&akinator);
})

COMMAND (6, COMPARE, akinator->locale->compareNodesCommand, {
    CompareNodes (&akinator);
})

COMMAND (7, CHANGE_LOCALE, akinator->locale->changeLanguageCommand, {
    ChangeLocale (&akinator); 
})
