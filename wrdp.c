/* Copyright (C) 2025 Andrew Trettel */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "words.h"

void
read_input(InputWord **list)
{
    unsigned long line = 1, column = 1, position = 1;
    int c = getchar();
    while (c != EOF)
    {
        size_t len = 0;
        char *data = NULL;
        while ((isspace(c) == false) && (c != EOF))
        {
            len++;
            column++;
            position++;
            if (data == NULL)
            {
                data = (char *) malloc(len * sizeof(char));
            }
            else
            {
                data = (char *) realloc(data, len);

            }
            if (data == NULL)
            {
                exit(EXIT_FAILURE);
            }
            data[len-1] = (char) c;
            c = getchar();
        }
        if (len > 0)
        {
            len++;
            data = (char *) realloc(data, len);
            if (data == NULL)
            {
                exit(EXIT_FAILURE);
            }
            data[len+1] = '\0';
            append_word(list, data, line, column, position);
        }
        if ((isspace(c) == true) && (isblank(c) == false)) /* Line break? */
        {
            line++;
            column = 1;
            position++;
        }
        else if (isblank(c) == true)
        {
            column++;
            position++;
        }
        c = getchar();
    }
}

int
main(void)
{
    InputWord *list = NULL;
    read_input(&list);
    free_word(list);
    return EXIT_SUCCESS;
}
