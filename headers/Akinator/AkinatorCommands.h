// COMMAND (ID, NAME, DESCRIPTION, CALLBACK)

COMMAND (0, QUIT, "Выйти из программы", {
    DestroyAkinator (&akinator);
    RETURN 0;
})

COMMAND (1, LOAD_DATABASE, "Загрузить базу данных из файла", {
    char filename [FILENAME_MAX] = "";

    scanf ("%s", filename);

    DestroyAkinator  (&akinator);
    if (InitAkinator (&akinator, filename) != NO_AKINATOR_ERRORS) {
        printf ("Возникла ошибка при инициализации акинатора");
    }
})

COMMAND (2, NEW_DATABASE, "Загрузить пустую базу данных", {
    DestroyAkinator (&akinator);
    InitAkinator    (&akinator, "");
})

COMMAND (3, GUESS_NODE, "Отгадать объект", {
    GuessNode (&akinator);
})

COMMAND (4, GET_DESCRIPTION, "Получить описание", {
    GetNodeDescription (&akinator);
})

COMMAND (5, COMPARE, "Сравнить два объекта", {
    CompareNodes (&akinator);
})
