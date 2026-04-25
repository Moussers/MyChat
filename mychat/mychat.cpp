// mychat.cpp : Определяет точку входа для приложения.

#include "framework.h"
#include "mychat.h"
#include "winsock2.h"               //Бибилиотека для работы с сетью
#define MAX_LOADSTRING 100
#define MAIN_WINDOW_POSITION_X 820
#define MAIN_WINDOW_POSITION_Y 580
#define MES_FIELD_X 10
#define MES_FIELD_Y 460
#define MES_FIELD_WIDTH 760
#define MES_FIELD_HEIGHT 20
#define INFO_FIELD_WIDTH 790
#define INFO_FIELD_HEIGT 440
#define INFO_FIELD_POS_X 5
#define INFO_FIELD_POS_Y 10
#define SEND_MES_WINDOW_X 20
#define SEND_MES_WINDOW_Y 490
#define SEND_MES_WINDOW_WIDTH 100
#define SEND_MES_WINDOW_HEIGHT 20
#define MAIN_FIELD_WIDTH 270
#define MAIN_FIELD_HEIGHT 300
#define DESCRIPT_FIELD_POS_X 20
#define DESCRIPT_FIELD_WIDTH 100
#define DESCRIPT_FIELD_HEIGHT 150
#define INPUT_FIELD_POS_X 100
#define COUNT_FIELD_POS_X(x) x+30
//Синтаксис псевдо функции через добавить define
#define INPUT_FIELD_WIDTH 140
#define INPUT_FIELD_HEIGHT 20
#define CANCEL_BUTTON_POS_X 160
#define CANCEL_BUTTON_POS_Y 200
#define CANCEL_BUTTON_WIDTH 70
#define CANCEL_BUTTON_HEIGH 20
#define DEAL_BUTTON_POS_X 100
#define DEAL_BUTTON_POS_Y 200
#define DEAL_BUTTON_WIDTH 45
#define DEAL_BUTTON_HEIGHT 20
CONST WCHAR USER_LIST_CLASS_NAME[] = L"UserListWindow";
CONST WCHAR USER_ACCOUNT_CLASS_NAME[] = L"AddingUserAccount";
CONST INT USERSIZE = 2000;
CONST INT IDSIZE = 1000;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int InsertEntry(HWND hwnd);
int checkTables();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYCHAT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    checkTables();
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYCHAT));
    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
LRESULT CALLBACK AddNewUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //LRESULT CALLBACK - это структура меню для созданного окна. Команды внутри окна
    //выполняются через switch(message)
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD (wParam)) 
        {
        case IDB_CANCELING_USER_ADDITION:
            SendMessage(hWnd, WM_CLOSE, 0, NULL);
            break;
        case IDB_DEALIGN_USER_ADDITION: 
        {
            InsertEntry(hWnd);
            SendMessage(hWnd, WM_CLOSE, 0, NULL);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    break;   
    case WM_DESTROY:
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void writtingDownLog(const WCHAR* record) 
{
    HANDLE logFile = CreateFile(L"log.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    //dwDesiredAccess - предоставляет атрибуты, иначе говоря права доступа на чтение и запись файлу.
    //dwShareMode - запрещает повторно открывать файл.
    //lpSecurityAttributes - отвечает за настройки безопасности файла, если поставить NULL, тогда 
    //применяются стандартные настройки безопасности к файлу при его создании.
    //dwCreationDisposition - флаг за тип взаимодействия с файлом: создания или открытия:
    //CREATE_NEW - создание нового файла;
    //OPEN_EXISTS - открыть существующий файл.
    //dwFlagsAndAttributes - предоставляает права программе для взаимодействия с файлом, обычно 
    //используется флаг FILE_ATTRIBUTE_NORMAL, дающий стандартные права для чтения и записи программе 
    //и ничего дополнительного.
    //hTemplateFile - работа с шифрованным файлом: чтение и запись, при созддании временног файла.
    if (logFile == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_EXISTS)
        {
            logFile = CreateFile(L"log.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        }
    }
    if (logFile != INVALID_HANDLE_VALUE)
    {
        SetFilePointer(logFile, 0, NULL, FILE_END);
        WriteFile(logFile, record, wcslen(record)*2, NULL, NULL);
        WriteFile(logFile, L"\n", 2, NULL, NULL);
        CloseHandle(logFile);
    }
}
void setDash(INT &startPos, INT endPos, CHAR* sendArr, CHAR* recArr, INT &shiftIn)
{
    CONST INT SIZE = 2000;
    while (startPos < endPos)
    {
        recArr[startPos + shiftIn] = sendArr[startPos];
        startPos++;
    }
    if (recArr[startPos + shiftIn] != '\0') 
    {
        recArr[startPos + shiftIn] = '\0';
    }
    if (sendArr[startPos] == '-')
    {
        
        recArr[startPos + shiftIn] = sendArr[startPos];
        startPos++;
    }
    else
    {
        strcat_s(recArr, SIZE, "-");
        shiftIn++;
    }
}

INT checkingEMail(CHAR* eMail) 
{
    int numAt = 0;
    INT len = strlen(eMail);
    for (int i = 0; i < len; ++i) 
    {
        if (eMail[i] == '@') 
        {
            numAt++;
        }
        if (((eMail[i] < '0') || (eMail[i] > '9')) && ((eMail[i] < 'A') || (eMail[i] > 'Z') )
            && ((eMail[i] < 'a') || (eMail[i] > 'z')) && (eMail[i] != '.') && (eMail[i] != '_')
            && (eMail[i] != '-') && (eMail[i] != '@'))
        {
            MessageBox(NULL, L"Неверный формат почты", L"Ошибка", MB_OK | MB_ICONERROR);
            return 1;
        }
    }
    if (numAt > 1) 
    {
        MessageBox(NULL, L"Должен быть только один знак @!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    else if (numAt == 0) 
    {
        MessageBox(NULL, L"Не обнаружен знак @!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}
INT checkingNumberPhone(CHAR* strPhone, CHAR* buffer) 
{
    INT codePage = 1251;
    INT numberCharacters = 0;
    CONST INT SIZE = 2000;
    WCHAR str[SIZE]{};
    INT startPosition = 0;
    if (strPhone == NULL)
    {
        MessageBox(NULL, L"Строка  являеться не определенной", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    if (strcmp(strPhone, "") == 0) 
    {
        MessageBox(NULL, L"Строка являеться пустой", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    INT len = static_cast<INT>(strlen(strPhone));
    if(len < 13)
    {
        MessageBox(NULL, L"Размер номера не соотвествует стандартному номеру", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    else 
    {
        int i = 0;
        if (strPhone[0] == '+') 
        {
            
            WCHAR tmp[SIZE];
            MultiByteToWideChar(codePage, 0, strPhone, strlen(strPhone) + 1, tmp, SIZE);
            wcscat_s(str, tmp);
            startPosition++;
        }
        else
        {
            if (iswdigit(strPhone[0]) != 0)
            {
                wsprintf(str, L"%s", L"");
                wcscat_s(str, L"+");
                WCHAR tmp[SIZE];
                MultiByteToWideChar(1251, 0, strPhone, strlen(strPhone) + 1, tmp, SIZE);
                wcscat_s(str, tmp);
                startPosition++;
            }
        }
        INT posOpenParet = -1;
        INT posCloseParet = -1;
        len = static_cast<int>(wcslen(str));
        for (int i = startPosition; i < len; ++i) 
        {
        if ((str[i] < L'0') || (str[i] > '9'))
        {
            if (str[i] != L'-' && str[i] != L'(' && str[i] != L')')
            {
                MessageBox(NULL, L"Неверный формат телефона", L"Ошибка", MB_OK | MB_ICONERROR);
                return 1;
            }
        }
        if (str[i] == L'(')
        {
            posOpenParet = i;
        }
        if (str[i] == L')')
        {
            posCloseParet = i;
        }
        }
        if (posOpenParet == -1 || posOpenParet == len-1) 
        {
            MessageBox(NULL, L"Не обнаружена открывающая скобка\nили она находится в конце строки", L"Ошибка", MB_OK | MB_ICONERROR);
            return 1;
        }
        if (posCloseParet == -1 || posCloseParet == len-1) 
        {
            MessageBox(NULL, L"Не обнаружена закрывающая скобка \nили она находится в конце строки!", L"Ошибка", MB_OK | MB_ICONERROR);
            return 1;
        }
        if (posOpenParet > posCloseParet) 
        {
            MessageBox(NULL, L"Открывающая скобка находится\nпосле закрывающей скобки!", L"Ошибка", MB_OK | MB_ICONERROR);
            return 1;
        }
        if (posOpenParet == 0) 
        {
            MessageBox(NULL, L"Код страны не ввидён!", L"Ошибка", MB_OK | MB_ICONERROR);
            return 1;
        }
        INT d = 0;
        INT c = 0;
        while(d <= posCloseParet)
        {
            buffer[d] = strPhone[d];
            d++;
        }
        INT shiftIn = 0;
        setDash(d, d + 3, strPhone, buffer, shiftIn);
        setDash(d, d + 2, strPhone, buffer, shiftIn);      
        c = d + 2;
        while (d < c)
        {
            buffer[d+shiftIn] = strPhone[d];
            d++;
        }
        if (buffer[d + shiftIn] != '\0')
        {
            buffer[d + shiftIn] = '\0';
        }
    }
    return 0;
}
char* cleaningMemory(char* arr) 
{
    if (arr != NULL)
    {
        free(arr);
        arr = NULL;
        return arr;
    }
    return arr;
}
int UpdateList(HWND userList) 
{
    INT codePage = 1251;
    SendMessage(userList, LB_RESETCONTENT, 0, 0);
    //LB_RESETCONTENT - очищает данные дексиптора, который является listBox.
    sqlite3* db;
    int result = sqlite3_open("DatabaseMessanger.db", &db);
    if (result) 
    //if result > 0
    {
        MessageBox(NULL, L"База данных не подключена", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_stmt* userTb;
    const char* getUserList = "SELECT * FROM users";
    if (sqlite3_prepare_v2(db, getUserList, -1, &userTb, NULL) == SQLITE_OK) 
    {
        int nextRow;
        while ((nextRow = sqlite3_step(userTb)) == SQLITE_ROW) 
        {
            int id = sqlite3_column_int(userTb, 0);
            //sqlite3_column_int - достает int значение из массива stmt по указанной ячейке
            CONST INT SIZE = 2000;
            WCHAR firstName[SIZE];
            WCHAR lastName[SIZE];
            const char* charFirstName = reinterpret_cast<const char*>(sqlite3_column_text(userTb, 1));
            const char* charLastName = reinterpret_cast<const char*>(sqlite3_column_text(userTb, 2));
            MultiByteToWideChar(codePage, 0, (char*)charFirstName, strlen((char*)charFirstName)+1, firstName, SIZE);
            MultiByteToWideChar(codePage, 0, (char*)charLastName, strlen((char*)charLastName) + 1, lastName, SIZE);
            WCHAR toList[SIZE];
            wcscpy_s(toList, lastName);
            //wcscpy - перезаписывает данные из одного массива в другой, 
            //в последнем стирая прежние данные замещая новыми
            wcscat_s(toList, L" ");
            wcscat_s(toList, firstName);
            SendMessage(userList, LB_ADDSTRING, 0, (LPARAM)toList);
            //LB_ADDSTRING - добавить строку в lixtBox.
        }
    }
    sqlite3_close(db);
    return 0;
}
int InsertEntry(HWND hwnd)
{
    WCHAR lastName[USERSIZE];
    WCHAR firstName[USERSIZE];
    WCHAR middleName[USERSIZE];
    WCHAR numbrerPhone[USERSIZE];
    WCHAR eMail[USERSIZE];
    WCHAR userId[IDSIZE];
    CONST INT SIZECOMMAND = 12000;
    CHAR command[SIZECOMMAND];
    CHAR buffer[USERSIZE];
    INT num = 0;
    INT status = 0;
    UINT codePage = 1251;
    LPCTSTR errMes;
    //UINT - безнаковый целочисленный тип числа
    GetWindowText(GetDlgItem(hwnd, IDM_LAST_NAME), lastName, USERSIZE);
    //GetWindowText - функция которая копирует строку из дескриптора окна в переменную,
    //с размером который мы указываем в поле buffer, последний параметр
    GetWindowText(GetDlgItem(hwnd, IDM_FIRST_NAME), firstName, USERSIZE);
    GetWindowText(GetDlgItem(hwnd, IDM_MIDDLE_NAME), middleName, USERSIZE);
    GetWindowText(GetDlgItem(hwnd, IDM_PHONE), numbrerPhone, USERSIZE);
    GetWindowText(GetDlgItem(hwnd, IDM_EMAIL), eMail, USERSIZE);
    //MessageBox(NULL, lastName, L"INFO", MB_OK | MB_ICONERROR);
    //MessageBox(NULL, firstName, L"INFO", MB_OK | MB_ICONERROR);
    const char lsUsrId[] = "SELECT MAX(USER_ID) FROM users";
    sqlite3* db;
    int res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res)
    {
        MessageBox(NULL, L"Ошибка подключения к базе данных", L"Информация", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_stmt* table;
    if (sqlite3_prepare_v2(db, lsUsrId, -1, &table, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(table);
        if (curRow == SQLITE_ROW)
        {
            num = sqlite3_column_int(table, 0) + 1;
            //+1 - получаем следующий id;
        }
    }
    sqlite3_finalize(table);
    strcpy_s(command, "INSERT INTO users (user_id, last_name, first_name, middle_name, phone, email, status) VALUES(");
    wsprintf(userId, L"%d\0", num);
    WideCharToMultiByte(codePage, 0, userId, IDSIZE + 1, buffer, USERSIZE, NULL, NULL);
    //CodePage (кодовая страница) - отвечает за хранение тип формата в который будет приобразована строка, 
    //в данный момент из Unicode в формат: 1251;
    //dwFlags - флаг правила преобразования формата кодировки;
    //LpWideChar - указатель на строку для преобразования;
    //cchWideChar - буфер (размер) строки;
    //LpMultiByteStr - указатель на строку в которую будет записана преобразованная строка;
    //cbMultiByte - размер буффера строки для записи;
    //lpDefaultChar - указатель на символ для преобразования, если он не указан в 
    //представленной таблице;
    //lpUsedDefaultChar - указатель для нескольких символов, если они не указаны в
    //в представленной таблице.
    strcat_s(command, buffer);
    strcat_s(command, ",");
    strcat_s(command, "'");
    WideCharToMultiByte(codePage, 0, lastName, USERSIZE + 1, buffer, USERSIZE, NULL, NULL);
    strcat_s(command, buffer);
    strcat_s(command, "'");
    strcat_s(command, ",");
    strcat_s(command, "'");
    WideCharToMultiByte(codePage, 0, firstName, USERSIZE + 1, buffer, USERSIZE, NULL, NULL);
    strcat_s(command, buffer);
    strcat_s(command, "'");
    strcat_s(command, ",");
    strcat_s(command, "'");
    WideCharToMultiByte(codePage, 0, middleName, USERSIZE + 1, buffer, USERSIZE, NULL, NULL);
    strcat_s(command, buffer);
    strcat_s(command, "'");
    strcat_s(command, ",");
    strcat_s(command, "'");
    WideCharToMultiByte(codePage, 0, numbrerPhone, USERSIZE + 1, buffer, USERSIZE, NULL, NULL);
    CONST INT SIZE = 2000;
    CHAR temp[SIZE]{};
    if (checkingNumberPhone(buffer, temp) == 1)
    {
        return 1;
    }
    strcat_s(command, temp);
    strcat_s(command, "'");
    strcat_s(command, ",");
    strcat_s(command, "'");
    WideCharToMultiByte(codePage, 0, eMail, USERSIZE + 1, buffer, USERSIZE, NULL, NULL);
    if (checkingEMail(buffer) == 1) 
    {
        return 1;
    }
    strcat_s(command, buffer);
    strcat_s(command, "'");
    strcat_s(command, ",");
    wsprintfA(buffer, "%d", status);
    //wsprintfA - записывает в переменную идущую первым аргументом в формате ANSI.
    strcat_s(command, buffer);
    strcat_s(command, ")");
    char *msg = NULL;
    try {
        res = sqlite3_exec(db, command, NULL, NULL, &msg);
        //5 параметр - сам выделяет память и создает массив без участия программиста.
        if (res == SQLITE_OK)
        {
            MessageBox(NULL, L"Пользователь добавлен", L"Инфо", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBox(NULL, L"Ошибка добавления пользователя", L"Ошибка", MB_OK | MB_ICONERROR);
            throw "SQL-Error";
        }
    }
    catch (...) 
    {
        CONST INT SIZE = 2000;
        WCHAR mesError[SIZE];
        size_t n_size;
        mbstowcs_s(&n_size,mesError, msg, SIZE);
        //mbstowcs_s - преобразует многобайтовую символьную строку из массива, в расширенное 
        //символьное представление (WCHAR);
        //pReturnValue / retval - указатель на тип данных в котором будут храниться данные, 
        //то есть размер и тип буфера;
        //dst - указатель на переменную в которую будут записаны преобразованные данные;
        //src - указатель на переменную источник с которой будут считаны данные для 
        //преобразования;
        //len - указатель на буффер (размер) строки источник.
        msg = cleaningMemory(msg);
        writtingDownLog(mesError);
        return 1;
    }
    msg = cleaningMemory(msg);
    sqlite3_close(db);
    return 0;
}
int deleteUser(int idx) 
{
    if (idx == -1) 
    {
        return 1;
    }
    INT codePage = 1251;
    sqlite3* db;
    INT result = sqlite3_open("DatabaseMessanger.db", &db);
    if (result)
    {
        MessageBox(NULL, L"База данных не подключена", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    const char* selectIdUser = "SELECT user_id FROM user LIMIT 1 OFFSET ";
    //прочитать про reinterpret_cast
    CONST INT SIZE = 2000;
    CHAR command[SIZE];
    strcpy_s(command, selectIdUser);
    WCHAR wchBuffer[SIZE];
    CHAR buffer[SIZE];
    wsprintf(wchBuffer, L"%d\0", idx);
    WideCharToMultiByte(codePage, 0, wchBuffer, SIZE, buffer, SIZE, NULL, NULL);
    strcat_s(command, buffer);
    strcat_s(command, ";");
    char* errorMsg = NULL;
    try 
    {
        result = sqlite3_exec(db, command, NULL, NULL, &errorMsg);
        if (result == SQLITE_OK)
        {
        }
    }
    catch (...) 
    {
        CONST INT SIZE = 2000;
        WCHAR errorMes[SIZE];
        size_t szType;
        mbstowcs_s(&szType, errorMes, errorMsg, SIZE);
        errorMsg = cleaningMemory(errorMsg);
        writtingDownLog(errorMes);
        return 1;
    }
    return 0;
}
void addUser() 
{
    WNDCLASSEXW userWnd;
    //WNDCLASSEXW - Содержит сведения о классе окна.
    ZeroMemory(&userWnd, sizeof(userWnd));
    userWnd.cbSize = sizeof(WNDCLASSEX);
    userWnd.style = CS_HREDRAW | CS_VREDRAW;
    userWnd.lpfnWndProc = AddNewUserWndProc;
    //lpfnWndProc - указатель на зарегестрированное и созданное окно.
    userWnd.cbClsExtra = 0;
    //Выделение дополнительных байт для класса после его регистрации, память 
    //будет привязана к самому классу, а не к конкретному окну.
    userWnd.cbWndExtra = 0;
    //Выделение дополнительных байт для каждого окна, так как каждое окно будет 
    //уникальным, выделена память будет сохранена для окна пока оно будет 
    //существовать.
    userWnd.hInstance = GetModuleHandle(NULL);
    userWnd.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_MYCHAT));;
    userWnd.hCursor = LoadCursor(hInst, IDC_ARROW);
    userWnd.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    userWnd.lpszMenuName = NULL;
    userWnd.lpszClassName = USER_ACCOUNT_CLASS_NAME;
    ATOM reg = RegisterClassEx(&userWnd);
    HFONT fontTitle = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    //Italic - отвечает: true (шрифт наклоненный), fasle (шрифт не наклоненный). Как курсив в microsft word.
    //StrikeOut - отвечает: true (шрифт зачеркнут), false (шрифт не зачеркнут).
    HWND userClass = CreateWindow(USER_ACCOUNT_CLASS_NAME, L"Добавить Пользователя", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, MAIN_FIELD_WIDTH, MAIN_FIELD_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    HWND hLastName = CreateWindow(L"STATIC", L"Фамилия:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_X(0), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    //HMENU - внутренее поле в котором мы можем храннить вписанный текст
    HWND hFirstName = CreateWindow(L"STATIC", L"Имя:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_X(30), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    HWND hMiddleName = CreateWindow(L"STATIC", L"Отчество:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_X(60), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    HWND hPhone = CreateWindow(L"STATIC", L"Телефон:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_X(90), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    HWND hMail = CreateWindow(L"STATIC", L"Почта:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_X(120), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hLastName, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hFirstName, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hMiddleName, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hPhone, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hMail, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    HWND hLast = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(0), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_LAST_NAME, GetModuleHandle(NULL), NULL);
    HWND hFirst = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(30), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_FIRST_NAME, GetModuleHandle(NULL), NULL);
    CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(60), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_MIDDLE_NAME, GetModuleHandle(NULL), NULL);
    CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(90), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_PHONE, GetModuleHandle(NULL), NULL);
    CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(120), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_EMAIL, GetModuleHandle(NULL), NULL);
    HWND hCancMes = CreateWindow(L"BUTTON", L"Отмена", WS_VISIBLE | WS_CHILD | WS_BORDER, CANCEL_BUTTON_POS_X, CANCEL_BUTTON_POS_Y, CANCEL_BUTTON_WIDTH, CANCEL_BUTTON_HEIGH, userClass, (HMENU)IDB_CANCELING_USER_ADDITION, GetModuleHandle(NULL), NULL);
    //Приравнивание ресурса к HMENU нужно для всех типов окон с которым юзер взаимодействует: нажатие клавиши, ввод в поле и так далее.
    HWND hDealMes = CreateWindow(L"BUTTON", L"Да", WS_VISIBLE | WS_CHILD | WS_BORDER, DEAL_BUTTON_POS_X, DEAL_BUTTON_POS_Y, DEAL_BUTTON_WIDTH, DEAL_BUTTON_HEIGHT, userClass, (HMENU)IDB_DEALIGN_USER_ADDITION, GetModuleHandle(NULL), NULL);
    ShowWindow(userClass, SW_SHOWDEFAULT);
    UpdateWindow(userClass);
    MSG msg;
    //msg - хранит сообщение которое получает окно
    while (IsWindow(userClass))
    {
        if (GetMessage(&msg, userClass, 0, 0)) {
            //GetMessage - извлекает и обрабатывает сообщение от окна
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //структура для обработки соообщения окна
    }
}
int checkTables() 
{
    sqlite3* db;
    CONST INT SIZE = 256;
    //char* mesError[SIZE];
    int res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res)
    //похожий вариант прочтения:
    //if(res !=0 ) 
    {
        MessageBox(NULL, L"База данных не подключена!", L"Ошибка!", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        //sqlite3_close - прерывает связь с базой данных
        return 1;
    }
    const char* groupTable = "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = 'groups';";
    //sqlite_master - хранит количество таблиц
    sqlite3_stmt* table;
    //sqlite3_stmt - структура где хранится информация таблице которая была создана с помощью sql-запроса, 
    //который находится в const char* переменной
    if (sqlite3_prepare_v2(db, groupTable, -1, &table, NULL) == SQLITE_OK)
    //sqlite3_prepare_v2 - создает структур откуда мы будем брать наши результаты
    {
        INT curRow = sqlite3_step(table);
        //sqlite3_step - двигаемся по записям из таблицы вынимая, каждую запись
        if (curRow == SQLITE_ROW)
        //SQLITE_ROW - идентификатор строки
        //Если int перменная получает значение sqlite_row, то это значит строка найденна
        {
            INT countRows = sqlite3_column_int(table, 0);
            //sqlite3_column_int - выводит текущий индекс колонки
            if (countRows == 0)
            {
                MessageBox(NULL, L"Ни одной группы не найдено!\nСоздаём новую...", L"Информация", MB_OK | MB_ICONERROR);
                const char* createTable = "CREATE TABLE groups (group_id PRIMARY KEY NOT NULL, group_name TEXT NOT NULL);";
                //char** errorTgroup = mesError;        //Как вариант.
                char* msg = NULL;
                try {
                    INT status = sqlite3_exec(db, createTable, NULL, NULL, &msg);
                    if (status == SQLITE_OK)
                    {
                        MessageBox(NULL, L"Таблица группа создана", L"Инфо", MB_OK | MB_ICONINFORMATION);
                    }
                    else 
                    {
                        MessageBox(NULL, L"Ошибка при создании таблицы группы", L"Ошибка", MB_OK | MB_ICONERROR);
                        throw "SQL-ERROR";
                    }
                }
                catch (...) 
                {
                    CONST INT SIZE = 2000;
                    WCHAR errorMes[SIZE];
                    size_t szType;
                    mbstowcs_s(&szType, errorMes, msg, SIZE);
                    msg = cleaningMemory(msg);
                    writtingDownLog(errorMes);
                    return 1;
                }
            }
        }
        sqlite3_finalize(table);
        //sqlite3_finalize - очищает память от переменной.
    }
    const char* userTable = "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' and name = 'users';";
    if (sqlite3_prepare_v2(db, userTable, -1, &table, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(table);
        if (curRow == SQLITE_ROW) 
        {
            INT countRows = sqlite3_column_int(table, 0);
            if (countRows == 0) 
            {
                const char* createTable = 
                    "CREATE TABLE users (user_id INT PRIMARY KEY NOT NULL,"
                    "first_name TEXT NOT NULL,"
                    "last_name TEXT NOT NULL,"
                    "middle_name TEXT,"
                    "phone TEXT NOT NULL,"
                    "email TEXT NOT NULL,"
                    "path_icon TEXT,"
                    "icon BLOB,"
                    "status INT NOT NULL);";
                //char** errorTUser = mesError;
                char* msg = NULL;
                try {
                    INT status = sqlite3_exec(db, createTable, NULL, NULL, &msg);
                    if (status == SQLITE_OK)
                    {
                        MessageBox(NULL, L"Таблица пользователь создана", L"Инфо", MB_OK | MB_ICONINFORMATION);
                    }
                    else
                    {
                        MessageBox(NULL, L"Ошибка при создании таблицы пользователь", L"Ошибка", MB_OK | MB_ICONERROR);
                        throw "SQL-ERROR";
                    }
                }
                catch (...) 
                {
                    CONST INT SIZE = 2000;
                    WCHAR errorMes[SIZE];
                    size_t szType;
                    mbstowcs_s(&szType, errorMes, msg, SIZE);
                    msg = cleaningMemory(msg);
                    writtingDownLog(errorMes);
                    return 1;
                }
                free(msg);
                msg = NULL;
            }
        }
        sqlite3_finalize(table);
    }
    const char* messageTable = "SELECT COUNT(*) FROM sqlite_master WHERE type ='table' AND name = 'messages';";
    if (sqlite3_prepare_v2(db, messageTable, -1, &table, NULL) == SQLITE_OK) 
    {
        INT curRow = sqlite3_step(table);
        if (curRow == SQLITE_ROW) 
        {
            int countRows = sqlite3_column_int(table, 0);
            if (countRows == 0) 
            {
                MessageBox(NULL, L"Таблица сообщений не создана! Создаём новую", L"Информация", MB_OK | MB_ICONINFORMATION);
                const char* createTable = "CREATE TABLE messages"
                    "(message_id PRIMARY KEY NOT NULL,"
                    "text_field TEXT NOT NULL,"
                    "file_field BLOB,"
                    "sender INT,"
                    "group_id INT,"
                    "recipent INT,"
                    "FOREIGN KEY (sender) REFERENCES users(user_id),"
                    "FOREIGN KEY (recipent) REFERENCES users(user_id),"
                    "FOREIGN KEY (group_id) REFERENCES groups(groupd_id))";
                //char** errorTMes = mesError;
                char* msg = NULL;
                try {
                    int result = sqlite3_exec(db, createTable, NULL, NULL, &msg);
                    //Пятый аргумент в sqlite3_exec - записывает ошибку в перменную которую мы передали.
                    if (result == SQLITE_OK)
                    {
                        MessageBox(NULL, L"Таблица сообщение создана", L"Инфо", MB_OK | MB_ICONINFORMATION);
                    }
                    else
                    {
                        MessageBox(NULL, L"Ошибка при создании таблицы сообщение", L"Инфо", MB_OK | MB_ICONERROR);
                        throw "SQL-ERROR";
                    }
                }
                catch (...) 
                {
                    CONST INT SIZE = 2000;
                    WCHAR errorMes[SIZE];
                    size_t szType;
                    mbstowcs_s(&szType, errorMes, msg, SIZE);
                    msg = cleaningMemory(msg);
                    writtingDownLog(errorMes);
                    return 1;
                }
                free(msg);
                msg = NULL;
            }
        }
        sqlite3_finalize(table);
    }
    sqlite3_close(db);
}
int SendPost(char* msg) 
{
    //SOCKET - сетевой интферфейс для передачи данных на компьютер к примеру сервер
    //WSADATA — структура в C++ для работы с сокетами (WinSock), которая содержит сведения о реализации сокетов Windows.
    WSADATA wsd;
    SOCKET listen = INVALID_SOCKET;
    //INVALID_SOCKET -1
    struct addrinfo* result, * ptr, hints;
    //addrinfo - хранит информацию об адресе в сети
    //WSAStartup - инициализирует SOCKET
    int res = WSAStartup(MAKEWORD(2, 2), &wsd);
    //Инициализирует данные о сокете.
    if (res != 0) 
    {
        MessageBox(NULL, L"WSocket not initializired", L"Error", MB_OK | MB_ICONERROR);
    }
    ZeroMemory(&hints, sizeof(hints));
    //ZeroMemory - обнулить память
    hints.ai_family = AF_UNSPEC;
    //AF_UNSPEC - обычное подключение
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;        //Инициализация типа протокола продключения
    res = getaddrinfo("127.0.0.1", "4321", &hints, &result);
    //если произйодет ошибка, переменная res получит код ошибки
    if (res != 0) 
    {
        MessageBox(NULL, L"getaddrinfo not working!", L"Error", MB_OK | MB_ICONERROR);
        WSACleanup();
        //Очищает данные о сокете.
        return 1;
    }
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        //ptr->ai_next -идём по адерсу ptr пока она не станет null
        listen = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (listen == INVALID_SOCKET)
        {
            MessageBox(NULL, L"socket don't created!", L"ERROR", MB_OK | MB_ICONERROR);
        }
        res = connect(listen, ptr->ai_addr, (int)ptr->ai_addrlen);
        //ai_addr - ссылка на адерс;
        //ai_addrlen - размер адреса
        if (res == SOCKET_ERROR)
        {
            closesocket(listen);
            listen = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    //очищаем память сокета от лишней информации.
    if (listen == INVALID_SOCKET)
    {
        MessageBox(NULL, L"Unable connect to server", L"Error", MB_OK | MB_ICONERROR);
        WSACleanup();
        return res;
    }
    res = shutdown(listen, SD_SEND);
    //shutdown - функция завершения работы отключает отправку или получение в сокете.
    if (res == SOCKET_ERROR)
    {
        MessageBox(NULL, L"Failed shutdown socket!", L"Error", MB_OK | MB_ICONERROR);
        closesocket(listen);
        WSACleanup();
        return res;
    }
    char recvbuf[512];
    do 
    {
        res = recv(listen, recvbuf, 512, 0);
        //recv - recieve;
        if (res < 0)
        {
            MessageBox(NULL, L"Recv filed", L"Error", MB_OK | MB_ICONERROR);
        }
    } while (res > 0);
    closesocket(listen);
    WSACleanup();
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    //ATOM - 2 байтовых WORD.
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    //CS_HREDRAW - перерисовывает окно при перемещении или измении размера клиентской части (окна).
    //CS_VREDRAW - перерисовывает окно при пермещении или изменении высоты клиентской части (окна).
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYCHAT));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYCHAT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
LRESULT CALLBACK UserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDB_ADD_USER:
            addUser();
            if (UpdateList(GetDlgItem(hWnd, IDM_USER_LIST)) == 1) 
            {
                return 1;
            }
            break;
        case IDB_DELETE_USER:
            deleteUser(SendMessage(GetDlgItem(hWnd, IDM_USER_LIST), LB_GETCURSEL, 0, 0));
            if (UpdateList(GetDlgItem(hWnd, IDM_USER_LIST)) == 1)
            {
                return 1;
            }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
        break;
    case WM_DESTROY:
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
//CALLBACK обработчик вызовов внутри нашего окна
int CreateDatabase(HWND hWnd) 
{
    sqlite3* db;
    CONST INT SIZE = 256;
    //char* errorMsg[SIZE];
    int result = sqlite3_open("DatabaseMessanger.db", &db);     //sqlite_open - открывает если файл найден или если файл не найден, тогда создааёт его
    if (result) 
    {
        MessageBox(NULL, L"База данных не подключена", L"Ошибка!", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    else 
    {
        MessageBox(NULL, L"Успешное подключение к базе дынных!", L"Инфо", MB_OK | MB_ICONINFORMATION);
    }
    WNDCLASSEX userWnd;
    ZeroMemory(&userWnd, sizeof(WNDCLASSEX));
    //WNDCLASSEX - в нем можно задать настройки отдельного окна: размер, высоту, расположение и так далее.
    userWnd.cbSize = sizeof(WNDCLASSEX);
    userWnd.style = CS_VREDRAW;
    userWnd.lpfnWndProc = UserWndProc;
    userWnd.cbClsExtra = 0;
    //задаёт количество дополнительных байтов памяти, выделяемых для каждого класса окон после структуры класса 
    userWnd.cbWndExtra = 0;
    //задаёт количество дополнительных байтов памяти, выделяемых для каждого экземпляра окна 
    userWnd.hInstance = GetModuleHandleA(NULL);
    userWnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    //адрес на функцию где будет обрабатываться наше событие. Ссылка на сам userWnd.
    //IDC_ARROW - указатель на курсор мышки.
    userWnd.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    userWnd.lpszClassName = USER_LIST_CLASS_NAME;
    ATOM reg = RegisterClassEx(&userWnd);
    HWND winUser = CreateWindow(USER_LIST_CLASS_NAME, L"Пользователи", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 360, 400, hWnd, 0, GetModuleHandle(NULL), NULL);
    //WS_OVERLAPPEDWINDOW - добавляет к окну значки закрыть, расширить, свернуть делая окно самостоятельным.
    CreateWindow(L"BUTTON", L"Добавить", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 320, 80, 20, winUser, (HMENU)IDB_ADD_USER, GetModuleHandle(NULL), NULL);
    CreateWindow(L"BUTTON", L"Удалить", WS_VISIBLE | WS_CHILD | WS_BORDER, 190, 320, 80, 20, winUser, (HMENU)IDB_DELETE_USER, GetModuleHandle(NULL), NULL);
    HWND userList = CreateWindow(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | WS_BORDER | LBS_NOTIFY, 10, 10, 270, 280, winUser, (HMENU)IDM_USER_LIST, GetModuleHandle(NULL), NULL);
    //WS_BORDER - ключ благодаря которму задаются границы окна.
    //ES_AUTOVSCROLL - автоматическое пермещение по списку если какой-то добавлен или удалён.
    //LBS_NOTIFY - нужен для работы флага LB_GETCURSEL
    //LB_CURSEL - нужен чтобы получить id пользователя по которму мы можем выводить сообщение
    if (UpdateList(GetDlgItem(winUser, IDM_USER_LIST)) == 1) 
    {
        return 1;
    }
    ShowWindow(winUser, SW_SHOWDEFAULT);
    MSG message;
    while (IsWindow(winUser))
    {
        if (GetMessage(&message, winUser, 0, 0))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    const char* findTableUs = "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' and name = 'users'";
    //char** errorCountT = errorMsg;
    char* msg = NULL;
    try 
    {
        sqlite3_exec(db, findTableUs, NULL, NULL, &msg);
    }
    catch (...) 
    {
        MessageBox(NULL, L"Произошла ошибка!", L"Ошибка!", MB_OK | MB_ICONERROR);
        CONST INT SIZE = 2000;
        WCHAR errorMes[SIZE];
        size_t szType;
        mbstowcs_s(&szType, errorMes, msg, SIZE);
        msg = cleaningMemory(msg);
        writtingDownLog(errorMes);
        return 1;
    }
    sqlite3_close(db);
    free(msg);
    msg = NULL;
    return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, MAIN_WINDOW_POSITION_X, MAIN_WINDOW_POSITION_Y, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   HWND storyField = CreateWindow(L"STATIC", L"", WS_VISIBLE| WS_CHILD| WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_WANTRETURN | ES_AUTOVSCROLL, INFO_FIELD_POS_X, INFO_FIELD_POS_Y, INFO_FIELD_WIDTH, INFO_FIELD_HEIGT, hWnd, 0, hInstance, NULL);
   HWND mesFiled = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, MES_FIELD_X, MES_FIELD_Y, MES_FIELD_WIDTH, MES_FIELD_HEIGHT, hWnd, 0, hInstance, NULL);
   HWND mesButton = CreateWindow(L"BUTTON", L"Отправить", WS_VISIBLE | WS_CHILD | WS_BORDER, SEND_MES_WINDOW_X, SEND_MES_WINDOW_Y, SEND_MES_WINDOW_WIDTH, SEND_MES_WINDOW_HEIGHT, hWnd, 0, hInstance, NULL);
   //WS-CHILD - не родительское окно
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                SendPost(NULL);
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDB_CREATE_DB:
                CreateDatabase(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
