#include "Akinator/Akinator.h"
#include "CustomAssert.h"

static void PrintCommands ();

int main () {
    PushLog (1);
 
    Akinator akinator = {};

    #define COMMAND(ID, NAME, DESCRIPTION, ...) \
        if (commandNumber == ID) {              \
            __VA_ARGS__;                        \
            continue;                           \
        }

    InitAkinator (&akinator, "");

    while (true) {
        PrintCommands ();
        printf ("%s ", akinator.locale->commandRequest);
        
        int commandNumber = -1;
        if (scanf ("%d", &commandNumber) == 0) {
            printf ("%s\n", akinator.locale->wrongCommandId);
            continue;
        }

        getchar ();

        #include "Akinator/AkinatorCommands.h"

        printf ("%s\n", akinator.locale->wrongCommand);
    }

    #undef COMMAND

    RETURN 0;
}

static void PrintCommands () {
    PushLog (4);

    #define COMMAND(ID, NAME, DESCRIPTION, ...) \
        printf ("%d. %s\n", ID, DESCRIPTION);

    #include "Akinator/AkinatorCommands.h"

    #undef COMMAND

    RETURN;
}
