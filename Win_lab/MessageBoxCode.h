#ifndef MESSAGEBOXCODE_H_INCLUDED
#define MESSAGEBOXCODE_H_INCLUDED

//���� ��������� ��� ����������� ����
typedef enum
{
    MBC_NO_ERROR,       //������ ���
    MBC_NO_FILE,        //���� �� �������� �� ������
    MBC_OPEN_FILE,      //������ �������� �����
    MBC_ABOUT,          //������� ���� About
    MBC_NO_MEMORY       //�������� ������
} MessageBoxCode_t;

#endif // MESSAGEBOXCODE_H_INCLUDED
