#include "text_class.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MessageBoxCode.h"


/*
    Считает количество строк в тексте str
    str - на вход
*/
unsigned long _countLines(char *str)
{
    if (str != NULL)
    {
        unsigned long i = 0;
        unsigned long n = 1;
        while (str[i] != '\0')
        {
            if (str[i++] == '\n')
            {
                n++;
            }
        }
        return n;
    }
    else
    {
        return 0;
    }
}

/*
    Находит позицию символа c в строке str
    str - на вход
    c - на вход
*/
char * _findSymbol(char *str, char c)
{
    if (str != NULL)
    {
        unsigned long i = 0lu;
        while (str[i] != c && str[i] != '\0')
        {
            ++i;
        }

        return (str[i] == c) ? (str + i) : NULL;
    }
    return NULL;
}

/*
    Создает структуру данных с представлением текста, читает текст из файла и возвращает указатель на структуру
    filename - имя файла (на вход)
    msg - куда записать сообщение об ошибке (на выход)
*/
Text_t * GetText(char * filename, MessageBoxCode_t *msg)
{
    if (filename != NULL)
    {
        FILE *f;
        Text_t *text;
        unsigned long i;
        unsigned long n;

        f = fopen(filename, "rb");
        if (f == NULL)
        {
            *msg = MBC_OPEN_FILE;
            return NULL;
        }

        fseek(f, 0, SEEK_END);
        n = (unsigned long)ftell(f);
        rewind(f);

        text = malloc(sizeof(Text_t));
        if (text == NULL)
        {
            *msg = MBC_NO_MEMORY;
            fclose(f);
            return NULL;
        }

        text->type = WIDT_REGULAR;
        text->str = malloc(sizeof(char) * (n + 1));
        if (text->str == NULL)
        {
            *msg = MBC_NO_MEMORY;
            free(text);
            fclose(f);
            return NULL;
        }
        n = fread(text->str, sizeof(char), n, f);
        text->str[n] = '\0';
        fclose(f);

        text->nLines = _countLines(text->str);
        text->pLines = malloc(sizeof(Line_t) * text->nLines);

        if (text->pLines == NULL)
        {
            *msg = MBC_NO_MEMORY;
            free(text->str);
            free(text);
            return NULL;
        }

        text->pLines[0] = text->str;
        for (i = 1; i < text->nLines; ++i)
        {
            text->pLines[i] = _findSymbol(text->pLines[i - 1], '\n') + 1;
            //text->pLines[i - 1].nLine = text->pLines[i].pLine - text->pLines[i - 1].pLine;
        }
        //text->pLines[text->nLines - 1].nLine = _findSymbol(text->pLines[text->nLines - 1].pLine, '\0') - text->pLines[text->nLines - 1].pLine;

        *msg = MBC_NO_ERROR;
        return text;
    }
    else
    {
        *msg = MBC_NO_FILE;
        return NULL;
    }
}

/*
    Возвращает количество строк в тексте
    text - на вход
*/
unsigned long GetLinesNumber(Text_t * text)
{
    if (text != NULL)
    {
        return text->nLines;
    }
    else
    {
        return 0;
    }
}

/*
    Возвращает строку по номеру numString
    text - на вход
    numString - на вход
*/
char * GetString(Text_t * text, unsigned long numString)
{
    if (text != NULL)
    {
        if (numString < text->nLines)
        {
            return text->pLines[numString];
        }
    }

    return NULL;
}

/*
    Возвращает индекс строки, в которой содержится начало строки str
    text - на вход
    str - на вход
*/
unsigned long GetStringIndex(Text_t * text, Line_t str)
{
    if (text != NULL && str != NULL)
    {
    unsigned int i = 0;

    for (i = 0; i < text->nLines && text->pLines[i] <= str; ++i);

    return i - 1;
    }
    return 0;
}

/*
    Возвращает длину строки по номеру numString
    text - на вход
    numString - на вход
*/
unsigned long GetStringLength(Text_t * text, unsigned long numString)
{
    if (text != NULL)
    {
        if (numString == (text->nLines - 1))
        {
            int i = 0;
            while (text->pLines[numString][i++] != '\0');

            return i - 1;
        }
        else
        {
            return text->pLines[numString + 1] - text->pLines[numString];
        }
    }

    return 0;
}

/*
    Возвращает наибольшую длину строки в тексте
    text - на вход
*/
unsigned long GetMaxStringLength(Text_t *text)
{
    if (text != NULL)
    {
        unsigned long max = 0lu;
        unsigned long i;
        for (i = 1; i < text->nLines; ++i)
        {
            if (GetStringLength(text, i) > max)
            {
                max = GetStringLength(text, i);
            }
        }
        return max;
    }
    return 0;
}

/*
    Изменяет способ представления данных (с версткой/без верстки)
    text - указатель на структуру данных с текстом (меняется)
    type - новый тип представления данных (с версткой/без верстки) (на вход)
    width - ширина для представления с версткой (на вход)
*/
int ChangeWidthType(Text_t *text, Width_type type, unsigned long width)
{
    if (text != NULL)
    {
        if (type != text->type)
        {
            unsigned int nLines;
            Line_t *lines;
            unsigned int i;
            int buf;

            switch (type)
            {
            case WIDT_FIXED:
                buf = UpdateWidth(text, width);
                if (buf != 0)
                {
                    return buf;
                }
                text->type = WIDT_FIXED;

                break;
            case WIDT_REGULAR:
                nLines = _countLines(text->str);
                lines = malloc(sizeof(Line_t) * nLines);

                if (lines == NULL)
                {
                    return -1;
                }

                lines[0] = text->str;
                for (i = 1; i < nLines; ++i)
                {
                    lines[i] = _findSymbol(lines[i - 1], '\n') + 1;
                    //lines[i - 1].nLine = lines[i].pLine - lines[i - 1].pLine - 2;
                }
                //lines[nLines - 1].nLine = _findSymbol(lines[nLines - 1].pLine, '\0') - lines[nLines - 1].pLine;

                free(text->pLines);
                text->pLines = lines;
                text->nLines = nLines;
                text->type = WIDT_REGULAR;

                break;
            default:
                break;
            }

            return 0;
        }
    }
    return -1;
}

/*
    Обновляет указатели на строки с учетом новой ширины для представления текста с версткой
    text - указатель на структуру с текстом (меняется)
    width - новое значение ширины (на вход)
*/
int UpdateWidth(Text_t *text, unsigned long width)
{
    if (text != NULL)
    {
        unsigned long nLines = 0;
        unsigned long i = 0;
        unsigned long j;
        unsigned long nLine = 0;

        Line_t *lines;

        while (text->str[i] != '\0')
        {
            nLine++;

            if (nLine == width || text->str[i] == '\n')
            {
                nLines++;
                nLine = 0;
            }

            ++i;
        }
        if (text->str[i] == '\0' && nLine)
        {
            nLines++;
        }
        if (nLines == 0)
        {
            nLines = 1;
        }

        lines = malloc(sizeof(Line_t) * nLines);

        if (lines == NULL)
        {
            return -1;
        }

        i = 0;
        j = 0;
        nLine = 0;
        while (text->str[i] != '\0')
        {
            nLine++;
            if (nLine == width || text->str[i] == '\n')
            {
                lines[j] = &text->str[i + 1 - nLine];
                j++;//lines[j++].nLine = nLine;
                nLine = 0;
            }

            ++i;
        }
        if (text->str[i] == '\0' && nLine)
        {
            lines[j] = &text->str[i - nLine];
            j++;//lines[j++].nLine = nLine;
        }
        if (nLines == 1 && text->str[0] == '\0')
        {
            lines[0] = &text->str[0];
            //lines[0].nLine = 0;
        }

        free(text->pLines);
        text->pLines = lines;
        text->nLines = nLines;

        return 0;
    }
    return -1;
}

/*
    Удаляет структуру с текстом
    text - указатель на структуру с текстом (удаляется)
*/
void DeleteText(Text_t *text)
{
    if (text != NULL)
    {
        free(text->str);
        free(text->pLines);
        free(text);
    }
}
