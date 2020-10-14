#ifndef MESSAGEBOXCODE_H_INCLUDED
#define MESSAGEBOXCODE_H_INCLUDED

//Коды сообщений для диалогового окна
typedef enum
{
    MBC_NO_ERROR,       //ошибок нет
    MBC_NO_FILE,        //файл на открытие не указан
    MBC_OPEN_FILE,      //ошибка открытия файла
    MBC_ABOUT,          //открыть окно About
    MBC_NO_MEMORY       //нехватка памяти
} MessageBoxCode_t;

#endif // MESSAGEBOXCODE_H_INCLUDED
