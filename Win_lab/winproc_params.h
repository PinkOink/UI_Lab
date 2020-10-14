#ifndef WINPROC_PARAMS_H_INCLUDED
#define WINPROC_PARAMS_H_INCLUDED

//������� ������ ������� (������, ������)
typedef struct
{
    int cxChar;     //������ ������ �������
    int cyChar;     //������ ������ ������� (���������� � ��������� ������)
} SymbolSize_t;

//������� ���� (������, ������)
typedef struct
{
    int cxClient;   //������ ���������� ������� ����
    int cyClient;   //������ ���������� ������� ����
} ClientRectSize_t;

//�������, ����������� � ������� ��� �������� ������������ ���������
typedef struct
{
    int vScrollPos;     //������� �������� ������������ ���������
    int vScrollCoef;    //����������� ������ (i-������� �������� -> i * coef ������ � ������)
    int vScrollMin;     //������ ������� �������� ������������ ���������
    int vScrollMax;     //������� ������� �������� ������������ ���������
} VertScroll_t;

//�������, ����������� � ������� ��� �������� �������������� ���������
typedef struct
{
    int hScrollPos;     //������� �������� �������������� ���������
    int hScrollCoef;    //����������� ������ (i-������� �������� -> i * coef ������ � ������)
    int hScrollMin;     //������ ������� �������� �������������� ���������
    int hScrollMax;     //������� ������� �������� �������������� ���������
} HorzScroll_t;

//������������� ������, ������ ������������ ������, ���������� ����� � ��� ������������� ������ Width_type (� ��������/��� �������)
typedef struct
{
    Text_t *text;               //��������� � �������������� ������
    unsigned long nLongest;     //����� ���������� ������ ������
    unsigned long nLines;       //���������� ����� ������
    Width_type type;            //��� ������������� ������ (� ��������/��� �������)
} TextParams_t;

//��� ��������� ���� ��������� � ������� ��������� �� ������ curLine
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
