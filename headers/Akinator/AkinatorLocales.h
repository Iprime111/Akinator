#ifndef AKINATOR_LOCALES_H_
#define AKINATOR_LOCALES_H_

#include "stddef.h"

#define LOCALE_FIELD_INIT(NAME) const char *NAME = NULL

struct AkinatorLocale {
    LOCALE_FIELD_INIT (defaultRecord);
    LOCALE_FIELD_INIT (nodeNotFound);
    LOCALE_FIELD_INIT (comparisonRequestMessage);
    LOCALE_FIELD_INIT (descriptionRequestMessage);
    LOCALE_FIELD_INIT (positiveAnswer);
    LOCALE_FIELD_INIT (negativeAnswer);
    LOCALE_FIELD_INIT (negativeCharacteristicsPrefix);
    LOCALE_FIELD_INIT (similarityMessage);
    LOCALE_FIELD_INIT (firstItemDifferencesMessage);
    LOCALE_FIELD_INIT (secondItemDifferencesMessage);
    LOCALE_FIELD_INIT (wrongAnswerFormat);
    LOCALE_FIELD_INIT (wrongAssumption);
    LOCALE_FIELD_INIT (newNodeSpecificQuestion);
    LOCALE_FIELD_INIT (descriptionTitle);
    LOCALE_FIELD_INIT (makeGuess);
    LOCALE_FIELD_INIT (question);
    LOCALE_FIELD_INIT (rightAnswer);
    LOCALE_FIELD_INIT (wrongCommand);
    LOCALE_FIELD_INIT (wrongCommandId);
    LOCALE_FIELD_INIT (commandRequest);
};

#undef LOCALE_FIElD_INIT

#define LOCALE_FIELD(NAME, VALUE) .NAME = VALUE,

const AkinatorLocale EnglishLocale {
    #include "Locales/EnglishLocale.h"
};

const AkinatorLocale RussianLocale {
    #include "Locales/RussianLocale.h"
};


#undef LOCALE_FIElD

#endif
