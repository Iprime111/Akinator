#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <festival/festival.h>

#include "Akinator/Akinator.h"
#include "Buffer.h"
#include "CustomAssert.h"
#include "Logger.h"
#include "TreeDefinitions.h"

static AkinatorError FindDiff   (Akinator *akinator, Buffer <Tree::TreeEdge> *firstNodePath, Buffer <Tree::TreeEdge> *secondNodePath,
                                    Tree::Node <char *> *firstNode, Tree::Node <char *> *secondNode); 
static AkinatorError SwitchNode (Akinator *akinator, Tree::Node <char *> **node);
static AkinatorError MakeGuess  (Akinator *akinator, Tree::Node <char *> *node);
static AkinatorError AddRecord  (Akinator *akinator, Tree::Node <char *> *node);
static AkinatorError ScanAnswer (Akinator *akinator, UserAnswer *answer);

static AkinatorError PrintNodeDiff (Akinator *akinator, Tree::Node <char *> *node, Buffer <Tree::TreeEdge> *nodePath, size_t pathOffset);

AkinatorError GuessNode (Akinator *akinator) {
    PushLog (1);

    Verification (akinator);

    custom_assert (akinator->databaseTree.root, pointer_is_null, NULL_TREE_POINTER);

    Tree::Node <char *> *currentNode = akinator->databaseTree.root;

    while (currentNode->left && currentNode->right) {
        SwitchNode (akinator, &currentNode);
    }

    MakeGuess (akinator, currentNode);

    RETURN NO_AKINATOR_ERRORS;
}

AkinatorError CompareNodes (Akinator *akinator) {
    PushLog (1);

    Verification (akinator);

    printf ("\n%s\n", akinator->locale->comparisonRequestMessage);

    char *firstNodeName  = NULL;
    char *secondNodeName = NULL;
    
    scanf ("%ms[^\n]", &firstNodeName);
    scanf ("%ms[^\n]", &secondNodeName);

    Buffer <Tree::TreeEdge> firstNodePath  = {};
    Buffer <Tree::TreeEdge> secondNodePath = {};

    InitBuffer (&firstNodePath);
    InitBuffer (&secondNodePath);

    Tree::Node <char *> *firstNode  = NULL;
    Tree::Node <char *> *secondNode = NULL;

    #define DestroyAllBuffers()                 \
        do {                                    \
            DestroyBuffer (&firstNodePath);     \
            DestroyBuffer (&secondNodePath);    \
            free (firstNodeName);               \
            free (secondNodeName);              \
        } while (0)

    #define FindNode(nodePrefix)                                                                                                                                                \
       do {                                                                                                                                                                     \
           if (Tree::FindNode (&akinator->databaseTree, &(nodePrefix##Node), &(nodePrefix##NodePath), &(nodePrefix##NodeName), NodeDataComparator) != Tree::NO_TREE_ERRORS) {   \
               printf (akinator->locale->nodeNotFound, nodePrefix##NodeName);                                                                                                   \
               printf ("\n");                                                                                                                                                   \
               DestroyAllBuffers ();                                                                                                                                            \
               RETURN TREE_ERROR;                                                                                                                                               \
           }                                                                                                                                                                    \
       } while (0)

    FindNode (first);
    FindNode (second);

    printf ("\n");

    AkinatorError error = FindDiff (akinator, &firstNodePath, &secondNodePath, firstNode, secondNode);
    DestroyAllBuffers ();

    #undef DestroyAllBuffers
    #undef FindNode

    RETURN error;
}

static AkinatorError FindDiff (Akinator *akinator, Buffer <Tree::TreeEdge> *firstNodePath, Buffer <Tree::TreeEdge> *secondNodePath,
                            Tree::Node <char *> *firstNode, Tree::Node <char *> *secondNode) {
    PushLog (2);

    custom_assert (akinator,       pointer_is_null, NULL_STRUCT_POINTER);
    custom_assert (firstNodePath,  pointer_is_null, TREE_ERROR);
    custom_assert (secondNodePath, pointer_is_null, TREE_ERROR);
    custom_assert (firstNode,      pointer_is_null, TREE_ERROR);
    custom_assert (secondNode,     pointer_is_null, TREE_ERROR);

    size_t firstPathIndex  = 0;
    size_t secondPathIndex = 0;

    printf (akinator->locale->similarityMessage, firstNode->nodeData, secondNode->nodeData);
    printf ("\n");

    Tree::Node <char *> *firstCurrentNode  = akinator->databaseTree.root;
    Tree::Node <char *> *secondCurrentNode = akinator->databaseTree.root;
    
    for (firstPathIndex = 0, secondPathIndex = 0; firstNodePath->data [firstPathIndex] == secondNodePath->data [secondPathIndex] &&
            firstPathIndex < firstNodePath->currentIndex; firstPathIndex++, secondPathIndex++) { 
        printf ("\t%s\n", firstCurrentNode->nodeData);

        firstCurrentNode  = Tree::NextNode (firstCurrentNode,  firstNodePath->data  [firstPathIndex]);
        secondCurrentNode = Tree::NextNode (secondCurrentNode, secondNodePath->data [secondPathIndex]); 
    }

    printf (akinator->locale->firstItemDifferencesMessage, firstNode->nodeData);
    printf ("\n");

    PrintNodeDiff (akinator, firstCurrentNode, firstNodePath, firstPathIndex);

    printf (akinator->locale->secondItemDifferencesMessage, secondNode->nodeData);
    printf ("\n");

    PrintNodeDiff (akinator, secondCurrentNode, secondNodePath, secondPathIndex);

    RETURN NO_AKINATOR_ERRORS;
}

static AkinatorError PrintNodeDiff (Akinator *akinator, Tree::Node <char *> *node, Buffer <Tree::TreeEdge> *nodePath, size_t pathOffset) {
    PushLog (4);

    custom_assert (akinator, pointer_is_null, NULL_STRUCT_POINTER);

    custom_assert (node,     pointer_is_null, TREE_ERROR);
    custom_assert (nodePath, pointer_is_null, TREE_ERROR);

    custom_assert (pathOffset < nodePath->currentIndex, invalid_value, TREE_ERROR);

    for (size_t nodePathIndex = pathOffset; nodePathIndex < nodePath->currentIndex; nodePathIndex++) {
        printf ("\t");

        if (nodePath->data [nodePathIndex] == Tree::LEFT_CHILD)
            printf ("%s", akinator->locale->negativeCharacteristicsPrefix);

        printf (" %s\n", node->nodeData);

        node = Tree::NextNode (node, nodePath->data [nodePathIndex]); 
    }

    RETURN NO_AKINATOR_ERRORS;
}

AkinatorError GetNodeDescription (Akinator *akinator) {
    PushLog (1);

    Verification (akinator);

    char *nodeNameBuffer = NULL;

    printf ("\n%s ", akinator->locale->descriptionRequestMessage);

    scanf("%ms[^\n]", &nodeNameBuffer);

    Tree::Node <char *> *node = NULL;
    Buffer <Tree::TreeEdge> nodePath = {};

    InitBuffer (&nodePath);

    if (Tree::FindNode (&akinator->databaseTree, &node, &nodePath, &nodeNameBuffer, NodeDataComparator) != Tree::NO_TREE_ERRORS) {
        printf (akinator->locale->nodeNotFound, nodeNameBuffer);
        printf ("\n");

        DestroyBuffer (&nodePath);
        free (nodeNameBuffer);
        RETURN TREE_ERROR;
    }


    printf (akinator->locale->descriptionTitle, node->nodeData);
    printf ("\n");

    PrintNodeDiff (akinator, akinator->databaseTree.root, &nodePath, 0);

    free (nodeNameBuffer);
    DestroyBuffer (&nodePath);
    RETURN NO_AKINATOR_ERRORS;
}

static AkinatorError SwitchNode (Akinator *akinator, Tree::Node <char *> **node) {
    PushLog (3);

    custom_assert (akinator, pointer_is_null, NULL_STRUCT_POINTER);
    custom_assert (node,     pointer_is_null, TREE_ERROR);
    custom_assert (*node,    pointer_is_null, TREE_ERROR);

    printf (akinator->locale->question, (*node)->nodeData, akinator->locale->positiveAnswer, akinator->locale->negativeAnswer);
    printf ("\n");
    

    UserAnswer answer = NO_ANSWER;
    ScanAnswer (akinator, &answer);

    if (answer == ANSWER_POSITIVE)
        *node = (*node)->right;
    else if (answer == ANSWER_NEGATIVE)
        *node = (*node)->left;
    else {
        RETURN USER_INPUT_ERROR;
    }

    RETURN NO_AKINATOR_ERRORS;
}

static AkinatorError MakeGuess (Akinator *akinator, Tree::Node <char *> *node) {
    PushLog (3);

    custom_assert (node, pointer_is_null, TREE_ERROR);

    Verification (akinator);

    printf (akinator->locale->makeGuess, node->nodeData, akinator->locale->positiveAnswer, akinator->locale->negativeAnswer);
    printf ("\n");
    
    UserAnswer answer = NO_ANSWER;
    ScanAnswer (akinator, &answer);

    if (answer == ANSWER_POSITIVE) {
        printf ("%s\n", akinator->locale->rightAnswer);
        RETURN NO_AKINATOR_ERRORS;
    } else if (answer == ANSWER_NEGATIVE) {
        AddRecord (akinator, node);
    } else {
        RETURN USER_INPUT_ERROR;
    }

    RETURN NO_AKINATOR_ERRORS;
}

static AkinatorError AddRecord (Akinator *akinator, Tree::Node <char *> *node) {
    PushLog (3);

    custom_assert (akinator, pointer_is_null, NULL_STRUCT_POINTER);
    custom_assert (node,     pointer_is_null, TREE_ERROR);

    char *newNodeDataBuffer  = NULL;
    char *questionDataBuffer = NULL;
    
    printf ("%s\n", akinator->locale->wrongAssumption);
    scanf ("%ms[^\n]", &newNodeDataBuffer);

    printf (akinator->locale->newNodeSpecificQuestion, newNodeDataBuffer, node->nodeData);
    printf ("\n");
    scanf ("%ms[^\n]", &questionDataBuffer);

    Tree::AddNode (&akinator->databaseTree, node, Tree::LEFT_CHILD);
    Tree::AddNode (&akinator->databaseTree, node, Tree::RIGHT_CHILD);
    
    node->left->nodeData  = node->nodeData;
    node->right->nodeData = newNodeDataBuffer;
    node->nodeData        = questionDataBuffer;

    RETURN NO_AKINATOR_ERRORS;
}

static AkinatorError ScanAnswer (Akinator *akinator, UserAnswer *answer) {
    PushLog (4);
   
    custom_assert (akinator, pointer_is_null, NULL_STRUCT_POINTER);
    custom_assert (answer,   pointer_is_null, NULL_STRUCT_POINTER);

    char scanBuffer [DATABASE_CHUNK_MAX_SIZE] = "";
    scanf ("%s", scanBuffer);

    while (strcmp (scanBuffer, akinator->locale->positiveAnswer) && strcmp (scanBuffer, akinator->locale->negativeAnswer)) {
        printf (akinator->locale->wrongAnswerFormat, akinator->locale->positiveAnswer, akinator->locale->negativeAnswer);
        printf ("\n");
        scanf ("%s", scanBuffer);
    }

    if (!strcmp (scanBuffer, akinator->locale->positiveAnswer))
        *answer = ANSWER_POSITIVE;
    else
        *answer = ANSWER_NEGATIVE;

    RETURN NO_AKINATOR_ERRORS;
}
