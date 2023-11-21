#include <cstdlib>
#include <iterator>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "Akinator/Akinator.h"
#include "Akinator/AkinatorLocales.h"
#include "CustomAssert.h"
#include "Logger.h"
#include "Tree.h"
#include "TreeDefinitions.h"
#include "TreeExtensions.h"

static AkinatorError ReadDatabaseChunck     (Akinator *akinator, Tree::Node <char *> *parentNode, Tree::TreeEdge direction, FILE *database, char *scanBuffer); 
static AkinatorError ReadNodeContent        (Tree::Node <char *> *node, FILE *database, char *scanBuffer);
static AkinatorError VerifyAkinatorTreeNode (const Tree::Node <char *> *root); 

AkinatorError VerifyAkinator (Akinator *akinator) {
    PushLog (3);

    if (!akinator) {
        RETURN NO_AKINATOR_ERRORS;
    }

    ReturnAkinatorErrors (akinator, VerifyAkinatorTreeNode (akinator->databaseTree.root));

    RETURN akinator->errors;
}

static AkinatorError VerifyAkinatorTreeNode (const Tree::Node <char *> *root) {
    PushLog (4);

    if (!root) {
        RETURN TREE_ERROR;
    }

    if ((root->left && !root->right) || (root->right && !root->left)) {
        RETURN TREE_ERROR;
    }

    if (!root->nodeData) {
        RETURN DATABASE_ERROR;
    }

    if (root->left) {
        VerifyAkinatorTreeNode (root->left);
        VerifyAkinatorTreeNode (root->right);
    }

    RETURN NO_AKINATOR_ERRORS;
}

AkinatorError ReadDatabase (Akinator *akinator, char *databaseFilename) {
    PushLog (2);
    
    custom_assert (akinator,         pointer_is_null, NULL_STRUCT_POINTER);
    custom_assert (databaseFilename, pointer_is_null, DATABASE_ERROR);

    if (access (databaseFilename, F_OK) != 0) {
        ReturnAkinatorErrors (akinator, DATABASE_ERROR);
    }

    FILE *database = fopen (databaseFilename, "r");  
    char *scanBuffer = (char *) calloc (DATABASE_CHUNK_MAX_SIZE, sizeof (char));

    AkinatorError error = ReadDatabaseChunck (akinator, NULL, Tree::NO_EDGE, database, scanBuffer); 

    if (error != NO_AKINATOR_ERRORS) {
        fclose (database);
        Tree::TreeDump (&akinator->databaseTree);
        Tree::DestroyTree (&akinator->databaseTree);
        ReturnAkinatorErrors (akinator, error);
        free (scanBuffer);
    }

    free (scanBuffer);
    fclose (database);
    RETURN NO_AKINATOR_ERRORS;
}

static AkinatorError ReadDatabaseChunck (Akinator *akinator, Tree::Node <char *> *parentNode, Tree::TreeEdge direction, FILE *database, char *scanBuffer) {
    PushLog (2);

    custom_assert (akinator,   pointer_is_null, NULL_STRUCT_POINTER);
    custom_assert (database,   pointer_is_null, DATABASE_ERROR);
    custom_assert (scanBuffer, pointer_is_null, DATABASE_ERROR);
    
    if (fscanf (database, "%s", scanBuffer) == 0) {
        ReturnAkinatorErrors (akinator, DATABASE_ERROR);
    }

    if (scanBuffer [0] != '(') {
        if (strcmp (scanBuffer, "nil") == 0) {
            RETURN NO_AKINATOR_ERRORS;
        } else {
            ReturnAkinatorErrors (akinator, DATABASE_ERROR);
        }
    }

    Tree::Node <char *> *currentNode = NULL;

    if (!parentNode) {
        currentNode = akinator->databaseTree.root;
    } else {
        Tree::TreeError treeError = Tree::AddNode (&akinator->databaseTree, parentNode, direction);

        if (treeError != Tree::NO_TREE_ERRORS) {
            ReturnAkinatorErrors (akinator, TREE_ERROR);
        }
        
        if (direction == Tree::LEFT_CHILD) {
            currentNode = parentNode->left;
        } else if (direction == Tree::RIGHT_CHILD) {
            currentNode = parentNode->right;
        } else {
            ReturnAkinatorErrors (akinator, TREE_ERROR);
        }
    }

    if (!currentNode)
        ReturnAkinatorErrors (akinator, TREE_ERROR);

    WriteAkinatorErrors (akinator, ReadNodeContent (currentNode, database, scanBuffer));

    #define RecursiveCall(direction) WriteAkinatorErrors (akinator, ReadDatabaseChunck (akinator, currentNode, direction, database, scanBuffer))  

    RecursiveCall (Tree::LEFT_CHILD);
    RecursiveCall (Tree::RIGHT_CHILD);
    
    #undef RecursiveCall

    if (!fscanf (database, "%s", scanBuffer))
        WriteAkinatorErrors (akinator, DATABASE_ERROR);

    if (scanBuffer [0] != ')') {
        WriteAkinatorErrors (akinator, DATABASE_ERROR);
    }

    RETURN akinator->errors;
}

static AkinatorError ReadNodeContent (Tree::Node <char *> *node, FILE *database, char *scanBuffer) {
    PushLog (3);

    fscanf (database, " ");

    if (getc (database) != '\"') {
        RETURN DATABASE_ERROR;
    }

    char *nodeContent = NULL;
    size_t bufferSize = 0;
    getdelim (&nodeContent, &bufferSize, '\"', database);
    getc (database);

    size_t nodeContentLength = strlen (nodeContent);

    if (nodeContentLength > 0)
        nodeContent [nodeContentLength - 1] = '\0'; 

    node->nodeData = nodeContent; 

    RETURN NO_AKINATOR_ERRORS;
}

AkinatorError SaveDatabase (Akinator *akinator, char *databaseFilename) {
    PushLog (2);

    custom_assert (databaseFilename, pointer_is_null, DATABASE_ERROR);

    Verification (akinator);

    FILE *database = fopen (databaseFilename, "w");

    if (!database) {
        ReturnAkinatorErrors (akinator, DATABASE_ERROR);
    }

    Tree::PrintTree (&akinator->databaseTree, Tree::PREFIX_PRINT, database);

    fclose (database);

    RETURN NO_AKINATOR_ERRORS;
}

AkinatorError CreateNewDatabase (Akinator *akinator) {
    PushLog (3);

    akinator->databaseTree.root->nodeData = (char *) calloc (DATABASE_CHUNK_MAX_SIZE, sizeof (char));
    strcpy (akinator->databaseTree.root->nodeData, akinator->locale->defaultRecord);

    RETURN NO_AKINATOR_ERRORS;
}

AkinatorError InitAkinator (Akinator *akinator, char *databaseFilename, const AkinatorLocale *locale) {
    PushLog (3);

    custom_assert (akinator,         pointer_is_null, NULL_STRUCT_POINTER);
    custom_assert (databaseFilename, pointer_is_null, TREE_ERROR);

    akinator->locale = locale;

    akinator->errors = NO_AKINATOR_ERRORS;
    if (Tree::InitTree (&akinator->databaseTree) != Tree::NO_TREE_ERRORS) {
        akinator->errors = TREE_ERROR;
    }

    if (databaseFilename && (access (databaseFilename, F_OK) == 0)) {
        char *savedDatabaseFilename = (char *) calloc (strlen (databaseFilename) + 1, sizeof (char));
        strcpy (savedDatabaseFilename, databaseFilename);

        ReadDatabase (akinator, databaseFilename);
        akinator->databaseFilename = savedDatabaseFilename;
    } else {
        CreateNewDatabase (akinator);
        akinator->databaseFilename = NULL;
    }
    
    Verification (akinator);

    RETURN akinator->errors;
}

AkinatorError DestroyAkinator (Akinator *akinator) {
    PushLog (3);

    custom_assert (akinator, pointer_is_null, NULL_STRUCT_POINTER);

    free (akinator->databaseFilename);
    Tree::DestroyTree (&akinator->databaseTree);

    RETURN NO_AKINATOR_ERRORS;
}

int NodeDataComparator (void *node1, void *node2) {
    return strcmp (*((char **) node1), *((char **) node2));
}
