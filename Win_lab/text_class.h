#ifndef TEXT_CLASS_H_INCLUDED
#define TEXT_CLASS_H_INCLUDED
#include <stdlib.h>
#include "MessageBoxCode.h"

// оды типы представлени€ данных (с версткой/без верстки)
typedef enum
{
    WIDT_FIXED,     //режим с версткой
    WIDT_REGULAR    //режим без верстки
} Width_type;

typedef char * Line_t;
/*
//ѕредставление строки с ее длиной
typedef struct Line {
    Line_t pLine;           //указатель на начало строки
} String_t;*/

//ѕредставление текста (общий буфер со всем текстом и массив указателей на начала строк), количество строк, размер максимальной строки и тип представлени€ текста (с версткой/без верстки)
typedef struct Text {
    Line_t str;             //ќбща€ строка со всем текстом
    Line_t *pLines;       //ћассив указателей на строки
    unsigned long nLines;   // оличество строк
    Width_type type;        //“ип представлени€ данных (с версткой/без верстки)
} Text_t;

/*
    —оздает структуру данных с представлением текста, читает текст из файла и возвращает указатель на структуру
    filename - им€ файла (на вход)
    msg - куда записать сообщение об ошибке (на выход)
*/
Text_t * GetText(char * file_name, MessageBoxCode_t *msg);

/*
    ¬озвращает количество строк в тексте
    text - на вход
*/
unsigned long GetLinesNumber(Text_t * text);

/*
    ¬озвращает строку по номеру numString
    text - на вход
    numString - на вход
*/
char * GetString(Text_t * text, unsigned long numString);

/*
    ¬озвращает индекс строки, в которой содержитс€ начало строки str
    text - на вход
    str - на вход
*/
unsigned long GetStringIndex(Text_t * text, Line_t str);

/*
    ¬озвращает длину строки по номеру numString
    text - на вход
    numString - на вход
*/
unsigned long GetStringLength(Text_t * text, unsigned long numString);

/*
    ¬озвращает наибольшую длину строки в тексте
    text - на вход
*/
unsigned long GetMaxStringLength(Text_t *text);

/*
    »змен€ет способ представлени€ данных (с версткой/без верстки)
    text - указатель на структуру данных с текстом (мен€етс€)
    type - новый тип представлени€ данных (с версткой/без верстки) (на вход)
    width - ширина дл€ представлени€ с версткой (на вход)
*/
int ChangeWidthType(Text_t *text, Width_type type, unsigned long width);

/*
    ќбновл€ет указатели на строки с учетом новой ширины дл€ представлени€ текста с версткой
    text - указатель на структуру с текстом (мен€етс€)
    width - новое значение ширины (на вход)
*/
int UpdateWidth(Text_t *text, unsigned long width);

/*
    ”дал€ет структуру с текстом
    text - указатель на структуру с текстом (удал€етс€)
*/
void DeleteText(Text_t *text);

#endif
