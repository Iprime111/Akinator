#ifndef TREE_EXTENSIONS_H_
#define TREE_EXTENSIONS_H_

#include "Logger.h"
#include "Tree.h"

namespace Tree {
    template <>
    TreeError NodeDataDestructor (Node <char *> *node) {
        PushLog (4);
    
        free (node->nodeData);
    
        RETURN NO_TREE_ERRORS;
    }

    template <>
    TreeError PrintNodeValue (Node <char *> *node, FILE *stream) {
        PushLog (4);

        fprintf (stream, "\" %s \"", node->nodeData);

        RETURN NO_TREE_ERRORS;
    }

    template <>
    TreeError SnprintNodeValue (Node <char *> *node, char *buffer, size_t maxLength) {
        PushLog (4);

        snprintf (buffer, maxLength, "%s", node->nodeData);

        RETURN NO_TREE_ERRORS;
    }
}

#endif
