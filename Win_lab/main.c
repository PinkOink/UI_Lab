

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include "text_class.h"
#include "MenuMacros.h"
#include "winproc_params.h"
#include "MessageBoxCode.h"

#define MAX_SCROLL 0xFFFFu

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = "TextReaderWindowsApp";
HWND hwnd;               /* This is the handle for our window */

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;    /* Redraw window when window size or position changes*/
    wincl.cbSize = sizeof (wincl);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);

    wincl.lpszMenuName = "TextReader";
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindow (
               szClassName,         /* Classname */
               "Text Reader",       /* Title Text */
               WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
               CW_USEDEFAULT,       /* x position */
               CW_USEDEFAULT,       /* y position */
               CW_USEDEFAULT,       /* width */
               CW_USEDEFAULT,       /* height */
               HWND_DESKTOP,        /* parent-window */
               NULL,                /* menu */
               hThisInstance,       /* Program Instance handler */
               lpszArgument         /* Command line args */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


//Заполнение структуры со всеми необходимыми параметрами prms
MessageBoxCode_t WM_CREATE_Func(WinProcParams_t *prms, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    TEXTMETRIC tm;
    CREATESTRUCT *pCS;
    HMENU hmenu;
    MessageBoxCode_t msg = MBC_NO_ERROR;

    hdc = GetDC(hwnd);
    SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

    GetTextMetrics(hdc, &tm);
    prms->SymbolSize.cxChar = tm.tmAveCharWidth;
    prms->SymbolSize.cyChar = tm.tmHeight + tm.tmExternalLeading;

    pCS = (CREATESTRUCT *) lParam;
    prms->TextParams.text = GetText((char *) pCS->lpCreateParams, &msg);

    prms->TextParams.nLongest = GetMaxStringLength(prms->TextParams.text);
    prms->TextParams.nLines = GetLinesNumber(prms->TextParams.text);
    prms->TextParams.type = WIDT_REGULAR;

    prms->VertScroll.vScrollMin = 0;
    prms->HorzScroll.hScrollMin = 0;

    prms->curLine = 0;

    if (msg != MBC_NO_ERROR)
    {
        hmenu = GetMenu(hwnd);
        CheckMenuItem(hmenu, IDM_VIEW_FIXED, MF_UNCHECKED);
        CheckMenuItem(hmenu, IDM_VIEW_REGULAR, MF_UNCHECKED);
        EnableMenuItem(hmenu, IDM_VIEW_FIXED, MF_DISABLED);
        EnableMenuItem(hmenu, IDM_VIEW_REGULAR, MF_DISABLED);
    }

    ReleaseDC(hwnd, hdc);

    return msg;
}

MessageBoxCode_t WM_PAINT_Func(WinProcParams_t *prms, HWND hwnd)
{
    HDC hdc;
    PAINTSTRUCT ps;

    int i_start;
    int i_end;
    int y;

    if (prms->TextParams.text != NULL)
    {
    hdc = BeginPaint(hwnd, &ps);

    i_start = prms->curLine;
    i_end = min(prms->TextParams.nLines, i_start + prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar + 1);

    //Отрисовка происходит только в переделах окна, за границами ничего не пытаемся отрисовать
    for (; i_start < i_end; ++i_start)
    {
        if (GetStringLength(prms->TextParams.text, i_start) > prms->HorzScroll.hScrollPos * prms->HorzScroll.hScrollCoef)
        {
            y = prms->SymbolSize.cyChar * (i_start - prms->curLine);
            TextOut(hdc, 0, y,
                    GetString(prms->TextParams.text, i_start) + prms->HorzScroll.hScrollPos * prms->HorzScroll.hScrollCoef,
                    min(prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar + 1, GetStringLength(prms->TextParams.text, i_start) - prms->HorzScroll.hScrollPos * prms->HorzScroll.hScrollCoef));
        }
    }

    EndPaint(hwnd, &ps);
    }

    return MBC_NO_ERROR;
}

MessageBoxCode_t WM_SIZE_Func(WinProcParams_t *prms, unsigned int height, unsigned int width)
{
    if (prms->TextParams.text != NULL)
    {
    int buf;
    Line_t buf_str = GetString(prms->TextParams.text, prms->curLine);

    prms->ClientRectSize.cxClient = width;
    prms->ClientRectSize.cyClient = height;

    //Если задан режим с версткой, обновляем представление текста
    if (prms->TextParams.type == WIDT_FIXED)
    {
        if ((unsigned int)prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar != prms->TextParams.nLongest)
        {
            buf = UpdateWidth(prms->TextParams.text, (unsigned int)prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar);
            if (buf != 0)
            {
                return MBC_NO_MEMORY;
            }
        }

        prms->TextParams.nLongest = GetMaxStringLength(prms->TextParams.text);
        prms->TextParams.nLines = GetLinesNumber(prms->TextParams.text);
    }

    //Текущая позиция не должна выходит за границы текста
    prms->curLine = min(GetStringIndex(prms->TextParams.text, buf_str), (prms->TextParams.nLines < prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar) ? 0 : (prms->TextParams.nLines - prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar));

    //Обновляем коэффициент сдивга и диапазон вертикальной прокрутки
    if (((prms->TextParams.nLines < prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar) ? 0 : (prms->TextParams.nLines - prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar)) < MAX_SCROLL)
    {
        prms->VertScroll.vScrollMax = (prms->TextParams.nLines < prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar) ? 0 : (prms->TextParams.nLines - prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar);
        prms->VertScroll.vScrollCoef = 1;
    }
    else
    {
        prms->VertScroll.vScrollCoef = (prms->TextParams.nLines - prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar + 1) / (MAX_SCROLL) + (((prms->TextParams.nLines - prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar + 1) % (MAX_SCROLL)) ? 1 : 0);
        prms->VertScroll.vScrollMax = (prms->TextParams.nLines - prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar + 1) / prms->VertScroll.vScrollCoef + (((prms->TextParams.nLines - prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar + 1) % prms->VertScroll.vScrollCoef) ? 1 : 0);
    }

    //Обновляем коэффициент сдивга и диапазон горизонтальной прокрутки
    if (((prms->TextParams.nLongest < prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar) ? 0 : (prms->TextParams.nLongest - prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar)) < MAX_SCROLL)
    {
        prms->HorzScroll.hScrollMax = (prms->TextParams.nLongest < prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar) ? 0 : (prms->TextParams.nLongest - prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar);
        prms->HorzScroll.hScrollCoef = 1;
    }
    else
    {
        prms->HorzScroll.hScrollCoef = (prms->TextParams.nLongest - prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar + 1) / (MAX_SCROLL) + (((prms->TextParams.nLongest - prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar + 1) % (MAX_SCROLL)) ? 1 : 0);
        prms->HorzScroll.hScrollMax = (prms->TextParams.nLongest - prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar + 1) / prms->HorzScroll.hScrollCoef + (((prms->TextParams.nLongest - prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar + 1) % prms->HorzScroll.hScrollCoef) ? 1 : 0);
    }

    //Согласуем позицию ползунка вертикальной прокрутки с текущей позицией по строке
    prms->VertScroll.vScrollPos = prms->curLine / prms->VertScroll.vScrollCoef;


    prms->HorzScroll.hScrollPos = min(prms->HorzScroll.hScrollPos, prms->HorzScroll.hScrollMax);

    SetScrollRange(hwnd, SB_VERT, prms->VertScroll.vScrollMin, prms->VertScroll.vScrollMax, FALSE);
    SetScrollPos(hwnd, SB_VERT, prms->VertScroll.vScrollPos, TRUE);
    SetScrollRange(hwnd, SB_HORZ, prms->HorzScroll.hScrollMin, prms->HorzScroll.hScrollMax, FALSE);
    SetScrollPos(hwnd, SB_HORZ, prms->HorzScroll.hScrollPos, TRUE);
    }

    return MBC_NO_ERROR;
}

MessageBoxCode_t WM_VSCROLL_Func(WinProcParams_t *prms, HWND hwnd, WPARAM wParam)
{
    int iScrollChange;

    //Узнаем смещение
    switch(LOWORD(wParam))
    {
    case SB_TOP :
        iScrollChange = -prms->VertScroll.vScrollPos;
        break;
    case SB_BOTTOM :
        iScrollChange = prms->VertScroll.vScrollMax - prms->VertScroll.vScrollPos;
        break;
    case SB_LINEUP :
        iScrollChange = -1;
        break;
    case SB_LINEDOWN :
        iScrollChange = 1;
        break;
    case SB_PAGEUP :
        iScrollChange = min(-1, -prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar);
        break;
    case SB_PAGEDOWN :
        iScrollChange = max(1, prms->ClientRectSize.cyClient / prms->SymbolSize.cyChar);
        break;
    case SB_THUMBTRACK :
        iScrollChange = HIWORD(wParam) - prms->VertScroll.vScrollPos;
        break;
    default :
        iScrollChange = 0;
        break;
    }

    iScrollChange = max(-prms->VertScroll.vScrollPos, min(iScrollChange, prms->VertScroll.vScrollMax - prms->VertScroll.vScrollPos));

    //Если сместились от исходной позиции, сдвигаем внутреннюю область окна
    //Если позиция не кратна коэффициенту сдвига, чиним
    if (iScrollChange)
    {
        prms->VertScroll.vScrollPos += iScrollChange;
        ScrollWindow(hwnd, 0, -prms->SymbolSize.cyChar * (prms->VertScroll.vScrollPos * prms->VertScroll.vScrollCoef - prms->curLine), NULL, NULL);
        SetScrollPos(hwnd, SB_VERT, prms->VertScroll.vScrollPos, TRUE);
        prms->curLine = prms->VertScroll.vScrollPos * prms->VertScroll.vScrollCoef;
        UpdateWindow(hwnd);
    }

    return MBC_NO_ERROR;
}

MessageBoxCode_t WM_HSCROLL_Func(WinProcParams_t *prms, HWND hwnd, WPARAM wParam)
{
    int iScrollChange;

    //Узнаем смещение
    switch(LOWORD(wParam))
    {
    case SB_TOP :
        iScrollChange = -prms->HorzScroll.hScrollPos;
        break;
    case SB_BOTTOM :
        iScrollChange = prms->HorzScroll.hScrollMax - prms->HorzScroll.hScrollPos;
        break;
    case SB_LINEUP :
        iScrollChange = -1;
        break;
    case SB_LINEDOWN :
        iScrollChange = 1;
        break;
    case SB_PAGEUP :
        iScrollChange = min(-1, -prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar);
        break;
    case SB_PAGEDOWN :
        iScrollChange = max(1, prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar);
        break;
    case SB_THUMBTRACK :
        iScrollChange = HIWORD(wParam) - prms->HorzScroll.hScrollPos;
        break;
    default :
        iScrollChange = 0;
        break;
    }

    iScrollChange = max(-prms->HorzScroll.hScrollPos, min(iScrollChange, prms->HorzScroll.hScrollMax - prms->HorzScroll.hScrollPos));

    //Если сместились от исходной позиции, сдвигаем внутреннюю область окна
    if (iScrollChange)
    {
        prms->HorzScroll.hScrollPos += iScrollChange;
        ScrollWindow(hwnd, -prms->SymbolSize.cxChar * iScrollChange, 0, NULL, NULL);
        SetScrollPos(hwnd, SB_HORZ, prms->HorzScroll.hScrollPos, TRUE);
        UpdateWindow(hwnd);
    }

    return MBC_NO_ERROR;
}

MessageBoxCode_t WM_COMMAND_Func(WinProcParams_t *prms, HWND hwnd, WPARAM wParam)
{
    HMENU hmenu;
    RECT rc;
    int buf;
    Line_t buf_str;
    OPENFILENAME ofn;
    char charbuf[256];
    MessageBoxCode_t msg = MBC_NO_ERROR;

    switch (LOWORD(wParam))
    {
    //Открываем окно-проводник и предлагаем пользователю выбрать существующий .txt файл
    case IDM_FILE_OPEN:
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = charbuf;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(charbuf);
        ofn.lpstrFilter = "Text\0*.txt\0\0";
        ofn.nFilterIndex = 1;
//        ofn.lpstrFileTitle = NULL;
//        ofn.nMaxFileTitle = 0;
//        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE)
        {
            DeleteText(prms->TextParams.text);
            prms->TextParams.text = GetText(ofn.lpstrFile, &msg);

            prms->TextParams.nLongest = GetMaxStringLength(prms->TextParams.text);
            prms->TextParams.nLines = GetLinesNumber(prms->TextParams.text);
            prms->TextParams.type = WIDT_REGULAR;

            prms->curLine = 0;

            hmenu = GetMenu(hwnd);
            if (prms->TextParams.text != NULL)
            {
                CheckMenuItem(hmenu, IDM_VIEW_FIXED, MF_UNCHECKED);
                CheckMenuItem(hmenu, IDM_VIEW_REGULAR, MF_CHECKED);
                EnableMenuItem(hmenu, IDM_VIEW_FIXED, MF_ENABLED);
                EnableMenuItem(hmenu, IDM_VIEW_REGULAR, MF_DISABLED);
            }
            else
            {
                CheckMenuItem(hmenu, IDM_VIEW_FIXED, MF_UNCHECKED);
                CheckMenuItem(hmenu, IDM_VIEW_REGULAR, MF_UNCHECKED);
                EnableMenuItem(hmenu, IDM_VIEW_FIXED, MF_DISABLED);
                EnableMenuItem(hmenu, IDM_VIEW_REGULAR, MF_DISABLED);
            }

            GetClientRect(hwnd, &rc);
            InvalidateRect(hwnd, &rc, TRUE);
            WM_SIZE_Func(prms, rc.bottom, rc.right);

            UpdateWindow(hwnd);
        }


        break;
    //Заканчиваем работу приложения
    case IDM_FILE_EXIT:
        SendMessage(hwnd, WM_DESTROY, 0, 0);

        break;
    //Меняем режим представления на режим с версткой. Обновляем представление данных текста, обновляем поля, связанные с прокруткой и перересовываем область
    case IDM_VIEW_FIXED:
        buf_str = GetString(prms->TextParams.text, prms->curLine);
        buf = ChangeWidthType(prms->TextParams.text, WIDT_FIXED, (unsigned long)prms->ClientRectSize.cxClient / prms->SymbolSize.cxChar);
        if (buf != 0)
        {
            msg =  MBC_NO_MEMORY;
        }
        prms->TextParams.nLongest = GetMaxStringLength(prms->TextParams.text);
        prms->TextParams.nLines = GetLinesNumber(prms->TextParams.text);

        prms->curLine = GetStringIndex(prms->TextParams.text, buf_str);

        prms->TextParams.type = WIDT_FIXED;

        WM_SIZE_Func(prms, prms->ClientRectSize.cyClient, prms->ClientRectSize.cxClient);

        GetClientRect(hwnd, &rc);
        InvalidateRect(hwnd, &rc, TRUE);

        hmenu = GetMenu(hwnd);
        if (prms->TextParams.text != NULL)
        {
            CheckMenuItem(hmenu, IDM_VIEW_FIXED, MF_CHECKED);
            CheckMenuItem(hmenu, IDM_VIEW_REGULAR, MF_UNCHECKED);
            EnableMenuItem(hmenu, IDM_VIEW_FIXED, MF_DISABLED);
            EnableMenuItem(hmenu, IDM_VIEW_REGULAR, MF_ENABLED);
        }
        else
        {
            CheckMenuItem(hmenu, IDM_VIEW_FIXED, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_VIEW_REGULAR, MF_UNCHECKED);
            EnableMenuItem(hmenu, IDM_VIEW_FIXED, MF_DISABLED);
            EnableMenuItem(hmenu, IDM_VIEW_REGULAR, MF_DISABLED);
        }

        break;
    //Меняем режим представления на режим без верстки. Обновляем представление данных текста, обновляем поля, связанные с прокруткой и перересовываем область
    case IDM_VIEW_REGULAR:
        buf_str = GetString(prms->TextParams.text, prms->curLine);
        buf = ChangeWidthType(prms->TextParams.text, WIDT_REGULAR, 0);
        if (buf != 0)
        {
            msg = MBC_NO_MEMORY;
        }
        prms->TextParams.type = WIDT_REGULAR;

        prms->TextParams.nLongest = GetMaxStringLength(prms->TextParams.text);
        prms->TextParams.nLines = GetLinesNumber(prms->TextParams.text);

        prms->curLine = GetStringIndex(prms->TextParams.text, buf_str);

        WM_SIZE_Func(prms, prms->ClientRectSize.cyClient, prms->ClientRectSize.cxClient);

        GetClientRect(hwnd, &rc);
        InvalidateRect(hwnd, &rc, TRUE);

        hmenu = GetMenu(hwnd);
        if (prms->TextParams.text != NULL)
        {
            CheckMenuItem(hmenu, IDM_VIEW_FIXED, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_VIEW_REGULAR, MF_CHECKED);
            EnableMenuItem(hmenu, IDM_VIEW_FIXED, MF_ENABLED);
            EnableMenuItem(hmenu, IDM_VIEW_REGULAR, MF_DISABLED);
        }
        else
        {
            CheckMenuItem(hmenu, IDM_VIEW_FIXED, MF_UNCHECKED);
            CheckMenuItem(hmenu, IDM_VIEW_REGULAR, MF_UNCHECKED);
            EnableMenuItem(hmenu, IDM_VIEW_FIXED, MF_DISABLED);
            EnableMenuItem(hmenu, IDM_VIEW_REGULAR, MF_DISABLED);
        }

        break;
    //Вызов окна About
    case IDM_HELP_ABOUT:
        msg = MBC_ABOUT;

        break;
    default:
        break;
    }

    return msg;
}

MessageBoxCode_t WM_KEYDOWN_Func(WinProcParams_t *prms, HWND hwnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case VK_HOME:
        SendMessage(hwnd, WM_HSCROLL, SB_TOP, 0L);

        break;
    case VK_END:
        SendMessage(hwnd, WM_HSCROLL, SB_BOTTOM, 0L);

        break;
    case VK_PRIOR:
        SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0L);

        break;
    case VK_NEXT:
        SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0L);

        break;
    case VK_UP:
        SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0L);

        break;
    case VK_DOWN:
        SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0L);

        break;
    case VK_LEFT:
        SendMessage(hwnd, WM_HSCROLL, SB_LINEUP, 0L);

        break;
    case VK_RIGHT:
        SendMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, 0L);

        break;
    default:
        break;
    }

    return MBC_NO_ERROR;
}

//Вывод диаологового окна
void ShowMessageBox(HWND hwnd, MessageBoxCode_t code)
{
    switch (code)
    {
    case MBC_NO_MEMORY:
        MessageBox(hwnd, "Not enough memory to proceed", NULL, MB_OK | MB_ICONHAND | MB_APPLMODAL);
        break;
    case MBC_OPEN_FILE:
        MessageBox(hwnd, "File can't be opened", NULL, MB_OK | MB_ICONHAND | MB_APPLMODAL);
        break;
    case MBC_ABOUT:
        MessageBox(hwnd, "Created in 2019", "About...", MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);
        break;
    default:
        break;
    }
}

LRESULT CALLBACK WindowProcedure (HWND hwndin, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WinProcParams_t prms;
    MessageBoxCode_t mbc;
    hwnd = hwndin;
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
        mbc = WM_CREATE_Func(&prms, hwnd, wParam, lParam);
        if (mbc != MBC_NO_ERROR)
        {
            ShowMessageBox(hwnd, mbc);
        }

        break;
    case WM_PAINT:
        mbc = WM_PAINT_Func(&prms, hwnd);
        if (mbc != MBC_NO_ERROR)
        {
            ShowMessageBox(hwnd, mbc);
        }

        break;
    case WM_SIZE:
        mbc = WM_SIZE_Func(&prms, HIWORD(lParam), LOWORD(lParam));
        if (mbc != MBC_NO_ERROR)
        {
            ShowMessageBox(hwnd, mbc);
        }

        break;
    case WM_VSCROLL:
        mbc = WM_VSCROLL_Func(&prms, hwnd, wParam);
        if (mbc != MBC_NO_ERROR)
        {
            ShowMessageBox(hwnd, mbc);
        }

        break;
    case WM_HSCROLL:
        mbc = WM_HSCROLL_Func(&prms, hwnd, wParam);
        if (mbc != MBC_NO_ERROR)
        {
            ShowMessageBox(hwnd, mbc);
        }

        break;
    case WM_COMMAND:
        mbc = WM_COMMAND_Func(&prms, hwnd, wParam);
        if (mbc != MBC_NO_ERROR)
        {
            MessageBox(hwnd, "Check", "Check", MB_OK | MB_ICONINFORMATION);
            ShowMessageBox(hwnd, mbc);
        }

        break;
    case WM_KEYDOWN:
        mbc = WM_KEYDOWN_Func(&prms, hwnd, wParam);
        if (mbc != MBC_NO_ERROR)
        {
            ShowMessageBox(hwnd, mbc);
        }

        break;
    case WM_DESTROY:
        DeleteText(prms.TextParams.text);
        PostQuitMessage(0);

        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
