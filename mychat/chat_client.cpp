// mychat.cpp : Определяет точку входа для приложения.

//Linker\Input\Additional Dependencies - поле куда записываем названия файлов динамических 
//или статических библиотек, они имеют расширение: .lib . Пример: sqlite.lib 
//C/C++\General\Additional Include Directions - поле куда записываем путь и далее директории
//где файл заголовок (header file) имеющий расширение: .h .
//Файл заголовок (Header file) - файл который содержит прототип фукции. А реализация находится
//в файле .cpp .

#include "framework.h"
#include "mychat.h"
#include <winsock2.h>               //Библиотека для работы с сетью
#include "NamesOfWndowClasses.h"
#include "PositionsButtonsAndWindows.h"
#include <WrittingDownLog.h>
#include <CleaningMemory.h>
#define PORT "4000"
CONST INT USERSIZE = 2000;
CONST INT IDSIZE = 1000;
CONST UINT codePage = 1251;                     //UINT - unsigned int
INT userId = 0;
SOCKET listenSock= INVALID_SOCKET;
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WCHAR szAutorization[MAX_LOADSTRING] = L"Autorization_window_class";
WCHAR szAdditionalInfoClass[MAX_LOADSTRING] = L"Additional_extra_info_class";
WCHAR szNicknameClass[MAX_LOADSTRING] = L"Nickname_window_class";

//ATOM Class
ATOM MyRegisterClass(HINSTANCE hInstance);
ATOM AuthorizationClass(HINSTANCE hInstance);
ATOM AdditionalInfoClass(HINSTANCE hInstance);
ATOM NicknameClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndExtraInfo(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK UserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutProgram(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndNickName(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
CONST INT arraySize = 2000;

class UserInfo
{
private:
    CHAR m_numberPhone[arraySize];
    CHAR m_email[arraySize];
    CHAR m_nickname[arraySize];
public:
    UserInfo() : m_numberPhone(""), m_email(""), m_nickname("") {};
public:
    void setNumberPhone(CHAR* numberPhone);
    CHAR* numberPhone();
    void setEMail(CHAR* email);
    CHAR* email();
    void setNickname(CHAR* nickname);
    CHAR* nickname();
};

void UserInfo::setNumberPhone(CHAR* userPhone) 
{
    strcpy_s(m_numberPhone, userPhone);
}

CHAR* UserInfo::numberPhone() 
{
    return m_numberPhone;
}

void UserInfo::setEMail(CHAR* email) 
{
    strcpy_s(m_email, email);
}

CHAR* UserInfo::email() 
{
    return m_email;
}

void UserInfo::setNickname(CHAR* nickname) 
{
    strcpy_s(m_nickname, nickname);
}

CHAR* UserInfo::nickname() 
{
    return m_nickname;
}
UserInfo userInfo;
int authorizationForm();
int insertEntry(HWND hwnd);
void writtingDownLog(const WCHAR* record);
int recieveData(SOCKET clientSocket);
int classModUserInfo(HWND hWnd, int idx);
int registrationInfo(SOCKET listenSock);
//Точка входа APIENTRY
//APIENTRY - это как и CALLBACK, фукнция обратного вызова
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
    AuthorizationClass(hInstance);
    AdditionalInfoClass(hInstance);
    NicknameClass(hInstance);
    int checkTables();
    // Выполнить инициализацию приложения:
    /*if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }*/
    authorizationForm();
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
int checkTables();
int checkingUserInfo(HWND hWnd);
int modifyUserInfo(HWND hWnd);
int accoutSearch(HWND hField, HWND hList);
LRESULT CALLBACK ModifyUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDB_GIVE_CONSENT_USER_MOD:
            modifyUserInfo(hWnd);
            SendMessage(hWnd, WM_CLOSE, 0, NULL);
            break;
        case IDB_CANCELING_USER_MOD:
            SendMessage(hWnd, WM_CLOSE, 0, NULL);
            break;
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }  
}
//CALLBACK обработчик вызовов внутри нашего окна
LRESULT CALLBACK AddNewUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //LRESULT CALLBACK - это структура меню для созданного окна. Команды внутри окна
    //выполняются через switch(message)
    //LRESULT CALLBACK - своего рода тоже цикл наподобие while, ведь он отвечает за 
    //логику работы окна которое выводится юзеру и с которым юзер может работать
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD (wParam)) 
        {
        case IDB_CANCELLING_USER_ADD:
            SendMessage(hWnd, WM_CLOSE, 0, NULL);
            break;
        case IDB_GIVE_CONSENT_USER_ADD: 
        {
            if (checkingUserInfo(hWnd) != 1)
            {
                insertEntry(hWnd);
                SendMessage(hWnd, WM_CLOSE, 0, NULL);
            }
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

INT checkingEMail(CHAR* eMail) 
{
    int numAt = 0;
    INT len = strlen(eMail);
    if (len == 0) 
    {
        return 0;
    }
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
INT checkingNumberPhone(CHAR* strPhone) 
{
    INT numberCharacters = 0;
    CONST INT SIZE = 2000;
    INT startPosition = 0;
    INT posDash = 0;
    if (strPhone == NULL)
    {
        MessageBox(NULL, L"Строка являеться не определенной", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    if (strcmp(strPhone, "") == 0) 
    {
        MessageBox(NULL, L"Строка являеться пустой", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    INT len = static_cast<INT>(strlen(strPhone));
    int i = 0;
    if (strPhone[0] == '+') 
    {
        for (int i = 0; i < len; i++) 
        {
            strPhone[i] = strPhone[i + 1];
        }
    }
    INT posOpenParet = -1;
    INT posCloseParet = -1;
    bool hasDash = false;
    len = static_cast<int>(strlen(strPhone));
    for (int i = startPosition; i < len; ++i) 
    {
    if ((strPhone[i] < L'0') || (strPhone[i] > '9'))
        {
        if (strPhone[i] != L'-' && strPhone[i] != L'(' && strPhone[i] != L')')
            {
                MessageBox(NULL, L"Неверный формат телефона", L"Ошибка", MB_OK | MB_ICONERROR);
                return 1;
            }
        }
    if (strPhone[i] == L'(')
    {
        posOpenParet = i;
    }
    if (strPhone[i] == L')')
    {
        posCloseParet = i;
    }
    }
    if (posOpenParet != -1 && posOpenParet != len - 1) 
    {
        for (int i = posOpenParet; i < posCloseParet; ++i)
        {
            strPhone[i] = strPhone[i + 1];
        }
    }
    if (posCloseParet != -1 && posCloseParet != len - 1) 
    {
        for (int i = posCloseParet-1; i < len-1; ++i)
        {
            strPhone[i] = strPhone[i + 2];
        }
    }
    do 
    {
        hasDash = false;
        posDash = 0;
        for (int i = 0; i < len; ++i)
        {
            if (strPhone[i] == '-')
            {
                posDash = i;
                hasDash = true;
                    break;
            }
        }
        if (posDash != 0)
        {
            for (i = posDash; i < len - 1; ++i)
            {
                strPhone[i] = strPhone[i + 1];
            }
        }
    } while (hasDash);
    len = static_cast<int>(strlen(strPhone));
    if (len != 11) 
    {
        MessageBox(NULL, L"Неправельный размер номера телефона", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}
int updateList(HWND userList) 
{
    SendMessage(userList, LB_RESETCONTENT, 0, 0);
    //LB_RESETCONTENT - очищает данные дескриптора, который является listBox.
    sqlite3* db;
    INT result = sqlite3_open("DatabaseMessanger.db", &db);
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
        INT nextRow;
        while ((nextRow = sqlite3_step(userTb)) == SQLITE_ROW) 
        {
            INT id = sqlite3_column_int(userTb, 0);
            //sqlite3_column_int - достает int значение из массива stmt по указанной ячейке
            CONST INT SIZE = 2000;
            WCHAR wNickname[SIZE];
            /*WCHAR firstName[SIZE];
            WCHAR lastName[SIZE];*/ 
            const char* chNickname = reinterpret_cast<const char*>(sqlite3_column_text(userTb, 1));
            /*const char* chFirstName = reinterpret_cast<const char*>(sqlite3_column_text(userTb, 1));
            const char* chLastName = reinterpret_cast<const char*>(sqlite3_column_text(userTb, 2));*/
            MultiByteToWideChar(codePage, 0, (char*)chNickname, strlen((char*)chNickname)+1, wNickname, SIZE);
            /*MultiByteToWideChar(codePage, 0, (char*)chFirstName, strlen((char*)chFirstName)+1, firstName, SIZE);
            MultiByteToWideChar(codePage, 0, (char*)chLastName, strlen((char*)chLastName) + 1, lastName, SIZE);*/
            WCHAR toList[SIZE];
            wcscpy_s(toList, wNickname);
            //wcscpy_s(toList, lastName);
            //wcscpy - перезаписывает данные из одного массива в другой, в последнем
            //стирая прежние данные замещая новыми
            /*wcscat_s(toList, L" ");
            wcscat_s(toList, firstName);*/
            SendMessage(userList, LB_ADDSTRING, 0, (LPARAM)toList);
            //LB_ADDSTRING - добавить строку в lixtBox.
        }
    }
    sqlite3_close(db);
    return 0;
}
int checkingUserInfo(HWND hWnd) 
{
    sqlite3* db;
    INT res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res) 
    {
        MessageBox(NULL, L"Ошибка подключения к базе данных!", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    CONST INT SIZE = 2000;
    WCHAR data[SIZE];
    int len = GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_LAST_NAME), data, SIZE);
    //Первый аргумент - это дескрптор дескриптор, из этого дескриптора мы получаем строку, и размер 
    //строки который сохраняется в отдельной int переменной.
    //GetWindowText нужен чтобы получить саму строку которую мы записываем в переменную вторым аргументом 
    //и её длину. Длину мы получаем как отдельное число записываем int переменную.
    if (len == 0) 
    {
        MessageBox(NULL, L"Не введена фамилия!", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    len = GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_FIRST_NAME), data, SIZE);
    if (len == 0) 
    {
        MessageBox(NULL, L"Не введено имя!", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
}
int checkExistsEMail(HWND hWnd) 
{
    sqlite3* db;
    INT res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res) 
    {
        MessageBox(NULL, L"Ошибка подключения к базе данных!", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    CONST INT SIZE = 1500;
    CONST INT COMMANDSIZE = 2000;
    WCHAR wcMail[SIZE];
    CHAR cMail[SIZE];
    GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_EMAIL), wcMail, SIZE);
    WideCharToMultiByte(codePage, 0, wcMail, wcslen(wcMail)+1, cMail, SIZE, NULL, NULL);
    CHAR getMail[COMMANDSIZE];
    strcpy_s(getMail, "SELECT COUNT(*) FROM users WHERE email='");
    strcat_s(getMail, cMail);
    strcat_s(getMail, "'");
    strcat_s(getMail, ";");
    //strcat - для char \ANSI, то есть для латиницы
    INT counter = 0;
    sqlite3_stmt* st;
    if (sqlite3_prepare_v2(db, getMail, -1, &st, NULL) == SQLITE_OK) 
    {
        INT nextRow = sqlite3_step(st);
        if (nextRow == SQLITE_ROW) 
        {
            counter = sqlite3_column_int(st, 0);
            if (counter) 
            {
                MessageBox(NULL, L"Запись пользователя с указанной почтой\nуже существует!", L"Ошибка", MB_OK | MB_ICONERROR);
                sqlite3_close(db);
                return 1;
            }
        }
    }
    sqlite3_finalize(st);
    sqlite3_close(db);
    return 0;
}
int checkExistsPhone(HWND hWnd) 
{
    sqlite3* db;
    INT res = sqlite3_open("DatabaseMessanger.db", &db );
    if (res) 
    {
        MessageBox(NULL, L"Ошибка подключения к базе данных!", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    CONST INT SIZE = 1500;
    CONST INT COMMANDSIZE = 2000;
    WCHAR wcPhone[SIZE];
    CHAR cPhone[SIZE];
    GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_PHONE), wcPhone, SIZE);
    WideCharToMultiByte(codePage, 0, wcPhone, SIZE + 1, cPhone, SIZE, NULL, NULL);
    CHAR getPhone[COMMANDSIZE];
    strcpy_s(getPhone, "SELECT COUNT(*) FROM users WHERE phone='");
    strcat_s(getPhone, cPhone);
    strcat_s(getPhone, "'");
    strcat_s(getPhone, ";");
    int counter = 0;
    sqlite3_stmt* st;
    if (sqlite3_prepare_v2(db, getPhone, -1, &st, NULL) == SQLITE_OK)
        //int nByte - этот параметр отвечает за размер формирующейся строки, мы ставим -1, чтобы фунция
        //сама определила размер строки
        //const char **pzTail - указатель по индексу на адрес в строке команды которую мы передаём вторым
        //аргументом, та часть строки которая не будет являться частью выполняемой команды
    {
        INT curRow = sqlite3_step(st);
        if (curRow == SQLITE_ROW)
        {
            counter = sqlite3_column_int(st, 0);
            sqlite3_finalize(st);
            //sqlite3_finalize - уничтожает объект структуры stmt, тем самым очищая память как от самого 
            //объекта, так и данных хранящихся в этой структуре
            if (counter) 
            {
                MessageBox(NULL, L"Запись пользователя с таким номером телефона\nуже существует", L"Ошибка", MB_OK | MB_ICONERROR);
                sqlite3_close(db);
                return 1;
            }
        }
    }
    sqlite3_close(db);
}
int insertEntry(HWND hWnd)
{
    WCHAR wNickname[USERSIZE];
    /*WCHAR lastName[USERSIZE];
    WCHAR firstName[USERSIZE];
    WCHAR middleName[USERSIZE];*/
    WCHAR numbrerPhone[USERSIZE];
    WCHAR email[USERSIZE];
    WCHAR userId[IDSIZE];
    CONST INT SIZECOMMAND = 12000;
    CHAR command[SIZECOMMAND];
    CHAR buffer[USERSIZE];
    INT number = 0;
    INT status = 0;
    LPCTSTR errMes;
    //UINT - безнаковый целочисленный тип числа
    //(GetDlgItem(hWnd, IDM_ADD_MENU_LAST_NAME), lastName, USERSIZE);
    //GetWindowText - функция которая копирует строку из дескриптора окна в переменную,
    //с размером который мы указываем в поле buffer, последний параметр
    //GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_FIRST_NAME), firstName, USERSIZE);
    //GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_MIDDLE_NAME), middleName, USERSIZE);
    GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_PHONE), numbrerPhone, USERSIZE);
    GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_EMAIL), email, USERSIZE);
    //MessageBox(NULL, lastName, L"INFO", MB_OK | MB_ICONERROR);
    //MessageBox(NULL, firstName, L"INFO", MB_OK | MB_ICONERROR);
    const char lsUsrId[] = "SELECT MAX(USER_ID) FROM users";
    sqlite3* db;
    int res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res)
    {
        MessageBox(NULL, L"Ошибка подключения к базе данных!", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_stmt* table;
    if (sqlite3_prepare_v2(db, lsUsrId, -1, &table, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(table);
        if (curRow == SQLITE_ROW)
        {
            number = sqlite3_column_int(table, 0) + 1;
            //+1 - получаем следующий id;
            //Второй аргумент номер колонки из которой берём значение;
        }
    }
    sqlite3_finalize(table);
    strcpy_s(command, "INSERT INTO users (user_id, nickname, phone, email, status) VALUES(");
    //strcpy_s(command, "INSERT INTO users (user_id, last_name, first_name, middle_name, phone, email, status) VALUES(");
    wsprintf(userId, L"%d\0", number);
    WideCharToMultiByte(codePage, 0, userId, IDSIZE + 1, buffer, USERSIZE, NULL, NULL);
    //CodePage (кодовая страница) - отвечает за хранение типа формата в который будет приобразована строка, 
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
    WideCharToMultiByte(codePage, 0, wNickname, wcslen(wNickname)+1, buffer, USERSIZE, NULL, NULL);
    /*WideCharToMultiByte(codePage, 0, lastName, USERSIZE + 1, buffer, USERSIZE, NULL, NULL);
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
    strcat_s(command, "'");*/
    WideCharToMultiByte(codePage, 0, numbrerPhone, wcslen(numbrerPhone)+1, buffer, USERSIZE, NULL, NULL);
    CONST INT SIZE = 2000;
    CHAR temp[SIZE]{};
    if (checkingNumberPhone(buffer) == 1)
    {
        return 1;
    }
    strcat_s(command, temp);
    strcat_s(command, "'");
    strcat_s(command, ",");
    strcat_s(command, "'");
    if (checkExistsPhone(hWnd) == 1) 
    {
        return 1;
    }
    WideCharToMultiByte(codePage, 0, email, wcslen(email)+1, buffer, USERSIZE, NULL, NULL);
    if (checkingEMail(buffer) == 1) 
    {
        return 1;
    }
    strcat_s(command, buffer);
    strcat_s(command, "'");
    strcat_s(command, ",");
    if (checkExistsEMail(hWnd) == 1) 
    {
        return 1;
    }
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
        mbstowcs_s(&n_size, mesError, msg, SIZE);
        //mbstowcs_s - преобразует многобайтовую символьную строку из массива, в расширенное 
        //символьное представление (WCHAR);
        //pReturnValue / retval - указатель на тип данных в котором будут храниться данные, 
        //то есть размер и тип буфера;
        //dst - указатель на переменную в которую будут записаны преобразованные данные;
        //src - указатель на переменную источник с которой будут считаны данные для преобразования;
        //len - указатель на буффер (размер) строки источник.
        msg = cleaningMemory(msg);
        writtingDownLog(mesError);
        return 1;
    }
    msg = cleaningMemory(msg);
    sqlite3_close(db);
    return 0;
}
int modifyUserInfo(HWND hWnd)
{
    sqlite3* db;
    int res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res) 
    {
        MessageBox(NULL, L"База данных не подключена", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    CONST INT SIZE = 2000;
    CONST INT NUMSIZE = 256;
    WCHAR wNickname[SIZE];
    /*WCHAR wLastName[SIZE];
    WCHAR wFirstName[SIZE];
    WCHAR wMiddleName[SIZE];*/
    WCHAR wPhone[SIZE];
    WCHAR wEMail[SIZE];
    CHAR chNickname[SIZE];
    /*CHAR chLastName[SIZE];
    CHAR chFirstName[SIZE];
    CHAR chMiddleName[SIZE];*/
    CHAR chPhone[SIZE];
    CHAR chEMail[SIZE];
    CHAR command[SIZE];
    WCHAR wNum[NUMSIZE];
    CHAR chNum[NUMSIZE];
    const char* updateData = "UPDATE users SET last_name = '";
    //const char* updateData = "UPDATE users SET last_name = '";
    strcpy_s(command, updateData);
    GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_NICKNAME), wNickname, SIZE);
    /*GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_LAST_NAME), wLastName, SIZE);
    GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_FIRST_NAME), wFirstName, SIZE);
    GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_MIDDLE_NAME), wMiddleName, SIZE);*/
    GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_PHONE), wPhone, SIZE);
    GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_EMAIL), wEMail, SIZE);
    WideCharToMultiByte(codePage, 0, wNickname, wcslen(wNickname) + 1, chNickname, SIZE, NULL, NULL);
    /*WideCharToMultiByte(codePage, 0, wLastName, wcslen(wLastName) + 1, chLastName, SIZE, NULL, NULL);
    WideCharToMultiByte(codePage, 0, wFirstName, wcslen(wFirstName) + 1, chFirstName, SIZE, NULL, NULL);
    WideCharToMultiByte(codePage, 0, wMiddleName, wcslen(wMiddleName) + 1, chMiddleName, SIZE, NULL, NULL);*/
    WideCharToMultiByte(codePage, 0, wPhone, wcslen(wPhone) + 1, chPhone, SIZE, NULL, NULL);
    WideCharToMultiByte(codePage, 0, wEMail, wcslen(wEMail) + 1, chEMail, SIZE, NULL, NULL);
    strcat_s(command, chNickname);
    /*strcat_s(command, chLastName);
    strcat_s(command, "', first_name = '");
    strcat_s(command, chFirstName);
    strcat_s(command, "', middle_name = '");
    strcat_s(command, chMiddleName);*/
    strcat_s(command, "', phone = '");
    strcat_s(command, chPhone);
    strcat_s(command, "', email = '");
    strcat_s(command, chEMail);
    strcat_s(command, "' WHERE user_id = ");
    wsprintf(wNum, L"%d\0", userId);
    WideCharToMultiByte(codePage, 0, wNum, wcslen(wNum) + 1, chNum, NUMSIZE, NULL, NULL);
    strcat_s(command, chNum);
    strcat_s(command, ";");
    //UPDATE users SET last_name = 'Вислов' first_name = 'Юрий', middle_name = 'Аркадъевич', phone = '+7(956)901-56-76', email = 'urei@mail.ru' WHERE user_id = 0;
    char* msg = NULL;
    try {
        INT status = sqlite3_exec(db, command, NULL, NULL, &msg);
        if (status == SQLITE_OK)
        {
            MessageBox(NULL, L"Пользователь успешно изменён!", L"Инфо", MB_OK | MB_ICONINFORMATION);
            sqlite3_close(db);
            return 0;
        }
        else
        {
            MessageBox(NULL, L"Ошибка при обновлении пользовательских данных!", L"Ошибка", MB_OK | MB_ICONERROR);
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
        sqlite3_close(db);
        return 1;
    }
    return 0;
}
int deleteUser(int idx) 
{
    if (idx == -1) 
    {
        return 1;
    }
    sqlite3* db;
    INT result = sqlite3_open("DatabaseMessanger.db", &db);
    if (result)
    {
        MessageBox(NULL, L"База данных не подключена", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    const char* selectIdUser = "SELECT user_id FROM users LIMIT 1 OFFSET ";
    //прочитать про reinterpret_cast
    CONST INT SIZE = 2000;
    CHAR command[SIZE];
    strcpy_s(command, selectIdUser);
    CONST INT BUFSIZE = 200;
    WCHAR wchBuffer[BUFSIZE];
    CHAR buffer[BUFSIZE];
    wsprintf(wchBuffer, L"%d\0", idx);
    WideCharToMultiByte(codePage, 0, wchBuffer, BUFSIZE + 1, buffer, BUFSIZE, NULL, NULL);
    strcat_s(command, buffer);
    strcat_s(command, ";");
    sqlite3_stmt* st;
    if (sqlite3_prepare_v2(db, command, -1, &st, NULL) == SQLITE_OK) 
    {
        INT nextRow = sqlite3_step(st);
        if (nextRow == SQLITE_ROW) 
        {
            INT id = sqlite3_column_int(st, 0);
            //Второй аргумент номер колонки из которой берём значение;
            const char* delReq = "DELETE FROM users WHERE user_id = ";
            strcpy_s(command, delReq);
            wsprintf(wchBuffer, L"%d\0", id);
            WideCharToMultiByte(codePage, 0, wchBuffer, BUFSIZE + 1, buffer, BUFSIZE, NULL, NULL);
            strcat_s(command, buffer);
            strcat_s(command, ";");
            char* errorMsg = NULL;
            try {
                if (sqlite3_exec(db, command, NULL, NULL, &errorMsg) == SQLITE_OK)
                {
                    MessageBox(NULL, L"Пользователь удалён", L"Инфо", MB_OK | MB_ICONINFORMATION);
                }
                else
                {
                    MessageBox(NULL, L"Ошибка при удалении пользователя!", L"Ошибка", MB_OK | MB_ICONERROR);
                    throw "SQL-ERROR";
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
                sqlite3_close(db);
                return 1;
            }
        }
    }
    else 
    {
        MessageBox(NULL, L"ID пользователя не найден!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    sqlite3_finalize(st);
    sqlite3_close(db);
    return 0;
}
int classModUserInfo(HWND hWnd, int idx) 
{
    if (idx == -1) 
    {
        return 1;
    }
    sqlite3* db;
    int res = sqlite3_open("DatabaseMessanger.db", &db);        //sqlite_open - открывает если файл найден или если файл не найден, тогда создааёт его
    if (res) 
    {
        MessageBox(NULL, L"База данных не подключена", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    const char* selId = "SELECT user_id FROM users LIMIT 1 OFFSET ";
    //SELECT user_id FROM users LIMIT 1 OFFSET - сдвиг на какое количество
    //записей и получение id записи, которая будет единственной благодаря
    //ключу LIMIT 
    CONST INT SIZE = 2000;
    CHAR command[SIZE];
    strcpy_s(command, selId);
    CONST INT IDXSIZE = 256;
    WCHAR wNum[IDXSIZE];
    CHAR chNum[IDXSIZE];
    wsprintf(wNum, L"%d\0", idx);
    WideCharToMultiByte(codePage, 0, wNum, wcslen(wNum) + 1, chNum, IDXSIZE, NULL, NULL);
    strcat_s(command, chNum);
    strcat_s(command, ";");
    sqlite3_stmt* st;
    if (sqlite3_prepare_v2(db, command, -1, &st, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(st);
        if (curRow == SQLITE_ROW)
        {
            INT changeUserId = sqlite3_column_int(st, 0);
            sqlite3_finalize(st);
            //sqlite3_column_int - берет строку из stmt и преобразует строку в
            //integer значение,записывая в выделенную int переменную;
            //Второй аргумент номер колонки из которой берём значение;
            userId = changeUserId;
            const char* dataUser = "SELECT nickname phone, email FROM users WHERE user_id = ";
            //const char* dataUser = "SELECT first_name, last_name, middle_name, phone, email FROM users WHERE user_id = ";
            //!SQLITE все работает через запросы которые мы собираем в переменной формата const char*
            WCHAR wId[IDXSIZE]{};
            CHAR chId[IDXSIZE]{};
            wsprintf(wId, L"%d\0", userId);
            WideCharToMultiByte(codePage, 0, wId, wcslen(wId), chId, IDXSIZE, NULL, NULL);
            strcpy_s(command, dataUser);
            strcat_s(command, chId);
            strcat_s(command, ";");
            if (sqlite3_prepare_v2(db, command, -1, &st, NULL) == SQLITE_OK)
            {
                INT curRow = sqlite3_step(st);
                //sqlite3_step возвращает значение в sqlite3_stmt, а sqlite3_exec просто выполняет запрос
                if (curRow == SQLITE_ROW) 
                {
                    const char* chNickname = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
                    /*const char* chFirstName = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
                    const char* chLastName = reinterpret_cast<const char*>(sqlite3_column_text(st, 1));
                    const char* chMiddleName = reinterpret_cast<const char*>(sqlite3_column_text(st, 2));*/
                    const char* chPhone = reinterpret_cast<const char*>(sqlite3_column_text(st, 3));
                    const char* chEMail = reinterpret_cast<const char*>(sqlite3_column_text(st, 4));
                    WCHAR wNickname[SIZE];
                    /*WCHAR wLastName[SIZE];
                    WCHAR wFirstName[SIZE];
                    WCHAR wMiddleName[SIZE];
                    MultiByteToWideChar(codePage, 0, chFirstName, strlen(chFirstName) + 1, wFirstName, SIZE);
                    MultiByteToWideChar(codePage, 0, chLastName, strlen(chLastName) + 1, wLastName, SIZE);
                    MultiByteToWideChar(codePage, 0, chMiddleName, strlen(chMiddleName) + 1, wMiddleName, SIZE);*/
                    WCHAR wPhone[SIZE];
                    WCHAR wEMail[SIZE];
                    MultiByteToWideChar(codePage, 0, chPhone, strlen(chPhone) + 1, wPhone, SIZE);
                    MultiByteToWideChar(codePage, 0, chEMail, strlen(chEMail) + 1, wEMail, SIZE);
                    MultiByteToWideChar(codePage, 0, chNickname, strlen(chNickname) + 1, wNickname, SIZE);
                    WNDCLASSEX userWnd;
                    ZeroMemory(&userWnd, sizeof(userWnd));
                    userWnd.cbSize = sizeof(WNDCLASSEX);
                    userWnd.style = CS_HREDRAW | CS_VREDRAW;
                    userWnd.lpfnWndProc = ModifyUserWndProc;
                    userWnd.cbClsExtra = 0;
                    userWnd.cbWndExtra = 0;
                    userWnd.hInstance = GetModuleHandle(NULL);
                    userWnd.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_MYCHAT));
                    userWnd.hCursor = LoadCursor(hInst, IDC_ARROW);
                    userWnd.hbrBackground = (HBRUSH)(COLOR_WINDOW);
                    userWnd.lpszMenuName = NULL;
                    userWnd.lpszClassName = INFO_MODIFICATION_CLASS;
                    ATOM reg = RegisterClassEx(&userWnd);
                    HWND userClass = CreateWindow(INFO_MODIFICATION_CLASS, L"Измененить", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, MODIFY_CLASS_WIDTH, MODIFY_CLASS_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
                    HWND hLastName = CreateWindow(L"STATIC", L"Имя:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, DESCRIPT_FIELD_MOD_LAST_POS_Y, DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, NULL, NULL, GetModuleHandle(NULL), NULL);
                    /*HWND hLastName = CreateWindow(L"STATIC", L"Фамилия:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, DESCRIPT_FIELD_MOD_LAST_POS_Y, DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, NULL, NULL, GetModuleHandle(NULL), NULL);
                    HWND hFirstName = CreateWindow(L"STATIC", L"Имя:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, DESCRIPT_FIELD_MOD_POS_Y(20), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, NULL, NULL, GetModuleHandle(NULL), NULL);
                    HWND hMiddleName = CreateWindow(L"STATIC", L"Отчество:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, DESCRIPT_FIELD_MOD_POS_Y(50), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, NULL, NULL, GetModuleHandle(NULL), NULL);*/
                    HWND hPhone = CreateWindow(L"STATIC", L"Телефон:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, DESCRIPT_FIELD_MOD_POS_Y(80), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, NULL, NULL, GetModuleHandle(NULL), NULL);
                    HWND hEMail = CreateWindow(L"STATIC", L"Почта:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, DESCRIPT_FIELD_MOD_POS_Y(110), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, NULL, NULL, GetModuleHandle(NULL), NULL);
                    CreateWindow(L"EDIT", wNickname, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X, 20, DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, (HMENU)IDR_MOD_MENU_NICKNAME, GetModuleHandle(NULL), NULL);
                    /*CreateWindow(L"EDIT", wLastName, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X, 20, DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, (HMENU)IDR_MOD_MENU_LAST_NAME, GetModuleHandle(NULL), NULL);
                    CreateWindow(L"EDIT", wFirstName, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X, MODIFY_BUTTON_EDIT_POS_Y(20), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, (HMENU)IDR_MOD_MENU_FIRST_NAME, GetModuleHandle(NULL), NULL);
                    CreateWindow(L"EDIT", wMiddleName, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X, MODIFY_BUTTON_EDIT_POS_Y(50), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, (HMENU)IDR_MOD_MENU_MIDDLE_NAME, GetModuleHandle(NULL), NULL);*/
                    CreateWindow(L"EDIT", wPhone, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X, MODIFY_BUTTON_EDIT_POS_Y(80), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, (HMENU)IDR_MOD_MENU_PHONE, GetModuleHandle(NULL), NULL);
                    CreateWindow(L"EDIT", wEMail, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X, MODIFY_BUTTON_EDIT_POS_Y(110), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_MOD_HEIGHT, userClass, (HMENU)IDR_MOD_MENU_EMAIL, GetModuleHandle(NULL), NULL);
                    CreateWindow(L"BUTTON", L"ОК", WS_VISIBLE | WS_CHILD | WS_BORDER, MOD_ACCEPT_BUTTON_POS_X, ACCEPT_BUTTON_POS_Y, MOD_ACCEPT_BUTTON_WIDTH, ACCEPT_BUTTON_HEIGHT, userClass, (HMENU)IDB_GIVE_CONSENT_USER_MOD, GetModuleHandle(NULL), NULL);
                    CreateWindow(L"BUTTON", L"Отмена", WS_VISIBLE | WS_CHILD | WS_BORDER, MOD_CANCEL_BUTTON_POS_X, MOD_CANCEL_BUTTON_POS_Y, MOD_CANCEL_BUTTON_WIDTH, MOD_CANCEL_BUTTON_HEIGHT, userClass, (HMENU)IDB_CANCELING_USER_MOD, GetModuleHandle(NULL), NULL);
                    ShowWindow(userClass, SW_SHOWDEFAULT);
                    MSG msg;
                    sqlite3_finalize(st);
                    sqlite3_close(db);
                    EnableWindow(hWnd, FALSE);
                    while (IsWindow(userClass))
                    {
                        if (GetMessage(&msg, userClass, 0, 0))
                        {
                            TranslateMessage(&msg);
                            DispatchMessage(&msg);
                        }
                    }
                }
            }
        }
    }
    EnableWindow(hWnd, TRUE);
    SetActiveWindow(hWnd);
    sqlite3_finalize(st);
    sqlite3_close(db);
    return 0;
}
void addUser() 
{
    WNDCLASSEX userWnd;
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
    //Выделение дополнительных байт для каждого окна, так как каждое окно будет уникальным,
    //выделена память будет сохранена для окна пока оно будет существовать.
    userWnd.hInstance = GetModuleHandle(NULL);
    userWnd.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_MYCHAT));;
    userWnd.hCursor = LoadCursor(hInst, IDC_ARROW);
    userWnd.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    userWnd.lpszMenuName = NULL;
    userWnd.lpszClassName = USER_ACCOUNT_CLASS_NAME;
    ATOM reg = RegisterClassEx(&userWnd);
    HFONT fontTitle = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    //FW_NORMAL - указывает тип шрифта (жирный, полужирный и тд.)
    //Italic - отвечает: true (шрифт наклоненный), fasle (шрифт не наклоненный). Как курсив в microsft word.
    //StrikeOut - отвечает: true (шрифт зачеркнут), false (шрифт не зачеркнут).
    HWND userClass = CreateWindow(USER_ACCOUNT_CLASS_NAME, L"Добавить", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, MAIN_FIELD_WIDTH, MAIN_FIELD_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
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
    CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(0), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_ADD_MENU_LAST_NAME, GetModuleHandle(NULL), NULL);
    CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(30), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_ADD_MENU_FIRST_NAME, GetModuleHandle(NULL), NULL);
    CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(60), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_ADD_MENU_MIDDLE_NAME, GetModuleHandle(NULL), NULL);
    CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(90), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_ADD_MENU_PHONE, GetModuleHandle(NULL), NULL);
    CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_X(120), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_ADD_MENU_EMAIL, GetModuleHandle(NULL), NULL);
    CreateWindow(L"BUTTON", L"Отмена", WS_VISIBLE | WS_CHILD | WS_BORDER, CANCEL_ADDING_ENTRY_POS_X, CANCEL_ADDING_ENTRY_POS_Y, CANCEL_ADDING_ENTRY_WIDTH, CANCEL_ADDING_ENTRY_HEIGHT, userClass, (HMENU)IDB_CANCELLING_USER_ADD, GetModuleHandle(NULL), NULL);
    //Приравнивание ресурса к HMENU нужно для всех типов окон с которым юзер взаимодействует: нажатие клавиши, ввод в поле и так далее.
    CreateWindow(L"BUTTON", L"Ок", WS_VISIBLE | WS_CHILD | WS_BORDER, ACCEPT_BUTTON_POS_X, ACCEPT_BUTTON_POS_Y, ACCEPT_BUTTON_WIDTH, ACCEPT_BUTTON_HEIGHT, userClass, (HMENU)IDB_GIVE_CONSENT_USER_ADD, GetModuleHandle(NULL), NULL);
    ShowWindow(userClass, SW_SHOWDEFAULT);
    UpdateWindow(userClass);
    MSG msg;
    //msg - хранит сообщение которое получает окно
    while (IsWindow(userClass))
    {
        if (GetMessage(&msg, userClass, 0, 0)) 
        {
            //GetMessage - извлекает и обрабатывает сообщение от окна
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //структура для обработки соообщения окна
    }
}

int connectToServ()
{
    WSADATA wsd;
    CONST INT SIZE = 256;
    WCHAR mesErr[SIZE] = {};
    //WSADATA — структура в C++ для работы с сокетами (WinSock), которая содержит сведения о реализации сокетов Windows.
    DWORD dwErr = 0;
    //DWORD - 32-bit unsigend integer
    INT res = WSAStartup(MAKEWORD(2, 2), &wsd);
    //Функция WSAStartup должна быть первой функцией Windows Sockets, вызванной приложением или библиотекой DLL.
    dwErr = WSAGetLastError();
    if (res != 0)
    {
        wsprintf(mesErr, L"%s: %d", L"Ошибка иницализации данных сокета", WSAGetLastError());
        MessageBox(NULL, mesErr, L"Ошибка", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }
    //SOCKET - сетевой интферфейс для передачи данных на компьютер к примеру сервер
    //INVALID_SOCKET -1
    struct addrinfo* result = NULL, hints;
    //addrinfo - хранит информацию об адресе в сети
    ZeroMemory(&hints, sizeof(hints));
    //ZeroMemory - обнуляе память у объекта hints структуры adddrinfo*

    //* Инициализация типа протокола продключения
    hints.ai_family = AF_UNSPEC;
    //AF_UNSPEC - обычное подключение
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    res = getaddrinfo("127.0.0.1", PORT, &hints, &result);        //IP адрес клиента
    //если произойдет ошибка, переменная res получит код ошибки
    if (res != 0)
    {
        MessageBox(NULL, L"Ошибка получения клиентского адреса!", L"Ошибка", MB_OK | MB_ICONERROR);
        WSACleanup();
        //Очищает данные о сокете.
        return 1;
    }
    //f = INVALID_SOCKET;
    listenSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSock == INVALID_SOCKET)
    {
        MessageBox(NULL, L"Сокет не создан!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    res = connect(listenSock, result->ai_addr, (int)result->ai_addrlen);
    //ai_addr - ссылка на адерс;
    //ai_addrlen - размер адреса
    if (res == SOCKET_ERROR)
    {
        MessageBox(NULL, L"Ошибка подключения к серверу", L"Ошибка!", MB_OK | MB_ICONERROR);
        closesocket(listenSock);
        listenSock = INVALID_SOCKET;
        return 1;
    }
    freeaddrinfo(result);
    //очищаем память сокета от лишней информации.
    if (listenSock == INVALID_SOCKET)
    {
        MessageBox(NULL, L"Unable connect to server", L"Error", MB_OK | MB_ICONERROR);
        WSACleanup();
        return res;
    }
    return  0;
}

int disconnectFromServ(SOCKET listenSock) 
{
    INT res = shutdown(listenSock, SD_SEND);
    //shutdown - функция завершения работы отключает отправку или получение в сокете.
    if (res == SOCKET_ERROR)
    {
        MessageBox(NULL, L"Ошибка закрытия сокета!", L"Ошибка", MB_OK | MB_ICONERROR);
        closesocket(listenSock);
        WSACleanup();
        return res;
    }
    closesocket(listenSock);
    WSACleanup();
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    //ATOM - 2 байтовых WORD.
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    //CS_HREDRAW - перерисовывает окно при перемещении или изменении размера клиентской части (окна).
    //CS_VREDRAW - перерисовывает окно при перемещении или изменении высоты клиентской части (окна).
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

ATOM AuthorizationClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    //CS_HREDRAW - перерисовывает окно при перемещении или изменении размера клиентской части (окна).
    //CS_VREDRAW - перерисовывает окно при перемещении или изменении высоты клиентской части (окна).
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYCHAT));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szAutorization;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
ATOM AdditionalInfoClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndExtraInfo;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYCHAT));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szAdditionalInfoClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}
int AddAdditionalInfo() 
{
    HWND mainWin = CreateWindow(szAdditionalInfoClass, L"My Chat", WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, ADDITIONAL_INFO_WIDTH, ADDITIONAL_INFO_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    HFONT hFont = CreateFont(FONT_THE_REGISTRATION_WINDOW, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    HWND extraMail = CreateWindow(L"STATIC", L"Укажите почту, как дополнительное средство для связи:", WS_CHILD | WS_VISIBLE, DESCRIPT_EMAIL_AS_EXTRA_INFO_POS_X, DESCRIPT_EMAIL_AS_EXTRA_INFO_POS_Y, DESCRIPT_EMAIL_AS_EXTRA_INFO_POS_WIDTH, DESCRIPT_EMAIL_AS_EXTRA_INFO_POS_HEIGHT, mainWin, NULL, GetModuleHandle(NULL), NULL);
    HWND emailInput = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, EMAIL_INPUT_FIELD_POS_X, EMAIL_INPUT_FIELD_POS_Y, EMAIL_INPUT_FIELD_POS_WIDTH, EMAIL_INPUT_FIELD_POS_HEIGHT, mainWin, (HMENU)IDR_REGISTRATION_REG_MAIL, GetModuleHandle(NULL), NULL);
    HWND skipButton = CreateWindow(L"BUTTON", L"Пропустить", WS_CHILD | WS_VISIBLE, EMAIL_SKIP_BUTTON_POS_X, EMAIL_SKIP_BUTTON_POS_Y, EMAIL_SKIP_BUTTON_POS_WIDTH, EMAIL_INPUT_FIELD_POS_HEIGHT, mainWin, (HMENU)IDB_ENTERING_MAIL_SKIP, GetModuleHandle(NULL), NULL);
    HWND accessButton = CreateWindow(L"BUTTON", L"Принять", WS_CHILD | WS_VISIBLE, EMAIL_ACCESS_BUTTON_POS_X, EMAIL_ACCESS_BUTTON_POS_Y, EMAIL_ACCESS_BUTTON_POS_WIDTH, EMAIL_ACCESS_BUTTON_POS_HEIGHT, mainWin, (HMENU)IDB_ENTERING_MAIL_ACCEPT, GetModuleHandle(NULL), NULL);
    SendMessage(extraMail, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(emailInput, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(skipButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(accessButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    ShowWindow(mainWin, SW_SHOWDEFAULT);
    MSG msg;
    while (IsWindow(mainWin)) 
    {
        if (GetMessage(&msg, mainWin, 0, 0)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}
int authorizationForm() 
{
    HWND registrWin = CreateWindow(szAutorization, L"My Chat", WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, REGISTRATION_WINDOW_WIDTH, REGISTRATION_WINDOW_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    //WS_OVERLAPPEDWIDNOW - создаёт перекрывающее окно границы которого можно двигать влево/вправо или вверх/вниз.
    //WS_CAPTION + WS_SYSMENU - позволяет отобразить кнопки окна такие как: закрыть, свернуть, расширить и тд.
    RECT rc;
    GetClientRect(registrWin, &rc);
    //получаем длину окна
    HWND tabCrl = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, rc.right, rc.bottom, registrWin, (HMENU)IDB_REGISTER_TABCONTROL, GetModuleHandle(NULL), NULL);
    //rc.right - длина
    //rc.buttom - ширина
    TCITEM tct;
    const WCHAR* str[2] = { L"Регистрация", L"Авторизация" };
    //str[2] - двухмерный массив
    tct.mask = TCIF_TEXT | TCIF_IMAGE;
    tct.iImage = -1;
    //iImage - индекс в списке изображений элемента управления «вкладка» или -1, если для вкладки нет изображения.
    tct.pszText = (LPWSTR)str[0];
    //pszText - указатель на строку с завершающим нулем, содержащую текст вкладки при установке информации об элементе. 
    TabCtrl_InsertItem(tabCrl, 0, &tct);
    tct.pszText = (LPWSTR)str[1];
    TabCtrl_InsertItem(tabCrl, 1, &tct);
    HFONT hFont = CreateFont(FONT_THE_REGISTRATION_WINDOW, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    HWND numPhone = CreateWindow(L"STATIC", L"Номер телефона:", WS_CHILD | WS_VISIBLE, REGISTRATION_PHONE_FIELD_POS_X, REGISTRATION_PHONE_FIELD_POS_Y, REGISTRATION_PHONE_FIELD_POS_WIDTH, REGISTRATION_PHONE_FIELD_POS_HEIGHT, registrWin, (HMENU)IDM_REGISTER_PHONE, GetModuleHandle(NULL), NULL);
    HWND email = CreateWindow(L"STATIC", L"Почта:", WS_CHILD, 10, 110, 170, 26, registrWin, (HMENU)IDM_REGISTER_EMAIL, GetModuleHandle(NULL), NULL);
    HWND enteringPhoneByReg = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, INPUT_REGISTERED_PHONE_POS_X, INPUT_REGISTERED_PHONE_POS_Y, INPUT_REGISTERED_PHONE_POS_WIDTH, INPUT_REGISTERED_PHONE_POS_HEIGHT, registrWin, (HMENU)IDR_REGISTRATION_REG_PHONE, GetModuleHandle(NULL), NULL);
    //WS_VISIBLE - при запуске программы делает поле: STATIC, EDIT, BUTTON и тд. видимым в окне программы.
    HWND enteringPhoneByAccept = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, INPUT_REGISTERED_PHONE_POS_X, INPUT_REGISTERED_PHONE_POS_Y, INPUT_REGISTERED_PHONE_POS_WIDTH, INPUT_REGISTERED_PHONE_POS_HEIGHT, registrWin, (HMENU)IDR_REGISTRATION_ACCEPT_PHONE, GetModuleHandle(NULL), NULL);
    HWND enteringEmailByAccept = CreateWindow(L"EDIT", L"", WS_CHILD | WS_BORDER, 10, 140, 360, 32, registrWin, (HMENU)IDR_REGISTRATION_ACCEPT_EMAIL, GetModuleHandle(NULL), NULL);
    HWND reg = CreateWindow(L"BUTTON", L"Зарегистрироваться", WS_CHILD | WS_VISIBLE | WS_BORDER, REGISTRATION_BY_PHONE_POS_X, REGISTRATION_BY_PHONE_POS_Y, REGISTRATION_BY_PHONE_POS_WIDTH, REGISTRATION_BY_PHONE_POS_HEIGHT, registrWin, (HMENU)IDB_REGISTER_REGIST, GetModuleHandle(NULL), NULL);
    HWND accept = CreateWindow(L"BUTTON", L"Войти", WS_CHILD | WS_BORDER, LOG_IN_TO_REGISTER_POX_X, LOG_IN_TO_REGISTER_POX_Y, LOG_IN_TO_REGISTER_POX_WIDTH, LOG_IN_TO_REGISTER_POX_HEIGHT, registrWin, (HMENU)IDB_REGISTER_ACCEPT, GetModuleHandle(NULL), NULL);
    SendMessage(numPhone, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(email, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(enteringPhoneByReg, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(enteringPhoneByAccept, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(reg, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(accept, WM_SETFONT, (WPARAM)hFont, TRUE);
    ShowWindow(registrWin, SW_SHOWDEFAULT);
    MSG msg;
    while (IsWindow(registrWin))
    {
        if (GetMessage(&msg, registrWin, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
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
            if (updateList(GetDlgItem(hWnd, IDM_MAIN_USER_LIST)) == 1) 
            {
                return 1;
            }
            break;
        case IDB_MODIFY_USER:
            classModUserInfo(hWnd, SendMessage(GetDlgItem(hWnd, IDM_MAIN_USER_LIST), LB_GETCURSEL, 0, 0));
            break;
        case IDB_DELETE_USER:
            deleteUser(SendMessage(GetDlgItem(hWnd, IDM_MAIN_USER_LIST), LB_GETCURSEL, 0, 0));
            //SendMessage(GetDlgItem(hWnd, IDM_USER_LIST), LB_GETCURSEL, 0, 0) - конструкция 
            //чтобы получить id/индекс выбранного пользователя
            //LB_GETCURSEL - флаг на получение индекса пользователя из дескриптора и всё это
            //выполняется через SendMessage работающий с системой Windows
            if (updateList(GetDlgItem(hWnd, IDM_MAIN_USER_LIST)) == 1)
            {
                return 1;
            }
            break;
        case IDB_CANCEL:
            SendMessage(hWnd, WM_CLOSE, 0, NULL);
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

int checkTables()
{
    sqlite3* db;
    CONST INT SIZE = 2000;
    int res = sqlite3_open("DatabaseMessanger.db", &db);            //sqlite_open - открывает если файл найден или если файл не найден, тогда создааёт его
    if (res)
        //похожий вариант прочтения:
        //if(res != 0) 
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
                    WCHAR errorMes[SIZE];
                    size_t szType;
                    mbstowcs_s(&szType, errorMes, msg, SIZE);
                    msg = cleaningMemory(msg);
                    writtingDownLog(errorMes);
                    sqlite3_close(db);
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
                    "nickname TEXT NOT NULL,"
                    "phone NUMBER NOT NULL,"
                    "email TEXT NULL,"
                    "path_icon TEXT,"
                    "icon BLOB,"
                    "status INT NOT NULL);";
                /*const char* createTable =
                    "CREATE TABLE users (user_id INT PRIMARY KEY NOT NULL,"
                    "first_name TEXT NOT NULL,"
                    "last_name TEXT NOT NULL,"
                    "middle_name TEXT,"
                    "phone NUMBER NOT NULL,"
                    "email TEXT NULL,"
                    "path_icon TEXT,"
                    "icon BLOB,"
                    "status INT NOT NULL);";*/
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
                    sqlite3_close(db);
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
                    //Пятый аргумент в sqlite3_exec - записывает ошибку в переменную которую мы передали.
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
                    sqlite3_close(db);
                    return 1;
                }
                free(msg);
                msg = NULL;
            }
        }
        sqlite3_finalize(table);
    }
    sqlite3_close(db);
    return 0;
}
int accoutSearch(HWND hField, HWND hList) 
{
    SendMessage(hList, LB_RESETCONTENT, NULL, NULL);
    sqlite3* db;
    INT result = sqlite3_open("DatabaseMessanger.db", &db);
    if (result) 
    {
        MessageBox(NULL, L"Ошибка подключения к базе данных", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    CONST INT SIZE = 256;
    WCHAR wStr[SIZE] = {};
    //Фигурные скобки после знака равно при объявлении WCHAR/CHAR массива служат для его 
    //инициализации и присваивания нуль-терминатора (\0) в конец строки 
    CHAR chStr[SIZE] = {};
    CHAR chFirstName[SIZE] = {};
    CHAR chLastName[SIZE] = {};
    WCHAR wFirstName[SIZE] = {};
    WCHAR wLastName[SIZE] = {};
    GetWindowText(hField, wStr, SIZE);
    if (wcscmp(wStr, L"") == 0) 
    {
        if (updateList(hList) == 1) 
        {
            return 1;
        }
        return 0;
    }
    WideCharToMultiByte(codePage, 0, wStr, SIZE + 1, chStr, SIZE, NULL, NULL);
    sqlite3_stmt* table;
    char getUserFirstName[1024] = "SELECT first_name, last_name FROM users WHERE first_name LIKE '%";
    strcat_s(getUserFirstName, chStr);
    strcat_s(getUserFirstName, "%' OR last_name LIKE '%");
    strcat_s(getUserFirstName, chStr);
    strcat_s(getUserFirstName, "%'");
    if (sqlite3_prepare_v2(db, getUserFirstName, -1, &table, NULL) == SQLITE_OK) 
    {
        INT curRow;
        while ((curRow = sqlite3_step(table)) == SQLITE_ROW)
        {
            strcpy_s(chFirstName,reinterpret_cast<const char*>(sqlite3_column_text(table, 0)));
            strcpy_s(chLastName, reinterpret_cast<const char*>(sqlite3_column_text(table, 1)));
            //reinterpret_cast - делает преобразование без проверки в отличие от static_cast
            MultiByteToWideChar(codePage, 0, chFirstName, strlen(chFirstName) + 1, wFirstName, SIZE);
            //strlen - расчет количества символов для ansi инча char строки
            //wcslen - расчет количества символом для wide char строки
            MultiByteToWideChar(codePage, 0, chLastName, strlen(chLastName) + 1, wLastName, SIZE);
            WCHAR toList[SIZE] = {};
            wcscpy_s(toList, wFirstName);
            wcscat_s(toList, L" ");
            wcscat_s(toList, wLastName);
            SendMessage(hField, LB_ADDSTRING, NULL, (LPARAM)chFirstName);
        }
    }
    sqlite3_finalize(table);
    sqlite3_close(db);
    return 0;
}
int recieveData(SOCKET clientSocket)
{
    INT iResuslt = 0;
    CHAR recvbuf[512];
    while (iResuslt <= 0)
    {
        iResuslt = recv(clientSocket, recvbuf, 512, 0);
        //recv - recieve;
        if (iResuslt < 0)
        {
            MessageBox(NULL, L"Ошибка получения данных", L"Ошибка", MB_OK | MB_ICONERROR);
        }
        INT res = strncmp(recvbuf, "EXIST", 5);
        if (!res) 
        //!res - res == 0
        {
            return 1;
        }
    }
    return 0;
}

int CreatingAuthorWindow(HWND hWnd) 
{
    WNDCLASSEX authorWnd;
    ZeroMemory(&authorWnd, sizeof(authorWnd));
    //WNDCLASSEX - в нем можно задать настройки отдельного окна: размер, высоту, расположение и так далее.
    authorWnd.cbSize = sizeof(WNDCLASSEX);
    authorWnd.style = CS_HREDRAW | CS_VREDRAW;
    authorWnd.lpfnWndProc = AboutProgram;
    authorWnd.cbClsExtra = 0;
    //задаёт количество дополнительных байтов памяти, выделяемых для каждого класса окон после структуры класса 
    authorWnd.cbWndExtra = 0;
    //задаёт количество дополнительных байтов памяти, выделяемых для каждого экземпляра окна 
    authorWnd.hInstance = GetModuleHandle(NULL);
    authorWnd.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDC_ICON));
    authorWnd.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_ARROW));
    //адрес на функцию где будет обрабатываться наше событие. Ссылка на сам userWnd.
    //IDC_ARROW - указатель на курсор мышки.
    authorWnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
    authorWnd.lpszMenuName = NULL;
    authorWnd.lpszClassName = INFO_ABOUT_PROGRAM;
    ATOM reg = RegisterClassEx(&authorWnd);
    HWND userClass = CreateWindow(INFO_ABOUT_PROGRAM, L"О Программе", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, INFO_ABOUT_PROGRAM_WIDTH, INFO_ABOUT_PROGRAM_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    //WS_OVERLAPPEDWINDOW - добавляет к окну значки закрыть, расширить, свернуть делая окно самостоятельным.
    CreateWindow(L"STATIC", L"Авторы:", WS_VISIBLE | WS_CHILD, ABOUT_AUTHOR_FIELD_POX_X, AUTHOR_FIELD_POS_Y, ABOUT_AUTHOR_FIELD_WIDTH, ABOUT_AUTHOR_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    CreateWindow(L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_AUTOVSCROLL | WS_BORDER, AUTHOR_LIST_POS_X, AUTHOR_FIELD_POS_Y, AUTHOR_LIST_POS_WIDTH, AUTHOR_LIST_POS_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    //WS_BORDER - ключ благодаря которму задаются границы окна.
    //ES_AUTOVSCROLL - автоматическое пермещение по списку если какой-то добавлен или удалён.
    //LBS_NOTIFY - нужен для работы флага LB_GETCURSEL
    //LB_CURSEL - нужен чтобы получить id пользователя по которму мы можем выводить сообщение
    CreateWindow(L"STATIC", L"Версия:", WS_VISIBLE | WS_CHILD, ABOUT_AUTHOR_FIELD_POX_X, ABOUT_VERSION_FIELD_POX_Y, ABOUT_AUTHOR_FIELD_WIDTH, ABOUT_AUTHOR_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    CreateWindow(L"BUTTON", L"Закрыть", WS_VISIBLE | WS_CHILD | WS_BORDER, 320, 150, 70, 20, userClass, (HMENU)IDB_ABOUT_PROG_BUTTON_CANCEL, GetModuleHandle(NULL), NULL);
    ShowWindow(userClass, SW_SHOWDEFAULT);
    MSG msg;
    while (IsWindow(userClass))
    {
        if (GetMessage(&msg, userClass, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
       MAIN_WINDOW_POSITION_WIDTH, MAIN_WINDOW_POSITION_HEIGHT, NULL, NULL, hInstance, NULL);
   if (!hWnd)
   {
      return FALSE;
   }
   CreateWindow(L"STATIC", L"", WS_VISIBLE| WS_CHILD| WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_WANTRETURN | ES_AUTOVSCROLL, INFO_FIELD_POS_X, INFO_FIELD_POS_Y, INFO_FIELD_WIDTH, INFO_FIELD_HEIGT, hWnd, 0, hInstance, NULL);
   CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, MES_FIELD_X, MES_FIELD_Y, MES_FIELD_WIDTH, MES_FIELD_HEIGHT, hWnd, 0, hInstance, NULL);
   CreateWindow(L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_AUTOVSCROLL | WS_BORDER | LBS_NOTIFY, MAIN_LIST_USERS_POS_X, MAIN_LIST_USERS_POS_Y, MAIN_LIST_USERS_WIDTH, MAIN_LIST_USERS_HEIGHT, hWnd, (HMENU)IDM_MAIN_USER_LIST, hInstance, NULL);
   if (updateList(GetDlgItem(hWnd, IDM_MAIN_USER_LIST)) == 1) 
   {
       return 1;
   }
   CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 450, 240, 20, hWnd, (HMENU)IDR_SEARCH_FIELD, hInstance, NULL);
   CreateWindow(L"BUTTON", L"Отправить", WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_BORDER, SEND_MES_WINDOW_X, SEND_MES_WINDOW_Y, SEND_MES_WINDOW_WIDTH, SEND_MES_WINDOW_HEIGHT, hWnd, 0, hInstance, NULL);
   CreateWindow(L"BUTTON", L"Поиск", WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_BORDER, 10, SEND_MES_WINDOW_Y, 60, SEND_MES_WINDOW_HEIGHT, hWnd, (HMENU)IDB_SEARCH, hInstance, NULL);
   //WS-CHILD - не родительское окно
   CreateWindow(L"BUTTON", L"Добавить", WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_BORDER, 80, SEND_MES_WINDOW_Y, 80, SEND_MES_WINDOW_HEIGHT, hWnd, (HMENU)IDB_ADD_USER, hInstance, NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}
int nicknameWindow()
{
    HWND mainWin = CreateWindow(szNicknameClass, L"My Chat", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, NICK_REGISTRATION_WINDOW_WIDTH, NICK_REGISTRATION_WINDOW_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    HFONT hFont = CreateFont(FONT_THE_REGISTRATION_WINDOW, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    HWND desriptInfo = CreateWindow(L"STATIC", L"Введите ваше имя: ", WS_VISIBLE | WS_CHILD, DESCRIPT_NICK_FIELD_POS_X, DESCRIPT_NICK_FIELD_POS_Y, DESCRIPT_NICK_FIELD_POS_WIDTH, DESCRIPT_NICK_FIELD_POS_HEIGHT, mainWin, NULL, GetModuleHandle(NULL), NULL);
    HWND inputNick = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD, INPUT_NICK_FIELD_POS_X, INPUT_NICK_FIELD_POS_Y, INPUT_NICK_FIELD_POS_WIDTH, INPUT_NICK_FIELD_POS_HEIGHT, mainWin, (HMENU)IDR_REGSITRATION_NICKNAME, GetModuleHandle(NULL), NULL);
    HWND contunieBut = CreateWindow(L"Button", L"Продолжить", WS_VISIBLE | WS_CHILD, NICK_BUTTON_CONTINUE_POS_X, NICK_BUTTON_CONTINUE_POS_Y, INPUT_NICK_FIELD_POS_WIDTH, NICK_BUTTON_CONTINUE_POS_HEIGHT, mainWin, (HMENU)IDB_REGISTRATION_NICK_CONTINUE, GetModuleHandle(NULL), NULL);
    SendMessage(desriptInfo, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(inputNick, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(contunieBut, WM_SETFONT, (WPARAM)hFont, TRUE);
    ShowWindow(mainWin, SW_SHOWDEFAULT);
    MSG msg;
    while (IsWindow(mainWin)) 
    {
        if (GetMessage(&msg, mainWin, 0, 0)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}
LRESULT CALLBACK WndNickName(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
        case WM_COMMAND:
        {
            switch (LOWORD(wParam)) 
            {
            case IDB_REGISTRATION_NICK_CONTINUE: 
            {
                HWND hNick = GetDlgItem(hWnd, IDR_REGSITRATION_NICKNAME);
                CONST INT SIZE = 2000;
                WCHAR wNickStr[SIZE];
                CHAR chNickStr[SIZE];
                int len = GetWindowText(hNick, wNickStr, SIZE);
                WideCharToMultiByte(codePage, 0, wNickStr, wcslen(wNickStr)+1, chNickStr, SIZE, 0, 0);
                userInfo.setNickname(chNickStr);
                SendMessage(hWnd, WM_CLOSE, NULL, 0);
                registrationInfo(listenSock);
                if (recieveData(listenSock)) 
                {
                    MessageBox(NULL, L"Учетная запись существует!", L"Инфо!", MB_OK | MB_ICONINFORMATION);
                    authorizationForm();
                }
            }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
ATOM NicknameClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndNickName;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYCHAT));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szNicknameClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

int registrationInfo(SOCKET lSocket) 
{
    CONST INT SIZE = 2000;
    CHAR numberPhone[SIZE];
    CHAR email[SIZE];
    CHAR nickname[SIZE];
    strcpy_s(numberPhone, SIZE, userInfo.numberPhone());
    strcpy_s(email, SIZE, userInfo.email());
    strcpy_s(nickname, SIZE, userInfo.nickname());
    CHAR regist[SIZE] = "REG ";
    strcat_s(regist, numberPhone);
    strcat_s(regist, "; ");
    strcat_s(regist, email);
    strcat_s(regist, "; ");
    strcat_s(regist, nickname);
    strcat_s(regist, ";");
    int iResult = send(lSocket, regist, strlen(regist), 0);
    if (iResult == INVALID_SOCKET) 
    {
        MessageBox(NULL, L"Ошибка отправки данных", L"Ошибка", MB_OK | MB_ICONERROR);
    }
    return 0;
}

LRESULT CALLBACK WndExtraInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_COMMAND: 
    {
        switch (LOWORD(wParam))
        {
        case IDB_ENTERING_MAIL_SKIP:
        {
            SendMessage(hWnd, WM_CLOSE, NULL, 0);
            nicknameWindow();
        }
        break;
        case IDB_ENTERING_MAIL_ACCEPT:
        {
            HWND hText = GetDlgItem(hWnd, IDR_REGISTRATION_REG_MAIL);
            CONST INT SIZE = 1024;
            CHAR chEMail[SIZE];
            WCHAR wEMail[SIZE];
            int len = GetWindowText(hText, wEMail, 1024);
            WideCharToMultiByte(codePage, 0, wEMail, wcslen(wEMail) + 1, chEMail, SIZE, NULL, NULL);
            if (checkingEMail(chEMail) == 1)
            {
                MessageBox(NULL, L"Неправильно введена почта", L"Ошибка", MB_OK | MB_ICONERROR);
                SetWindowText(GetDlgItem(hWnd, IDR_REGISTRATION_REG_MAIL), L"");
            }
            else 
            {
                len = strlen(chEMail);
                userInfo.setEMail(chEMail);
                SendMessage(hWnd, WM_CLOSE, NULL, 0);
                nicknameWindow();
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
        WORD notificationCode = HIWORD(wParam);
        //WORD - unsigned short
        WORD controlID = LOWORD(wParam);
        if (notificationCode == EN_CHANGE) 
        {
            switch (controlID) 
            {
            case IDR_SEARCH_FIELD:
                accoutSearch(GetDlgItem(hWnd, IDR_SEARCH_FIELD), GetDlgItem(hWnd, IDM_MAIN_USER_LIST));
                break;
            }
        }
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDB_ABOUT_PROGRAM:
            CreatingAuthorWindow(hWnd);
            break;
        case IDB_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDB_ADD_USER:
            addUser();
            if (updateList(GetDlgItem(hWnd, IDM_MAIN_USER_LIST)) == 1) 
            {
                return 1;
            }
            break;
        case IDB_REGISTER_REGIST:
        {
            WCHAR wcPhone[USERSIZE];
            CHAR buffer[USERSIZE];
            if (connectToServ()) 
            {
                return 1;
            }
            GetWindowText(GetDlgItem(hWnd, IDR_REGISTRATION_REG_PHONE), wcPhone, USERSIZE);
            WideCharToMultiByte(codePage, 0, wcPhone, wcslen(wcPhone) + 1, buffer, USERSIZE, NULL, NULL);
            if (checkingNumberPhone(buffer) == 1)
            {
                MessageBox(NULL, L"Пользователь ввёл неправелный номер", L"Ошибка", MB_OK | MB_ICONERROR);
                SetWindowText(GetDlgItem(hWnd, IDR_REGISTRATION_REG_PHONE), L"");
            }
            else 
            {
                INT len = strlen(buffer);
                userInfo.setNumberPhone(buffer);
                SendMessage(hWnd, WM_CLOSE, 0, NULL);
                AddAdditionalInfo();
            }
        }
        break;
        case IDM_MAIN_USER_LIST:
        {
            if (HIWORD(wParam) == LBN_SELCHANGE)
            //LBN_SELCHANGE - работает когда при выборе мышки мы нажимаем левую кнопку мышки
            {
                HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_STRING, IDB_MODIFY_USER, L"Изменить");
                //AppendMenu - добавляет список popup menu новые слова
                //hMENU - handle hmenu
                //uFlags - сюда пишем команду которая нужно выполнить
                //uIDNewItem - id объекта
                //lpNewItem - название объекта
                AppendMenu(hMenu, MF_STRING, IDB_DELETE_USER, L"Удалить");
                POINT pos;
                GetCursorPos(&pos);
                //GetCursorPos - сохраняет положение мышки и сохраняет данные в объекте структуры POINT
                SetForegroundWindow(hWnd);
                //SetForegroundWindow - выводит окно POOPUP на передний план
                int num = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pos.x, pos.y, 0, hWnd, NULL);
                //TrackPopupMenu - отображает контекстное POPUP меню 
                //TPM_RETURNCMD - вернуть выбранный номер
                switch (num) 
                {
                case IDB_MODIFY_USER:
                    classModUserInfo(hWnd, SendMessage(GetDlgItem(hWnd, IDM_MAIN_USER_LIST), LB_GETCURSEL, 0, 0));
                    if (updateList(GetDlgItem(hWnd, IDM_MAIN_USER_LIST)) == 1) 
                    {
                        return 1;
                    }
                    break;
                case IDB_DELETE_USER:
                    deleteUser(SendMessage(GetDlgItem(hWnd, IDM_MAIN_USER_LIST), LB_GETCURSEL, 0, 0));
                    //SendMessage(GetDlgItem(hWnd, IDM_USER_LIST), LB_GETCURSEL, 0, 0) - конструкция 
                    //чтобы получить id/индекс выбранного пользователя
                    //LB_GETCURSEL - флаг на получение индекса пользователя из дескриптора и всё это
                    //выполняется через SendMessage работающий с системой Windows
                    if (updateList(GetDlgItem(hWnd, IDM_MAIN_USER_LIST)) == 1) 
                    {
                        return 1;
                    }
                    break;
                }
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_NOTIFY: 
    {
        HWND tabCtrl = GetDlgItem(hWnd, IDB_REGISTER_TABCONTROL);
        int num = TabCtrl_GetCurSel(tabCtrl);
        if (num == 0)
        {
            ShowWindow(GetDlgItem(hWnd, IDM_REGISTER_PHONE), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDM_REGISTER_EMAIL), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDB_REGISTER_REGIST), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDB_REGISTER_ACCEPT), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDR_REGISTRATION_REG_PHONE), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDR_REGISTRATION_ACCEPT_PHONE), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDR_REGISTRATION_ACCEPT_EMAIL), SW_HIDE);
        }
        else
        {
            ShowWindow(GetDlgItem(hWnd, IDM_REGISTER_PHONE), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDM_REGISTER_EMAIL), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDB_REGISTER_REGIST), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDB_REGISTER_ACCEPT), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDR_REGISTRATION_REG_PHONE), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDR_REGISTRATION_ACCEPT_PHONE), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDR_REGISTRATION_ACCEPT_EMAIL), SW_SHOW);
        }
    }
    break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
LRESULT CALLBACK AboutProgram(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
//LRESULT — это целочисленное значение, которое программа возвращает в Windows. Он содержит 
//ответ вашей программы на определенное сообщение. Значение этого значения зависит от кода 
//сообщения. 
{
    switch (message) 
    {
        case WM_COMMAND:
            switch (LOWORD(wParam)) 
            {
            case IDB_ABOUT_PROG_BUTTON_CANCEL:
                SendMessage(hWnd, WM_CLOSE, 0, NULL);
                //Msg (второй параметр) - вписываем нужную команду сообщение 
                //Список комманд - в Windows Notifications
                break;
            }
        break;
        default: 
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
}
