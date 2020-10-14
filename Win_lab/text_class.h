#ifndef TEXT_CLASS_H_INCLUDED
#define TEXT_CLASS_H_INCLUDED
#include <stdlib.h>
#include "MessageBoxCode.h"

//���� ���� ������������� ������ (� ��������/��� �������)
typedef enum
{
    WIDT_FIXED,     //����� � ��������
    WIDT_REGULAR    //����� ��� �������
} Width_type;

typedef char * Line_t;
/*
//������������� ������ � �� ������
typedef struct Line {
    Line_t pLine;           //��������� �� ������ ������
} String_t;*/

//������������� ������ (����� ����� �� ���� ������� � ������ ���������� �� ������ �����), ���������� �����, ������ ������������ ������ � ��� ������������� ������ (� ��������/��� �������)
typedef struct Text {
    Line_t str;             //����� ������ �� ���� �������
    Line_t *pLines;       //������ ���������� �� ������
    unsigned long nLines;   //���������� �����
    Width_type type;        //��� ������������� ������ (� ��������/��� �������)
} Text_t;

/*
    ������� ��������� ������ � �������������� ������, ������ ����� �� ����� � ���������� ��������� �� ���������
    filename - ��� ����� (�� ����)
    msg - ���� �������� ��������� �� ������ (�� �����)
*/
Text_t * GetText(char * file_name, MessageBoxCode_t *msg);

/*
    ���������� ���������� ����� � ������
    text - �� ����
*/
unsigned long GetLinesNumber(Text_t * text);

/*
    ���������� ������ �� ������ numString
    text - �� ����
    numString - �� ����
*/
char * GetString(Text_t * text, unsigned long numString);

/*
    ���������� ������ ������, � ������� ���������� ������ ������ str
    text - �� ����
    str - �� ����
*/
unsigned long GetStringIndex(Text_t * text, Line_t str);

/*
    ���������� ����� ������ �� ������ numString
    text - �� ����
    numString - �� ����
*/
unsigned long GetStringLength(Text_t * text, unsigned long numString);

/*
    ���������� ���������� ����� ������ � ������
    text - �� ����
*/
unsigned long GetMaxStringLength(Text_t *text);

/*
    �������� ������ ������������� ������ (� ��������/��� �������)
    text - ��������� �� ��������� ������ � ������� (��������)
    type - ����� ��� ������������� ������ (� ��������/��� �������) (�� ����)
    width - ������ ��� ������������� � �������� (�� ����)
*/
int ChangeWidthType(Text_t *text, Width_type type, unsigned long width);

/*
    ��������� ��������� �� ������ � ������ ����� ������ ��� ������������� ������ � ��������
    text - ��������� �� ��������� � ������� (��������)
    width - ����� �������� ������ (�� ����)
*/
int UpdateWidth(Text_t *text, unsigned long width);

/*
    ������� ��������� � �������
    text - ��������� �� ��������� � ������� (���������)
*/
void DeleteText(Text_t *text);

#endif
