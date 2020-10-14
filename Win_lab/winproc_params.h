#ifndef WINPROC_PARAMS_H_INCLUDED
#define WINPROC_PARAMS_H_INCLUDED

//Размеры одного символа (ширина, высота)
typedef struct
{
    int cxChar;     //ширина одного символа
    int cyChar;     //высота одного символа (заглавного с пропуском сверху)
} SymbolSize_t;

//Размеры окна (ширина, высота)
typedef struct
{
    int cxClient;   //ширина клиентской области окна
    int cyClient;   //высота клиентской области окна
} ClientRectSize_t;

//Позиция, коэффициент и границы для ползунка вертикальной прокрутки
typedef struct
{
    int vScrollPos;     //позиция ползунка вертикальной прокрутки
    int vScrollCoef;    //коэффициент сдвига (i-позиция ползунка -> i * coef строка в тексте)
    int vScrollMin;     //нижняя граница ползунка вертикальной прокрутки
    int vScrollMax;     //верхняя граница ползунка вертикальной прокрутки
} VertScroll_t;

//Позиция, коэффициент и границы для ползунка горизонтальной прокрутки
typedef struct
{
    int hScrollPos;     //позиция ползунка горизонтальной прокрутки
    int hScrollCoef;    //коэффициент сдвига (i-позиция ползунка -> i * coef строка в тексте)
    int hScrollMin;     //нижняя граница ползунка горизонтальной прокрутки
    int hScrollMax;     //верхняя граница ползунка горизонтальной прокрутки
} HorzScroll_t;

//Представление текста, размер максимальной строки, количество строк и тип представления текста Width_type (с версткой/без верстки)
typedef struct
{
    Text_t *text;               //структура с представлением текста
    unsigned long nLongest;     //длина наибольшей строки текста
    unsigned long nLines;       //количество строк текста
    Width_type type;            //тип представления текста (с версткой/без верстки)
} TextParams_t;

//Все указанные выше структуры и текущее положение по строке curLine
typedef struct
{
    SymbolSize_t SymbolSize;
    ClientRectSize_t ClientRectSize;
    VertScroll_t VertScroll;
    HorzScroll_t HorzScroll;
    TextParams_t TextParams;
    unsigned long curLine;
} WinProcParams_t;

#endif // WINPROC_PARAMS_H_INCLUDED
