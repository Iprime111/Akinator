#ifndef AKINATOR_H_
#define AKINATOR_H_

#include "Akinator/AkinatorLocales.h"
#include "Tree.h"
#include "TreeDefinitions.h"

const size_t DATABASE_CHUNK_MAX_SIZE = 512;

enum AkinatorError {
    NO_AKINATOR_ERRORS  = 0,
    TREE_ERROR          = 1 << 0,
    NULL_STRUCT_POINTER = 1 << 1,
    DATABASE_ERROR      = 1 << 2,
    NULL_TREE_POINTER   = 1 << 3,
    USER_INPUT_ERROR    = 1 << 4,
    NODE_NOT_FOUND      = 1 << 5,
};

struct Akinator {
    Tree::Tree <char *> databaseTree = {};

    AkinatorError errors = NO_AKINATOR_ERRORS;

    const AkinatorLocale *locale = NULL;
};

enum UserAnswer {
    NO_ANSWER        = 0,
    ANSWER_POSITIVE  = 1,
    ANSWER_NEGATIVE  = 2,
};

#define COMMAND(ID, NAME, DESCRIPTION, ...) NAME##_COMMAND = ID,

enum AkinatorCommand {
    #include "AkinatorCommands.h"
};

#undef COMMAND

AkinatorError SaveDatabase       (Akinator *akinator, char *databaseFilename);
AkinatorError ReadDatabase       (Akinator *akinator, char *databaseFilename);
AkinatorError CreateNewDatabase  (Akinator *akinator);
AkinatorError InitAkinator       (Akinator *akinator, char *databaseFilename, const AkinatorLocale *locale = &RussianLocale);
AkinatorError DestroyAkinator    (Akinator *akinator);
AkinatorError GuessNode          (Akinator *akinator);
AkinatorError CompareNodes       (Akinator *akinator);
AkinatorError GetNodeDescription (Akinator *akinator);

int NodeDataComparator (void *node1, void *node2);

#endif