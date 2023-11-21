#include "Akinator/Akinator.h"
#include "Akinator/AkinatorLocales.h"
#include "CustomAssert.h"
#include "Logger.h"

static void PrintCommands (Akinator *akinator);
static void ChangeLocale  (Akinator *akinator);

int main () {
    PushLog (1);
    Akinator akinator = {};

    InitAkinator (&akinator, "./летопись.дуб", &LocalesArray [0]);

    while (true) {
        PrintCommands (&akinator);
        printf ("%s ", akinator.locale->commandRequest);
        
        int commandNumber = -1;
        if (scanf ("%d", &commandNumber) == 0) {
            printf ("%s\n", akinator.locale->wrongCommandId);
            continue;
        }

        getchar ();

        #define COMMAND(ID, NAME, DESCRIPTION, ...) \
            if (commandNumber == ID) {              \
                __VA_ARGS__;                        \
                continue;                           \
            }
        #include "Akinator/AkinatorCommands.def"

        printf ("%s\n", akinator.locale->wrongCommand);
    }

    #undef COMMAND

    RETURN 0;
}

static void PrintCommands (Akinator *akinator) {
    PushLog (4);

    printf ("\n");

    #define COMMAND(ID, NAME, DESCRIPTION, ...) \
        printf ("%d. %s\n", ID, DESCRIPTION);

    #include "Akinator/AkinatorCommands.def"

    #undef COMMAND

    RETURN;
}

static void ChangeLocale (Akinator *akinator) {
    PushLog (4);

    int localeId = 0;

    printf ("\n");

    for (size_t localeIndex = 0; localeIndex < LOCALES_COUNT; localeIndex++) {
        printf ("%lu. %s\n", localeIndex + 1, LocalesArray [localeIndex].localeName);
    }

    printf ("%s ", akinator->locale->changeLocaleMessage);

    if (scanf ("%d", &localeId) == 0) {
        printf ("%s\n", akinator->locale->wrongLocaleId);
        RETURN;
    }

    if (localeId <= 0 || (size_t) localeId > LOCALES_COUNT) {
        printf ("%s\n", akinator->locale->wrongLocaleId);
        RETURN;
    }

    akinator->locale = &LocalesArray [localeId - 1];

    RETURN;
}
