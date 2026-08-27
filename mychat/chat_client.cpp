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

//winUI3

//Глобальные переменные:
CONST INT arraySize = 2000;
CONST INT USERSIZE = 2000;
CONST INT IDSIZE = 1000;
CONST UINT codePage = 1251;                     //UINT - unsigned int
INT userId = 0;
SOCKET listenSock = INVALID_SOCKET;
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

//ATOM Class
ATOM UserWndProcClass(HINSTANCE hInstance);
ATOM AuthorizationClass(HINSTANCE hInstance);
ATOM AuthorizationFormClass(HINSTANCE hInstance);
ATOM AdditionalInfoClass(HINSTANCE hInstance);
ATOM AddingEntryClass(HINSTANCE hInstance);
ATOM NicknameClass(HINSTANCE hInstance);

//Window functions
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndAuthorizationForm(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndExtraInfo(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK UserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutProgram(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndNickName(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

INT createUserWndProc();
INT checkTables();
INT checkingUserInfo(HWND hWnd);
INT modifyUserInfo(HWND hWnd);
INT accoutSearch(HWND hField, HWND hList);
INT updateList(HWND userList);
INT recievedRegData(CHAR* recvBuf);
INT addUser();
INT deleteUser(INT idx);
CHAR* checkPlusInPhone(const char* numPhone);

class UserInfo
{
private:
    CHAR m_numberPhone[arraySize];
    CHAR m_email[arraySize];
    CHAR m_nickname[arraySize];
    CHAR m_birthdayDay[arraySize];
    CHAR m_birthdayMonth[arraySize];
    CHAR m_birthdayYear[arraySize];
public:
    UserInfo() : m_numberPhone(""), m_email(""), m_nickname(""), m_birthdayDay(""), m_birthdayMonth(""), m_birthdayYear("") {};
public:
    void setNumberPhone(CHAR* numberPhone);
    CHAR* numberPhone();
    void setEmail(CHAR* email);
    CHAR* email();
    void setNickname(CHAR* nickname);
    CHAR* nickname();
    void setBirthdayDay(CHAR* userBirthdayDay);
    CHAR* birthdayDay();
    void setBirthdayMonth(CHAR* userBirthdayMonth);
    CHAR* birthdayMonth();
    void setBirthdayYear(CHAR* userBirthdayYear);
    CHAR* birthdayYear();
};

void UserInfo::setNumberPhone(CHAR* userPhone) 
{
    strcpy_s(m_numberPhone, userPhone);
}

CHAR* UserInfo::numberPhone() 
{
    return m_numberPhone;
}

void UserInfo::setEmail(CHAR* email) 
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

void UserInfo::setBirthdayDay(CHAR* userBirthdayDay) 
{
    strcpy_s(m_birthdayDay, userBirthdayDay);
}

CHAR* UserInfo::birthdayDay() 
{
    return m_birthdayDay;
}

void UserInfo::setBirthdayMonth(CHAR* userBirthdayMonth) 
{
    strcpy_s(m_birthdayMonth, userBirthdayMonth);
}

CHAR* UserInfo::birthdayMonth() 
{
    return m_birthdayMonth;
}

void UserInfo::setBirthdayYear(CHAR* userBirthdayYear) 
{
    strcpy_s(m_birthdayYear, userBirthdayYear);
}

CHAR* UserInfo::birthdayYear() 
{
    return m_birthdayYear;
}

UserInfo userInfo;
INT authorizationForm();
INT insertEntry(HWND hwnd);
INT checkExistsEMail(HWND hWnd);
INT checkExistsNumPhone(HWND hWnd);
VOID writtingDownLog(const WCHAR* record);
INT recieveData(SOCKET clientSocket);
INT classModUserInfo(HWND hWnd, INT idx);
INT registrationInfo(SOCKET listenSock);
INT connectToServ();
INT disconnectFromServ(SOCKET listenSock);
//Точка входа APIENTRY
//APIENTRY - это как и CALLBACK, фукнция обратного вызова
INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYCHAT, szWindowClass, MAX_LOADSTRING);
    UserWndProcClass(hInstance);
    AuthorizationClass(hInstance);
    AuthorizationFormClass(hInstance);
    AdditionalInfoClass(hInstance);
    AddingEntryClass(hInstance);
    NicknameClass(hInstance);
    checkTables();
    // Выполнить инициализацию приложения:
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
LRESULT CALLBACK ModifyUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDB_MOD_BUTTON_OK:
        {
            modifyUserInfo(hWnd);
            SendMessage(hWnd, WM_CLOSE, 0, NULL);
        }
        break;
        case IDB_CANCELING_USER_MOD:
        {
            SendMessage(hWnd, WM_CLOSE, 0, NULL);
        }
        break;
        }
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
                if (connectToServ()) 
                {
                    return 1;
                }
                insertEntry(hWnd);
                SendMessage(hWnd, WM_CLOSE, 0, NULL);
                CONST INT SIZE = 1024;
                WCHAR wcNumPhone[SIZE];
                GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_PHONE), wcNumPhone, SIZE);
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

INT checkingEMail(CHAR* email) 
{
    if(!strcmp(email, ""))
    {
        MessageBox(NULL, L"Была обнаружена пустая строка\nв поле ввода почты!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    INT numAt = 0;
    INT len = strlen(email);
    if (len == 0) 
    {
        return 0;
    }
    for (int i = 0; i < len; ++i) 
    {
        if (email[i] == '@') 
        {
            numAt++;
        }
        if (((email[i] < '0') || (email[i] > '9')) && ((email[i] < 'A') || (email[i] > 'Z') )
            && ((email[i] < 'a') || (email[i] > 'z')) && (email[i] != '.') && (email[i] != '_')
            && (email[i] != '-') && (email[i] != '@'))
        {
            MessageBox(NULL, L"Ошибка ввода!\nНеверный формат почты", L"Ошибка", MB_OK | MB_ICONERROR);
            return 1;
        }
    }
    if (numAt > 1) 
    {
        MessageBox(NULL, L"Ошибка ввода!\nДолжен быть только один знак @", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    else if (numAt == 0) 
    {
        MessageBox(NULL, L"Ошибка ввода!\nНе обнаружен знак @", L"Ошибка", MB_OK | MB_ICONERROR);
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
    //INT len = static_cast<INT>(strlen(strPhone));
    INT len = strlen(strPhone);
    INT i = 0;
    if (strPhone[0] == '+') 
    {
        for (int i = 0; i < len; i++) 
        {
            strPhone[i] = strPhone[i + 1];
        }
    }
    INT posOpenParet = -1;
    INT posCloseParet = -1;
    BOOL hasDash = FALSE;
    //len = static_cast<INT>(strlen(strPhone));
    len = strlen(strPhone);
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
        hasDash = FALSE;
        posDash = 0;
        for (int i = 0; i < len; ++i)
        {
            if (strPhone[i] == '-')
            {
                posDash = i;
                hasDash = TRUE;
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
    len = strlen(strPhone);
    if (len != 11) 
    {
        MessageBox(NULL, L"Неправельный размер номера телефона", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}
INT updateList(HWND userList) 
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
    const char* getUserList = "SELECT * FROM contacts";
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
INT checkingUserInfo(HWND hWnd) 
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
    INT len = GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_NICKNAME), data, SIZE);
    //Первый аргумент - это дескрптор дескриптор, из этого дескриптора мы получаем строку, и размер 
    //строки который сохраняется в отдельной int переменной.
    //GetWindowText нужен чтобы получить саму строку которую мы записываем в переменную вторым аргументом 
    //и её длину. Длину мы получаем как отдельное число записываем int переменную.
    if (len == 0) 
    {
        MessageBox(NULL, L"Не введено имя пользователя!", L"Ошибка", MB_OK | MB_ICONERROR);
        //MessageBox(NULL, L"Не введена фамилия!", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
}
INT checkExistsEMail(HWND hWnd) 
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
INT checkExistsNumPhone(HWND hWnd) 
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
    INT counter = 0;
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
INT insertEntry(HWND hWnd)
{
    WCHAR wNickname[USERSIZE];
    /*WCHAR wLastName[USERSIZE];
    WCHAR wFirstName[USERSIZE];
    WCHAR wMiddleName[USERSIZE];*/
    WCHAR numbrerPhone[USERSIZE];
    WCHAR email[USERSIZE];
    WCHAR userId[IDSIZE];
    CONST INT SIZECOMMAND = 12000;
    CHAR command[SIZECOMMAND];
    CHAR buffer[USERSIZE];
    INT number = 0;
    LPCTSTR errMes;
    //UINT - безнаковый целочисленный тип числа
    //(GetDlgItem(hWnd, IDM_ADD_MENU_LAST_NAME), wLastName, USERSIZE);
    //GetWindowText - функция которая копирует строку из дескриптора окна в переменную,
    //с размером который мы указываем в поле buffer, последний параметр
    //GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_FIRST_NAME), wFirstName, USERSIZE);
    //GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_MIDDLE_NAME), wMiddleName, USERSIZE);
    GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_PHONE), numbrerPhone, USERSIZE);
    GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_EMAIL), email, USERSIZE);
    //MessageBox(NULL, wLastName, L"INFO", MB_OK | MB_ICONERROR);
    //MessageBox(NULL, wFirstName, L"INFO", MB_OK | MB_ICONERROR);
    const char lsUsrId[] = "SELECT MAX(USER_ID) FROM users";
    sqlite3* db;
    INT res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res)
    {
        MessageBox(NULL, L"Ошибка подключения к базе данных!", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, lsUsrId, -1, &stmt, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(stmt);
        if (curRow == SQLITE_ROW)
        {
            number = sqlite3_column_int(stmt, 0) + 1;
            //+1 - получаем следующий id;
            //Второй аргумент номер колонки из которой берём значение;
        }
    }
    sqlite3_finalize(stmt);
    strcpy_s(command, "INSERT INTO contacts (contact_id, nickname, phone, email) VALUES(");
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
    GetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_NICKNAME), wNickname, USERSIZE);
    WideCharToMultiByte(codePage, 0, wNickname, wcslen(wNickname)+1, buffer, USERSIZE, NULL, NULL);
    strcat_s(command, buffer);
    strcat_s(command, "',");
    strcat_s(command, "'");
    WideCharToMultiByte(codePage, 0, numbrerPhone, wcslen(numbrerPhone)+1, buffer, USERSIZE, NULL, NULL);
    CONST INT SIZE = 2000;
    if (checkingNumberPhone(buffer) == 1)
    {
        SetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_PHONE), L"");
    }
    strcat_s(command, buffer);
    strcat_s(command, "',");
    strcat_s(command, "'");
    checkExistsNumPhone(hWnd);
    WideCharToMultiByte(codePage, 0, email, wcslen(email)+1, buffer, USERSIZE, NULL, NULL);
    if (checkingEMail(buffer) == 1) 
    {
        SetWindowText(GetDlgItem(hWnd, IDM_ADD_MENU_EMAIL), L"");
    }
    strcat_s(command, buffer);
    strcat_s(command, "');");
    checkExistsEMail(hWnd);
    //wsprintfA - записывает в переменную идущую первым аргументом в формате ANSI.
    char *msg = NULL;
    try {
        sqlite3_busy_timeout(db, 5000);
        //sqlite3_exec(db, "BEGIN IMMEDIATE", NULL, NULL, &msg);
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
INT modifyUserInfo(HWND hWnd)
{
    sqlite3* db;
    INT res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res) 
    {
        MessageBox(NULL, L"База данных не подключена", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    CONST INT SIZE = 2000;
    CONST INT NUMSIZE = 256;
    WCHAR wcNickname[SIZE];
    WCHAR wcPhone[SIZE];
    WCHAR wcEmail[SIZE];
    CHAR chNickname[SIZE];
    CHAR chPhone[SIZE];
    CHAR chEMail[SIZE];
    CHAR command[SIZE];
    WCHAR wNum[NUMSIZE];
    CHAR chNum[NUMSIZE];
    CHAR buffer[SIZE];
    const char* updateData = "UPDATE contacts SET nickname = '";
    strcpy_s(command, updateData);
    GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_NICKNAME), wcNickname, SIZE);
    GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_PHONE), wcPhone, SIZE);
    GetWindowText(GetDlgItem(hWnd, IDR_MOD_MENU_EMAIL), wcEmail, SIZE);
    WideCharToMultiByte(codePage, 0, wcNickname, wcslen(wcNickname) + 1, chNickname, SIZE, NULL, NULL);
    WideCharToMultiByte(codePage, 0, wcPhone, wcslen(wcPhone) + 1, chPhone, SIZE, NULL, NULL);
    strcpy_s(buffer, checkPlusInPhone(chPhone));
    WideCharToMultiByte(codePage, 0, wcEmail, wcslen(wcEmail) + 1, chEMail, SIZE, NULL, NULL);
    strcat_s(command, chNickname);
    strcat_s(command, "', phone = '");
    strcat_s(command, buffer);
    strcat_s(command, "', email = '");
    strcat_s(command, chEMail);
    strcat_s(command, "' WHERE contact_id = ");
    wsprintf(wNum, L"%d\0", userId);
    WideCharToMultiByte(codePage, 0, wNum, wcslen(wNum) + 1, chNum, NUMSIZE, NULL, NULL);
    strcat_s(command, chNum);
    strcat_s(command, ";");
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

INT deleteUser(INT idx) 
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
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, command, -1, &stmt, NULL) == SQLITE_OK) 
    {
        INT nextRow = sqlite3_step(stmt);
        if (nextRow == SQLITE_ROW) 
        {
            INT id = sqlite3_column_int(stmt, 0);
            //Второй аргумент номер колонки из которой берём значение;
            const char* delReq = "DELETE FROM contacts WHERE user_id = ";
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
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
CHAR* checkPlusInPhone(const char* numPhone) 
{
    CONST INT SIZE = 1024;
    CHAR arr[1024]{};
    if (numPhone[0] != '+') 
    {
         strcpy_s(arr, "+");
         strcat_s(arr, numPhone);
    }
    else if (numPhone[0] == '+') 
    {
        int i = 1;
        int p = 0;
        while (numPhone[i] != '\0') 
        {
            arr[p] = numPhone[i];
            i++;
            p++;
        }
    }
    return arr;
}

INT classModUserInfo(HWND hWnd, INT idx) 
{
    if (idx == -1) 
    {
        return 1;
    }
    sqlite3* db;
    INT res = sqlite3_open("DatabaseMessanger.db", &db);        //sqlite_open - открывает если файл найден или если файл не найден, тогда создааёт его
    if (res) 
    {
        MessageBox(NULL, L"База данных не подключена", L"Ошибка", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        return 1;
    }
    const char* selId = "SELECT contact_id FROM contacts LIMIT 1 OFFSET ";
    //SELECT user_id FROM users LIMIT 1 OFFSET - сдвиг на какое количество
    //записей и получение id записи, которая будет единственной благодаря
    //ключу LIMIT 
    CONST INT SIZE = 2000;
    CHAR command[SIZE];
    strcpy_s(command, selId);
    CONST INT IDXSIZE = 256;
    WCHAR wcId[IDXSIZE];
    CHAR chId[IDXSIZE];
    wsprintf(wcId, L"%d\0", idx);
    WideCharToMultiByte(codePage, 0, wcId, wcslen(wcId) + 1, chId, IDXSIZE, NULL, NULL);
    strcat_s(command, chId);
    strcat_s(command, ";");
    sqlite3_stmt* stGetId;
    HWND userClass = NULL;
    if (sqlite3_prepare_v2(db, command, -1, &stGetId, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(stGetId);
        if (curRow == SQLITE_ROW)
        {
            sqlite3_stmt* stUserInfo;
            INT changeUserId = sqlite3_column_int(stGetId, 0);
            //sqlite3_column_int - берет строку из stmt и преобразует строку в integer значение, записывая
            //в выделенную int переменную;
            //Второй аргумент номер колонки из которой берём значение;
            userId = changeUserId;
            const char* dataUser = "SELECT nickname, phone, email FROM contacts WHERE contact_id = ";
            //!SQLITE все работает через запросы которые мы собираем в переменной формата const char*
            WCHAR wId[IDXSIZE]{};
            CHAR chId[IDXSIZE]{};
            wsprintf(wId, L"%d\0", userId);
            WideCharToMultiByte(codePage, 0, wId, wcslen(wId), chId, IDXSIZE, NULL, NULL);
            strcpy_s(command, dataUser);
            strcat_s(command, chId);
            strcat_s(command, ";");
            if (sqlite3_prepare_v2(db, command, -1, &stUserInfo, NULL) == SQLITE_OK)
            {
                INT curRow = sqlite3_step(stUserInfo);
                //sqlite3_step возвращает значение в sqlite3_stmt, а sqlite3_exec просто выполняет запрос
                if (curRow == SQLITE_ROW) 
                {
                    const char* chNickname = reinterpret_cast<const char*>(sqlite3_column_text(stUserInfo, 0));
                    const char* chPhone = reinterpret_cast<const char*>(sqlite3_column_text(stUserInfo, 1));
                    const char* chEmail = reinterpret_cast<const char*>(sqlite3_column_text(stUserInfo, 2));
                    CHAR buffer[SIZE]{};
                    strcpy_s(buffer, checkPlusInPhone(chPhone));
                    WCHAR wcNickname[SIZE];
                    WCHAR wcPhone[SIZE];
                    WCHAR wcEMail[SIZE];
                    MultiByteToWideChar(codePage, 0, chNickname, strlen(chNickname) + 1, wcNickname, SIZE);
                    MultiByteToWideChar(codePage, 0, chEmail, strlen(chEmail) + 1, wcEMail, SIZE);
                    MultiByteToWideChar(codePage, 0, buffer, strlen(buffer) + 1, wcPhone, SIZE);
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
                    userWnd.lpszClassName = szInfoModificationClass;
                    ATOM reg = RegisterClassEx(&userWnd);
                    userClass = CreateWindow(szInfoModificationClass, L"Измененить", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, MODIFY_CLASS_WIDTH, MODIFY_CLASS_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
                    HFONT hFont = CreateFont(FONT_THE_REGISTRATION_WINDOW, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
                    HWND hNickname = CreateWindow(L"STATIC", L"Имя:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_MOD_POS_X(-10), DESCRIPT_FIELD_MOD_POS_Y(-10), MOD_DESCRIPT_FIELD_WIDTH(80), DESCRIPT_FIELD_MOD_HEIGHT(0), userClass, NULL, NULL, GetModuleHandle(NULL), NULL);
                    HWND hPhone = CreateWindow(L"STATIC", L"Телефон:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_MOD_POS_X(-10), DESCRIPT_FIELD_MOD_POS_Y(30), MOD_DESCRIPT_FIELD_WIDTH(80), DESCRIPT_FIELD_MOD_HEIGHT(0), userClass, NULL, NULL, GetModuleHandle(NULL), NULL);
                    HWND hEMail = CreateWindow(L"STATIC", L"Почта:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_MOD_POS_X(-10), DESCRIPT_FIELD_MOD_POS_Y(70), MOD_DESCRIPT_FIELD_WIDTH(80), DESCRIPT_FIELD_MOD_HEIGHT(0), userClass, NULL, NULL, GetModuleHandle(NULL), NULL);
                    HWND hNickInputFld = CreateWindow(L"EDIT", wcNickname, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X(100), MODIFY_BUTTON_EDIT_POS_Y(-10), MOD_INPUT_FIELD_WIDTH(140), MOD_INPUT_FIELD_HEIGHT(0), userClass, (HMENU)IDR_MOD_MENU_NICKNAME, GetModuleHandle(NULL), NULL);
                    HWND hPhoneInputFld = CreateWindow(L"EDIT", wcPhone, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X(100), MODIFY_BUTTON_EDIT_POS_Y(30), MOD_INPUT_FIELD_WIDTH(140), MOD_INPUT_FIELD_HEIGHT(0), userClass, (HMENU)IDR_MOD_MENU_PHONE, GetModuleHandle(NULL), NULL);
                    HWND hEmailInputFld =  CreateWindow(L"EDIT", wcEMail, WS_VISIBLE | WS_CHILD | WS_BORDER, MODIFY_BUTTON_EDIT_POS_X(100), MODIFY_BUTTON_EDIT_POS_Y(70), MOD_INPUT_FIELD_WIDTH(140), MOD_INPUT_FIELD_HEIGHT(0), userClass, (HMENU)IDR_MOD_MENU_EMAIL, GetModuleHandle(NULL), NULL);
                    HWND hBtnOK = CreateWindow(L"BUTTON", L"ОК", WS_VISIBLE | WS_CHILD | WS_BORDER, MOD_BUTTON_POS_X(100), MOD_BUTTON_POS_Y(170), MOD_BUTTON_WIDTH(30), MOD_BUTTON_HEIGHT(0), userClass, (HMENU)IDB_MOD_BUTTON_OK, GetModuleHandle(NULL), NULL);
                    HWND hBtnCanc = CreateWindow(L"BUTTON", L"Отмена", WS_VISIBLE | WS_CHILD | WS_BORDER, MOD_BUTTON_POS_X(170), MOD_BUTTON_POS_Y(170), MOD_BUTTON_WIDTH(60), MOD_BUTTON_HEIGHT(0), userClass, (HMENU)IDB_CANCELING_USER_MOD, GetModuleHandle(NULL), NULL);
                    SendMessage(hNickname, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(hPhone, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(hEMail, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(hNickInputFld, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(hPhoneInputFld, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(hEmailInputFld, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(hBtnOK, WM_SETFONT, (WPARAM)hFont, TRUE);
                    SendMessage(hBtnCanc, WM_SETFONT, (WPARAM)hFont, TRUE);
                    ShowWindow(userClass, SW_SHOWDEFAULT);
                    EnableWindow(hWnd, FALSE);
                }
                sqlite3_finalize(stUserInfo);
                //stmt - обязательно очищать от старых данных, чтобы во время другого запрооса не получить ошибку: "database is locked".
            }
        }
        sqlite3_finalize(stGetId);
    }
    sqlite3_close(db);
    MSG msg;
    if (userClass)
    {
        while (IsWindow(userClass))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    EnableWindow(hWnd, TRUE);
    SetActiveWindow(hWnd);
    return 0;
}

INT addUser() 
{
    HWND userClass = CreateWindow(szUserAccountClassName, L"Добавить", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, MAIN_FIELD_WIDTH, MAIN_FIELD_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    //HWND hLastName = CreateWindow(L"STATIC", L"Фамилия:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_X(0), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    //HMENU - внутренее поле в котором мы можем храннить вписанный текст
    HFONT fontTitle = CreateFont(FONT_THE_REGISTRATION_WINDOW, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    //FW_NORMAL - указывает тип шрифта (жирный, полужирный и тд.)
    //Italic - отвечает: true (шрифт наклоненный), fasle (шрифт не наклоненный). Как курсив в microsft word.
    //StrikeOut - отвечает: true (шрифт зачеркнут), false (шрифт не зачеркнут).
    HWND hNickname = CreateWindow(L"STATIC", L"Имя:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_Y(0), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    HWND hPhone = CreateWindow(L"STATIC", L"Телефон:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_Y(40), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    HWND hMail = CreateWindow(L"STATIC", L"Почта:", WS_VISIBLE | WS_CHILD, DESCRIPT_FIELD_POS_X, COUNT_FIELD_POS_Y(80), DESCRIPT_FIELD_WIDTH, DESCRIPT_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    HWND hNickInputFld = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_Y(0), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_ADD_MENU_NICKNAME, GetModuleHandle(NULL), NULL);
    HWND hPhoneInputFld = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_Y(40), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_ADD_MENU_PHONE, GetModuleHandle(NULL), NULL);
    HWND hEmailInputFld = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_FIELD_POS_X, COUNT_FIELD_POS_Y(80), INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, userClass, (HMENU)IDM_ADD_MENU_EMAIL, GetModuleHandle(NULL), NULL);
    HWND hBtnOK = CreateWindow(L"BUTTON", L"Ок", WS_VISIBLE | WS_CHILD | WS_BORDER, ACCEPT_BUTTON_POS_X, ACCEPT_BUTTON_POS_Y, ACCEPT_BUTTON_WIDTH, ACCEPT_BUTTON_HEIGHT, userClass, (HMENU)IDB_GIVE_CONSENT_USER_ADD, GetModuleHandle(NULL), NULL);
    HWND hBtnCancel = CreateWindow(L"BUTTON", L"Отмена", WS_VISIBLE | WS_CHILD | WS_BORDER, CANCEL_ADDING_ENTRY_POS_X, CANCEL_ADDING_ENTRY_POS_Y, CANCEL_ADDING_ENTRY_WIDTH, CANCEL_ADDING_ENTRY_HEIGHT, userClass, (HMENU)IDB_CANCELLING_USER_ADD, GetModuleHandle(NULL), NULL);
    //Приравнивание ресурса к HMENU нужно для всех типов окон с которым юзер взаимодействует: нажатие клавиши, ввод в поле и так далее.  
    SendMessage(hNickname, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hPhone, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hMail, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hNickInputFld, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hPhoneInputFld, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hEmailInputFld, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hBtnCancel, WM_SETFONT, (WPARAM)fontTitle, TRUE);
    SendMessage(hBtnOK, WM_SETFONT, (WPARAM)fontTitle, TRUE);
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
    return 0;
}

INT connectToServ()
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

INT disconnectFromServ(SOCKET listenSock) 
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
ATOM AddingEntryClass(HINSTANCE hInstance) 
{
    WNDCLASSEX wcex;
    //WNDCLASSEXW - Содержит сведения о классе окна.
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = AddNewUserWndProc;
    //lpfnWndProc - указатель на зарегестрированное и созданное окно.
    wcex.cbClsExtra = 0;
    //Выделение дополнительных байт для класса после его регистрации, память 
    //будет привязана к самому классу, а не к конкретному окну.
    wcex.cbWndExtra = 0;
    //Выделение дополнительных байт для каждого окна, так как каждое окно будет уникальным,
    //выделена память будет сохранена для окна пока оно будет существовать.
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_MYCHAT));;
    wcex.hCursor = LoadCursor(hInst, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szUserAccountClassName;
    return RegisterClassExW(&wcex);
}

ATOM AuthorizationFormClass(HINSTANCE hInstance) 
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndAuthorizationForm;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYCHAT));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWndAthorizationClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

ATOM UserWndProcClass(HINSTANCE hInstance)
{
    //ATOM - 2 байтовых WORD.
    WNDCLASSEX wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX);
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

INT getNumberStr(CONST WCHAR** array) 
{
    INT count = 0;
    while (array[count++][0] != L'\0');
    return count-1;
}

INT addAdditionalInfo() 
{
    HWND mainWin = CreateWindow(szAdditionalInfoClass, L"My Chat", WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, ADDITIONAL_INFO_WIDTH, ADDITIONAL_INFO_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    HFONT hFont = CreateFont(FONT_THE_REGISTRATION_WINDOW, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    HWND extraMail = CreateWindow(L"STATIC", L"Укажите почту, как дополнительное средство для связи:", WS_CHILD | WS_VISIBLE, DESCRIPT_EMAIL_AS_EXTRA_INFO_POS_X, DESCRIPT_EMAIL_AS_EXTRA_INFO_POS_Y, DESCRIPT_EMAIL_AS_EXTRA_INFO_POS_WIDTH, DESCRIPT_EMAIL_AS_EXTRA_INFO_POS_HEIGHT, mainWin, NULL, GetModuleHandle(NULL), NULL);
    HWND emailInput = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, EMAIL_INPUT_FIELD_POS_X, EMAIL_INPUT_FIELD_POS_Y, EMAIL_INPUT_FIELD_POS_WIDTH, EMAIL_INPUT_FIELD_POS_HEIGHT, mainWin, (HMENU)IDR_REGISTRATION_MAIL, GetModuleHandle(NULL), NULL);
    HWND skipButton = CreateWindow(L"BUTTON", L"Пропустить", WS_CHILD | WS_VISIBLE, EMAIL_SKIP_BUTTON_POS_X, EMAIL_SKIP_BUTTON_POS_Y, EMAIL_SKIP_BUTTON_POS_WIDTH, EMAIL_INPUT_FIELD_POS_HEIGHT, mainWin, (HMENU)IDB_ENTERING_MAIL_SKIP, GetModuleHandle(NULL), NULL);
    HWND accessButton = CreateWindow(L"BUTTON", L"Принять", WS_CHILD | WS_VISIBLE, EMAIL_ACCESS_BUTTON_POS_X, EMAIL_ACCESS_BUTTON_POS_Y, EMAIL_ACCESS_BUTTON_POS_WIDTH, EMAIL_ACCESS_BUTTON_POS_HEIGHT, mainWin, (HMENU)IDB_ENTERING_MAIL_ACCEPT, GetModuleHandle(NULL), NULL);
    HWND hDataEdit = CreateWindow(L"STATIC", L"Дата Рождения:", WS_CHILD | WS_VISIBLE, EXTRA_REG_DATA_EDIT_POS_X, EXTRA_REG_DATA_EDIT_POS_Y, EXTRA_REG_DATA_EDIT_WIDTH, EXTRA_REG_DATA_EDIT_HEIGHT, mainWin, NULL, hInst, NULL);
    HWND hDyasBox = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWN | WS_VSCROLL | WS_CHILD | WS_VISIBLE, EXTRA_REG_DAY_COMBO_POS_X, EXTRA_REG_DAY_COMBO_POS_Y, EXTRA_REG_DAY_COMBO_WIDTH, EXTRA_REG_DAY_COMBO_HEIGHT, mainWin, (HMENU)IDC_EXTRA_WIN_COMB_DAYS, GetModuleHandle(NULL), NULL);
    //WS_CHILD - указывает (связывает) объект с окном, то есть указывает что объект будет являться частью окна
    HWND hMonthBox = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWN | WS_VSCROLL | WS_CHILD | WS_VISIBLE, EXTRA_REG_MONTH_COMBO_POS_X, EXTRA_REG_MONTH_COMBO_POS_Y, EXTRA_REG_MONTH_COMBO_WIDTH, EXTRA_REG_MONTH_COMBO_HEIGHT, mainWin, (HMENU)IDC_EXTRA_WIN_COMB_MONTHS, GetModuleHandle(NULL), NULL);
    HWND hYearsBox = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWN | WS_VSCROLL | WS_CHILD | WS_VISIBLE, 170, 160, 90, 220, mainWin, (HMENU)IDC_EXTRA_WIN_COMB_YEARS, GetModuleHandle(NULL), NULL);
    SendMessage(extraMail, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(emailInput, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hDataEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hDyasBox, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hMonthBox, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hYearsBox, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(skipButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(accessButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    for (int i = 0; i < 31; ++i) 
    {
        SendMessage(hDyasBox, CB_ADDSTRING, 0, (LPARAM)numberOfDays[i]);
    }
    SendMessage(hDyasBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
    //CB_SETCURSEL - используется для програмного выбора элемента в списке элемента управления «поле со списком» (combo box)
    for (int i = 0; i < 12; ++i) 
    {
        SendMessage(hMonthBox, CB_ADDSTRING, 0, (LPARAM)numberOfMonths[i]);
    }
    SendMessage(hMonthBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
    INT strLength = getNumberStr(numberOfDates);
    for (int i = 0; i < strLength; ++i) 
    {
        SendMessage(hYearsBox, CB_ADDSTRING, 0, (LPARAM)numberOfDates[i]);
    }
    SendMessage(hYearsBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
    ShowWindow(mainWin, SW_SHOWDEFAULT);
    MSG msg;
    while (IsWindow(mainWin)) 
    {
        if (GetMessage(&msg, NULL, 0, 0)) 
        //Второй параметр отвечает за обработку сообщений происходящий в окне, ставим NULL, тем самым указываем что нужно 
        //обрабатывать все сообщения, а не только внутри окна программы
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

INT authorizationForm() 
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
    CONST WCHAR* str[2] = { L"Регистрация", L"Авторизация" };
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
    HWND hNumPhone = CreateWindow(L"STATIC", L"Номер телефона:", WS_CHILD | WS_VISIBLE, REGISTRATION_PHONE_FIELD_POS_X, REGISTRATION_PHONE_FIELD_POS_Y, REGISTRATION_PHONE_FIELD_POS_WIDTH, REGISTRATION_PHONE_FIELD_POS_HEIGHT, registrWin, (HMENU)IDM_REGISTER_PHONE, GetModuleHandle(NULL), NULL);
    HWND hEmail = CreateWindow(L"STATIC", L"Почта:", WS_CHILD, REGISTRATION_EMAIL_DATA_POS_X, REGISTRATION_EMAIL_DATA_POS_Y, REGISTRATION_EMAIL_DATA_WIDTH, REGISTRATION_EMAIL_DATA_HEIGHTS, registrWin, (HMENU)IDM_REGISTER_EMAIL, GetModuleHandle(NULL), NULL);
    HWND hEnteringPhoneByReg = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, INPUT_REGISTERED_PHONE_POS_X, INPUT_REGISTERED_PHONE_POS_Y, INPUT_REGISTERED_PHONE_POS_WIDTH, INPUT_REGISTERED_PHONE_POS_HEIGHT, registrWin, (HMENU)IDR_REGISTRATION_PHONE, GetModuleHandle(NULL), NULL);
    //WS_VISIBLE - при запуске программы делает поле: STATIC, EDIT, BUTTON и тд. видимым в окне программы.
    HWND hEnteringPhoneByLogin = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, INPUT_REGISTERED_PHONE_POS_X, INPUT_REGISTERED_PHONE_POS_Y, INPUT_REGISTERED_PHONE_POS_WIDTH, INPUT_REGISTERED_PHONE_POS_HEIGHT, registrWin, (HMENU)IDR_LOGIN_PHONE, GetModuleHandle(NULL), NULL);
    HWND hEnteringEmailByLogin = CreateWindow(L"EDIT", L"", WS_CHILD | WS_BORDER, 10, 140, 360, 32, registrWin, (HMENU)IDR_LOGIN_EMAIL, GetModuleHandle(NULL), NULL);
    HWND hReg = CreateWindow(L"BUTTON", L"Зарегистрироваться", WS_CHILD | WS_VISIBLE | WS_BORDER, REGISTRATION_BY_PHONE_POS_X, REGISTRATION_BY_PHONE_POS_Y, REGISTRATION_BY_PHONE_POS_WIDTH, REGISTRATION_BY_PHONE_POS_HEIGHT, registrWin, (HMENU)IDB_REGISTER_REGIST, GetModuleHandle(NULL), NULL);
    HWND hAccept = CreateWindow(L"BUTTON", L"Войти", WS_CHILD | WS_BORDER, LOG_IN_TO_REGISTER_POX_X, LOG_IN_TO_REGISTER_POX_Y, LOG_IN_TO_REGISTER_POX_WIDTH, LOG_IN_TO_REGISTER_POX_HEIGHT, registrWin, (HMENU)IDB_REGISTER_ATHORIZ_ACCEPT, GetModuleHandle(NULL), NULL);
    SendMessage(hNumPhone, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hEmail, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hEnteringPhoneByReg, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hEnteringPhoneByLogin, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hEnteringEmailByLogin, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hReg, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hAccept, WM_SETFONT, (WPARAM)hFont, TRUE);
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

//INT getDataFromServ(SOCKET lSocket) 
//{
//    CONST INT SIZE = 1024;
//    CHAR command[SIZE]{};
//    strcpy_s(command, "/getDataFromServ");
//    INT iResult = send(lSocket, command, strlen(command)+1, 0);
//    if (iResult == INVALID_SOCKET) 
//    {
//        MessageBox(NULL, L"Ошибка при отправке данных", L"Ошибка", MB_OK | MB_ICONERROR);
//        return 1;
//    }
//    return 0;
//}

LRESULT CALLBACK UserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) 
        {
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

INT checkTables()
{
    sqlite3* db;
    CONST INT SIZE = 2000;
    INT res = sqlite3_open("DatabaseMessanger.db", &db);            //sqlite_open - открывает если файл найден или если файл не найден, тогда создааёт его
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
    sqlite3_stmt* stmt;
    //sqlite3_stmt - структура где хранится информация таблице которая была создана с помощью sql-запроса, 
    //который находится в const char* переменной
    if (sqlite3_prepare_v2(db, groupTable, -1, &stmt, NULL) == SQLITE_OK)
        //sqlite3_prepare_v2 - создает структур откуда мы будем брать наши результаты
    {
        INT curRow = sqlite3_step(stmt);
        //sqlite3_step - двигаемся по записям из таблицы вынимая, каждую запись
        if (curRow == SQLITE_ROW)
            //SQLITE_ROW - идентификатор строки
            //Если int перменная получает значение sqlite_row, то это значит строка найденна
        {
            INT countRows = sqlite3_column_int(stmt, 0);
            //sqlite3_column_int - выводит текущий индекс колонки
            if (countRows == 0)
            {
                //MessageBox(NULL, L"Ни одной группы не найдено!\nСоздаём новую...", L"Информация", MB_OK | MB_ICONERROR);
                const char* createTable = "CREATE TABLE groups (group_id PRIMARY KEY NOT NULL, group_name TEXT NOT NULL);";
                //char** errorTgroup = mesError;        //Как вариант.
                char* msg = NULL;
                try {
                    INT status = sqlite3_exec(db, createTable, NULL, NULL, &msg);
                    if (status != SQLITE_OK)
                    {
                        MessageBox(NULL, L"Ошибка при создании таблицы группы", L"Ошибка", MB_OK | MB_ICONERROR);
                        throw "SQL-ERROR";
                        
                    }
                    /*else
                    {
                        MessageBox(NULL, L"Таблица группа создана", L"Инфо", MB_OK | MB_ICONINFORMATION);
                    }*/
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
        sqlite3_finalize(stmt);
        //sqlite3_finalize - очищает память от переменной.
    }
    const char* userTable = "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' and name = 'users';";
    if (sqlite3_prepare_v2(db, userTable, -1, &stmt, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(stmt);
        if (curRow == SQLITE_ROW)
        {
            INT countRows = sqlite3_column_int(stmt, 0);
            if (countRows == 0)
            {
                const char* createTable =
                    "CREATE TABLE users ("
                    "user_id INT PRIMARY KEY NOT NULL,"
                    "nickname TEXT NOT NULL,"
                    "phone INTEGER NOT NULL,"
                    "email TEXT NULL,"
                    "birthday date NULL,"
                    "icon BLOB NULL,"
                    "path_icon TEXT NULL,"
                    "BIO TEXT NULL,"
                    "last_login TEXT NULL);";
                char* msg = NULL;
                try {
                    INT status = sqlite3_exec(db, createTable, NULL, NULL, &msg);
                    if (status != SQLITE_OK)
                    {
                        MessageBox(NULL, L"Ошибка при создании таблицы пользователь", L"Ошибка", MB_OK | MB_ICONERROR);
                        throw "SQL-ERROR";
                        
                    }
                    /*else
                    {
                        MessageBox(NULL, L"Таблица пользователь создана", L"Инфо", MB_OK | MB_ICONINFORMATION);
                    }*/
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
        sqlite3_finalize(stmt);
    }
    const char* messageTable = "SELECT COUNT(*) FROM sqlite_master WHERE type ='table' AND name = 'messages';";
    if (sqlite3_prepare_v2(db, messageTable, -1, &stmt, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(stmt);
        if (curRow == SQLITE_ROW)
        {
            INT countRows = sqlite3_column_int(stmt, 0);
            if (countRows == 0)
            {
                //MessageBox(NULL, L"Таблица сообщений не создана! Создаём новую", L"ИНФО", MB_OK | MB_ICONINFORMATION);
                const char* createTable = "CREATE TABLE messages("
                    "message_id INT PRIMARY KEY NOT NULL,"
                    "text_field TEXT NOT NULL,"
                    "file_field BLOB,"
                    "sender INT,"
                    "group_id INT,"
                    "recipent INT,"
                    "FOREIGN KEY (sender) REFERENCES users(user_id),"
                    "FOREIGN KEY (recipent) REFERENCES users(user_id),"
                    "FOREIGN KEY (group_id) REFERENCES groups(groupd_id))";
                char* msg = NULL;
                try {
                    INT status = sqlite3_exec(db, createTable, NULL, NULL, &msg);
                    //Пятый аргумент в sqlite3_exec - записывает ошибку в переменную char указатель (char*) которую мы передали.
                    if (status != SQLITE_OK)
                    {
                        MessageBox(NULL, L"Ошибка при создании таблицы сообщение", L"Инфо", MB_OK | MB_ICONERROR);
                        throw "SQL-ERROR";
                        
                    }
                    /*else
                    {
                        MessageBox(NULL, L"Таблица сообщение создана", L"Инфо", MB_OK | MB_ICONINFORMATION);
                    }*/
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
        sqlite3_finalize(stmt);
    }
    const char* contactListTable = "SELECT COUNT(*) FROM sqlite_master WHERE type ='table' AND name ='contacts';";
    if (sqlite3_prepare_v2(db, contactListTable, -1, &stmt, 0) == SQLITE_OK)
    //const char* zSql, SQL     - выражение(в кодировке UTF - 8)
    //int nByte,                - максимальный размер в байтах строки zSql
    //sqlite3_stmt **ppStmt,    - указатель на компилируемое выражение sqlite3_stmt
    //const char** pzTail       - указатель на неиспользуемую часть zSql
    {
        INT curRow = sqlite3_step(stmt);
        if (curRow == SQLITE_ROW) 
        {
            INT countRows = sqlite3_column_int(stmt, 0);
            if (countRows == 0) 
            {
                //MessageBox(NULL, L"Таблица списка контактов пользователя не была создана! Создаём новую", L"ИНФО", MB_OK | MB_ICONINFORMATION);
                const char* createTable = "CREATE TABLE contacts("
                    "contact_id INT PRIMARY KEY NOT NULL,"
                    "nickname TEXT NOT NULL,"
                    "phone INT NOT NULL,"
                    "email TEXT NULL,"
                    "birth_day TEXT NULL,"
                    "icon BLOB NULL,"
                    "BIO text NULL,"
                    "last_login TEXT NULL);";
                char* msg = NULL;
                try 
                {
                    INT status = sqlite3_exec(db, createTable, NULL, NULL, &msg);
                    if (status != SQLITE_OK) 
                    {
                        MessageBox(NULL, L"Ошибка при создании таблицы контактов пользователя", L"Инфо", MB_OK | MB_ICONERROR);
                        throw ("SQL-ERROR");
                        
                    }
                    /*else 
                    {
                        MessageBox(NULL, L"Таблица списка контактов пользователя создана", L"Инфо", MB_OK | MB_ICONINFORMATION);
                    }*/
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
            }
        }
        sqlite3_finalize(stmt);
        //stmt - обязательно очищать от старых данных, чтобы во время другого запрооса не получить ошибку: "database is locked".
    }
    sqlite3_close(db);
    //Контроль за закрытием и открытием базы данных обязательный иначе получим ошибку: "database is locked".
    return 0;
}

INT accoutSearch(HWND hField, HWND hList) 
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
    sqlite3_stmt* stmt;
    char getUserFirstName[1024] = "SELECT nickname FROM users WHERE nickname LIKE '%";
    strcat_s(getUserFirstName, chStr);
    strcat_s(getUserFirstName, "%'");
    //strcat_s(getUserFirstName, "%' OR last_name LIKE '%");
    /*strcat_s(getUserFirstName, chStr);
    strcat_s(getUserFirstName, "%'");*/
    if (sqlite3_prepare_v2(db, getUserFirstName, -1, &stmt, NULL) == SQLITE_OK) 
    {
        INT curRow;
        while ((curRow = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            strcpy_s(chFirstName,reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            strcpy_s(chLastName, reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
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
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

INT getUrl(CHAR* recvBuf) 
{
    INT i = 0;
    INT p = 0;
    CONST INT SIZE = 1024;
    CHAR command[SIZE]{};
    while (recvBuf[i] != '/') 
    {
        i++;
    }
    i++;
    while (recvBuf[i] != '\0') 
    {
        command[p] = recvBuf[i];
        i++;
        p++;
    }
    if (!strcmp(command, "registration"))
    {
        return 0;
    }
    if (!strcmp(command, "authorization")) 
    {
        return 1;
    }
    /*if (!strcmp(command, "sendDataToUser"))
    {
        return 2;
    }*/
    return -1;
}

INT insertDataToDb(CHAR* id, CHAR* nickname, CHAR* numPhone, CHAR* email) 
{
    sqlite3* db;
    INT res = sqlite3_open("DatabaseMessanger.db", &db);
    if (res)
    {
        MessageBox(NULL, L"База данных не подключена!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    CONST INT SIZE = 1024;
    //"INSERT INTO users (user_id, nickname, phone, email, status) VALUES(
    CHAR command[SIZE]{};
    strcpy_s(command, "INSERT INTO users(user_id, nickname, phone, email,status)VALUES(");
    strcat_s(command, id);
    strcat_s(command, ",");
    strcat_s(command, "'");
    strcat_s(command, nickname);
    strcat_s(command, "'");
    strcat_s(command, ",");
    strcat_s(command, numPhone);
    strcat_s(command, ",");
    strcat_s(command, "'");
    strcat_s(command, email);
    strcat_s(command, "',0");
    strcat_s(command, ");");
    char* msg;
    //"INSERT INTO users(user_id, nickname, number_phone, email)VALUES(2,dog,79324781441,dog@mail.ru);"
    try 
    {
        if(sqlite3_exec(db, command, NULL, NULL, &msg) != SQLITE_OK)
        {
            MessageBox(NULL, L"Ошибка добавления аккаунта в базу данных клиента!", L"Ошибка", MB_OK | MB_ICONERROR);
            throw "SQL-Error";
        }
        msg = cleaningMemory(msg);
        sqlite3_close(db);
        return 0;
    }
    catch (...) 
    {
        CONST INT SIZE = 2000;
        WCHAR mesError[SIZE];
        size_t n_size;
        mbstowcs_s(&n_size, mesError, msg, SIZE);
        msg = cleaningMemory(msg);
        writtingDownLog(mesError);
        return 1;
    }
}

BOOL getStringFromArr(INT& indexI, INT& indexK, CHAR* chSource, CHAR* chDest) 
{
    while (chSource[indexI] != ',' && chSource[indexI] != ';')
    {
        chDest[indexK] = chSource[indexI];
        indexK++;
        indexI++;
    }
    indexI++;
    chDest[indexK] = '\0';
    indexK = 0;
    return TRUE;
}

INT insertServDataToDB(CHAR* recvBuf) 
{
    INT i = 1;
    CONST INT SIZE = 512;
    CHAR id[SIZE], nickname[SIZE], email[SIZE], numPhone[SIZE];
    while (recvBuf[i] != ']')
    {
        INT k = 0;
        getStringFromArr(i, k, recvBuf, id);
        getStringFromArr(i, k, recvBuf, nickname);
        getStringFromArr(i, k, recvBuf, numPhone);
        getStringFromArr(i, k, recvBuf, email);
        insertDataToDb(id, nickname, numPhone, email);
    }
    return 0;
}

INT recievedRegData(CHAR* recvBuf) 
{
    CONST INT SIZE = 1024;
    CHAR command[SIZE]{};
    INT i = 0;
    while (recvBuf[i] != '/') 
    {
        command[i] = recvBuf[i];
        i++;
    }
    if (!strcmp(command, "CREATED"))
    {
        return 0;
    }
    if (!strcmp(command, "EXIST")) 
    {
        return 1;
    }
    return -1;
}
BOOL recievedAuthorizData(CHAR* recvBuf) 
{
    CONST INT SIZE = 1024;
    CHAR command[SIZE]{};
    INT i = 0;
    while (recvBuf[i] != '/') 
    {
        command[i] = recvBuf[i];
        i++;
    }
    if (!strcmp(command, "NOEXIST"))
    {
        return 0;
    }
    if (!strcmp(command, "EXIST"))
        //!res - res == 0
    {
        return 1;
    }
    return -1;
}

INT recieveData(SOCKET clientSocket)
{
    INT iResuslt = 0;
    CONST INT SIZE = 3000;
    CHAR recvBuf[SIZE]{};
    while (iResuslt <= 0)
    {
        iResuslt = recv(clientSocket, recvBuf, SIZE, 0);
        //recv - recieve;
        if (iResuslt < 0)
        {
            MessageBox(NULL, L"Ошибка получения данных", L"Ошибка", MB_OK | MB_ICONERROR);
        }
        INT res = getUrl(recvBuf);
        switch (res) 
        {
        case IDS_REGISTRATION: 
        {
            if (recievedRegData(recvBuf)) 
            {
                return 1;
            }
            return 0;
        }
        break;
        case IDS_AUTHORIZATION: 
        {
            if (!recievedAuthorizData(recvBuf)) 
            {
                //MessageBox(NULL, L"Такой учетной записи не существует.\nСоздайте аккаунт пожалуйста.", L"Ошибка", MB_OK | MB_ICONERROR);
                return 0;
            }
            return 1;
        }
        break;
        default: 
            return 1;
        }       
    }
    return -1;
}

INT CreatingAuthorWindow(HWND hWnd) 
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
    authorWnd.lpszClassName = szInfoAboutProgram;
    ATOM reg = RegisterClassEx(&authorWnd);
    HWND userClass = CreateWindow(szInfoAboutProgram, L"О Программе", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, INFO_ABOUT_PROGRAM_WIDTH, INFO_ABOUT_PROGRAM_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    //WS_OVERLAPPEDWINDOW - добавляет к окну значки закрыть, расширить, свернуть делая окно самостоятельным.
    CreateWindow(L"STATIC", L"Авторы:", WS_VISIBLE | WS_CHILD, ABOUT_AUTHOR_FIELD_POX_X, AUTHOR_FIELD_POS_Y, ABOUT_AUTHOR_FIELD_WIDTH, ABOUT_AUTHOR_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    CreateWindow(L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_AUTOVSCROLL | WS_BORDER, AUTHOR_LIST_POS_X, AUTHOR_FIELD_POS_Y, AUTHOR_LIST_POS_WIDTH, AUTHOR_LIST_POS_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    //WS_BORDER - ключ благодаря которму задаются границы окна.
    //ES_AUTOVSCROLL - автоматическое пермещение по списку если какой-то добавлен или удалён.
    //LBS_NOTIFY - нужен для работы флага LB_GETCURSEL
    //LB_CURSEL - нужен чтобы получить id пользователя по которму мы можем выводить сообщение
    CreateWindow(L"STATIC", L"Версия:", WS_VISIBLE | WS_CHILD, ABOUT_AUTHOR_FIELD_POX_X, ABOUT_VERSION_FIELD_POX_Y, ABOUT_AUTHOR_FIELD_WIDTH, ABOUT_AUTHOR_FIELD_HEIGHT, userClass, NULL, GetModuleHandle(NULL), NULL);
    CreateWindow(L"BUTTON", L"Закрыть", WS_VISIBLE | WS_CHILD | WS_BORDER, ABOUT_BUTTON_CLOSE_POS_X, ABOUT_BUTTON_CLOSE_POS_Y, ABOUT_BUTTON_CLOSE_POS_WIDTH, ABOUT_BUTTON_CLOSE_POS_HEIGHT, userClass, (HMENU)IDB_ABOUT_PROG_BUTTON_CANCEL, GetModuleHandle(NULL), NULL);
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
INT createExtraAthorizWnd() 
{
    HWND hMain = CreateWindow(szWndAthorizationClass, szAthorizationTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 410, 270, NULL, NULL, hInst, NULL);
    HFONT hFont = CreateFont(FONT_THE_REGISTRATION_WINDOW, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    HWND hPhoneDescript = CreateWindow(L"STATIC", L"Номер телефона:", WS_VISIBLE | WS_CHILD, 10, 20, 140, 30, hMain, NULL, GetModuleHandle(NULL), NULL);
    HWND hPhoneInputField = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 60, 360, 35, hMain, (HMENU)IDR_EXTRA_REG_INPUT_PHONE_FIELD, GetModuleHandle(NULL), NULL);
    HWND hMailDescript = CreateWindow(L"STATIC", L"Почта:", WS_VISIBLE | WS_CHILD, 10, 100, 70, 30, hMain, NULL, GetModuleHandle(NULL), NULL);
    HWND hMailInputField = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 140, 360, 35, hMain, (HMENU)IDR_EXTRA_REG_INPUT_EMAIL_FIELD, GetModuleHandle(NULL), NULL);
    HWND hAccept = CreateWindow(L"Button", L"Войти", WS_VISIBLE | WS_CHILD | WS_BORDER, 280, 190, 80, 35, hMain, (HMENU)IDB_EXTRA_REG_ACCEPT, GetModuleHandle(NULL), NULL);
    SendMessage(hPhoneDescript, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hPhoneInputField, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hMailDescript, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hMailInputField, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hAccept, WM_SETFONT, (WPARAM)hFont, TRUE);
    ShowWindow(hMain, SW_SHOWDEFAULT);
    MSG msg;
    while (IsWindow(hMain)) 
    {
        if (GetMessage(&msg, hMain, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

INT createUserWndProc()
{
   HWND hMain = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, MAIN_WINDOW_POSITION_WIDTH, MAIN_WINDOW_POSITION_HEIGHT, NULL, NULL, hInst, NULL);
   CreateWindow(L"STATIC", L"", WS_VISIBLE| WS_CHILD| WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_WANTRETURN | ES_AUTOVSCROLL, DESCRIPT_LIST_FIELD_POS_X, DESCRIPT_LIST_FIELD_POS_Y, DESCRIPT_LIST_FIELD_WIDTH, DESCRIPT_LIST_FIELD_HEIGT, hMain, 0, hInst, NULL);
   CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, MES_FIELD_X, MES_FIELD_Y, MES_FIELD_WIDTH, MES_FIELD_HEIGHT, hMain, 0, hInst, NULL);
   CreateWindow(L"LISTBOX", L"", WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_AUTOVSCROLL | WS_BORDER | LBS_NOTIFY, MAIN_LIST_USERS_POS_X, MAIN_LIST_USERS_POS_Y, MAIN_LIST_USERS_WIDTH, MAIN_LIST_USERS_HEIGHT, hMain, (HMENU)IDM_MAIN_USER_LIST, hInst, NULL);
   if (updateList(GetDlgItem(hMain, IDM_MAIN_USER_LIST)) == 1)
   {
       return 1;
   }
   CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 450, 240, 20, hMain, (HMENU)IDR_SEARCH_FIELD, hInst, NULL);
   CreateWindow(L"BUTTON", L"Отправить", WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_BORDER, SEND_MES_WINDOW_X, SEND_MES_WINDOW_Y, SEND_MES_WINDOW_WIDTH, SEND_MES_WINDOW_HEIGHT, hMain, 0, hInst, NULL);
   CreateWindow(L"BUTTON", L"Поиск", WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_BORDER, SEARCH_BUTTON_POS_X, SEARCH_BUTTON_POS_Y, SEARCH_BUTTON_WIDTH, SEARCH_BUTTON_HEIGHT, hMain, (HMENU)IDB_SEARCH, hInst, NULL);
   //WS-CHILD - не родительское окно
   CreateWindow(L"BUTTON", L"Добавить", WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_BORDER, BUTTON_ADDING_AN_ENTRY_POS_X, BUTTON_ADDING_AN_ENTRY_POS_Y, BUTTON_ADDING_AN_ENTRY_WIDTH, BUTTON_ADDING_AN_ENTRY_HEIGHT, hMain, (HMENU)IDB_ADD_USER, hInst, NULL);
   //recieveData(listenSock);
   ShowWindow(hMain, SW_SHOWDEFAULT);
   UpdateWindow(hMain);
   MSG msg;
   while (IsWindow(hMain)) 
   {
       if (GetMessage(&msg, hMain, 0, 0)) 
       {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
       }
   }
   return 0;
}

INT nicknameWindow()
{
    HWND mainWin = CreateWindow(szNicknameClass, L"My Chat", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, NICK_REGISTRATION_WINDOW_WIDTH, NICK_REGISTRATION_WINDOW_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    HFONT hFont = CreateFont(FONT_THE_REGISTRATION_WINDOW, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
    HWND desriptInfo = CreateWindow(L"STATIC", L"Введите ваше имя: ", WS_VISIBLE | WS_CHILD, DESCRIPT_NICK_FIELD_POS_X, DESCRIPT_NICK_FIELD_POS_Y, DESCRIPT_NICK_FIELD_POS_WIDTH, DESCRIPT_NICK_FIELD_POS_HEIGHT, mainWin, NULL, GetModuleHandle(NULL), NULL);
    HWND inputNick = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, INPUT_NICK_FIELD_POS_X, INPUT_NICK_FIELD_POS_Y, INPUT_NICK_FIELD_POS_WIDTH, INPUT_NICK_FIELD_POS_HEIGHT, mainWin, (HMENU)IDR_REGSITRATION_NICKNAME, GetModuleHandle(NULL), NULL);
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
                WCHAR wNickStr[SIZE]{};
                CHAR chNickStr[SIZE]{};
                INT len = GetWindowText(hNick, wNickStr, SIZE);
                WideCharToMultiByte(codePage, 0, wNickStr, wcslen(wNickStr)+1, chNickStr, SIZE, 0, 0);
                userInfo.setNickname(chNickStr);
                SendMessage(hWnd, WM_CLOSE, NULL, 0);
                registrationInfo(listenSock);
                if (recieveData(listenSock)) 
                {
                    MessageBox(NULL, L"Учетная запись существует!", L"Инфо!", MB_OK | MB_ICONINFORMATION);
                    authorizationForm();
                }
                else 
                {
                    MessageBox(NULL, L"Аккаунт успешно создан!\nВойдите в него.", L"Ифно", MB_OK | MB_ICONINFORMATION);
                    createExtraAthorizWnd();
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

INT registrationInfo(SOCKET lSocket) 
{
    CONST INT SIZE = 1024;
    CHAR numberPhone[SIZE];
    CHAR email[SIZE];
    CHAR nickname[SIZE];
    CHAR day[SIZE];
    CHAR month[SIZE];
    CHAR year[SIZE];
    strcpy_s(numberPhone, SIZE, userInfo.numberPhone());
    strcpy_s(email, SIZE, userInfo.email());
    strcpy_s(nickname, SIZE, userInfo.nickname());
    strcpy_s(day, SIZE, userInfo.birthdayDay());
    strcpy_s(month, SIZE, userInfo.birthdayMonth());
    strcpy_s(year, SIZE, userInfo.birthdayYear());
    CHAR regist[SIZE];
    strcpy_s(regist, numberPhone);
    strcat_s(regist, ",");
    if (strcmp(email, "")) 
    {
        strcat_s(regist, email);
        strcat_s(regist, ",");
    }
    strcat_s(regist, nickname);
    strcat_s(regist, ",");
    strcat_s(regist, day);
    strcat_s(regist, ",");
    strcat_s(regist, month);
    strcat_s(regist, ",");
    strcat_s(regist, year);
    strcat_s(regist, "/");
    strcat_s(regist,"registration");
    INT iResult = send(lSocket, regist, strlen(regist)+1, 0);
    if (iResult == INVALID_SOCKET) 
    {
        MessageBox(NULL, L"Ошибка отправки данных", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}

INT dataLogin(SOCKET lSocket) 
{
    CONST INT SIZE = 1024;
    CHAR numberPhone[SIZE];
    CHAR email[SIZE];
    strcpy_s(numberPhone, SIZE, userInfo.numberPhone());
    //strcpy_s - безопасная функция, мы ставим размер буффера, чтобы корректно 
    //произошло копирования данных при получении зи второго массива в первый
    strcpy_s(email, SIZE, userInfo.email());
    CHAR authoriz[SIZE];
    strcpy_s(authoriz, numberPhone);
    if (!strcmp(email, "")) 
    {
        strcat_s(authoriz, "/");
    }
    else 
    {
        strcat_s(authoriz, ",");
        strcat_s(authoriz, email);
        strcat_s(authoriz, "/");
    }
    strcat_s(authoriz, "login");
    INT iResult = send(lSocket, authoriz, strlen(authoriz), 0);
    if (iResult == INVALID_SOCKET) 
    //Значение INVALID_SOCKET не является допустимым сокетом. 
    {
        MessageBox(NULL, L"Ошибка отправки даных", L"Ошибка", MB_OK |MB_ICONERROR);
        return 1;
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
            CONST INT SIZE = 1024;
            CHAR chEmail[SIZE];
            CHAR chDays[SIZE];
            CHAR chMonths[SIZE];
            CHAR chYears[SIZE];
            WCHAR wcEmail[SIZE];
            WCHAR wcDays[SIZE];
            WCHAR wcMonths[SIZE];
            WCHAR wcYears[SIZE];
            GetWindowText(GetDlgItem(hWnd, IDR_REGISTRATION_MAIL), wcEmail, SIZE);
            GetWindowText(GetDlgItem(hWnd, IDC_EXTRA_WIN_COMB_DAYS), wcDays, SIZE);
            GetWindowText(GetDlgItem(hWnd, IDC_EXTRA_WIN_COMB_MONTHS), wcMonths, SIZE);
            GetWindowText(GetDlgItem(hWnd, IDC_EXTRA_WIN_COMB_YEARS), wcYears, SIZE);
            WideCharToMultiByte(codePage, 0, wcEmail, wcslen(wcEmail) + 1, chEmail, SIZE, NULL, NULL);
            if (checkingEMail(chEmail) == 1)
            {
                //MessageBox(NULL, L"Неправильно введена почта!", L"Ошибка", MB_OK | MB_ICONERROR);
                SetWindowText(GetDlgItem(hWnd, IDR_REGISTRATION_MAIL), L"");
            }
            else 
            {
                //INT len = strlen(chEmail);
                userInfo.setEmail(chEmail);
                WideCharToMultiByte(codePage, 0, wcDays, wcslen(wcDays) + 1, chDays, SIZE, NULL, NULL);
                WideCharToMultiByte(codePage, 0, wcMonths, wcslen(wcMonths) + 1, chMonths, SIZE, NULL, NULL);
                WideCharToMultiByte(codePage, 0, wcYears, wcslen(wcYears) + 1, chYears, SIZE, NULL, NULL);
                userInfo.setBirthdayDay(chDays);
                userInfo.setBirthdayMonth(chMonths);
                userInfo.setBirthdayYear(chYears);
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
LRESULT CALLBACK WndAuthorizationForm(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_COMMAND: 
    {
        switch (LOWORD(wParam)) 
        {
        case IDB_EXTRA_REG_ACCEPT:
        {
            CONST INT SIZE = 1024;
            WCHAR wcNumPhone[SIZE]{};
            WCHAR wcEmail[SIZE]{};
            CHAR chNumPhone[SIZE]{};
            CHAR chEmail[SIZE]{};
            GetWindowText(GetDlgItem(hWnd, IDR_EXTRA_REG_INPUT_PHONE_FIELD), wcNumPhone, SIZE);
            GetWindowText(GetDlgItem(hWnd, IDR_EXTRA_REG_INPUT_EMAIL_FIELD), wcEmail, SIZE);
            WideCharToMultiByte(codePage, 0, wcNumPhone, wcslen(wcNumPhone)+1, chNumPhone, SIZE, NULL, NULL);
            WideCharToMultiByte(codePage, 0, wcEmail, wcslen(wcEmail)+1, chEmail, SIZE, NULL, NULL);
            if (!checkingNumberPhone(chNumPhone) || !checkingEMail(chEmail))
            {
                userInfo.setNumberPhone(chNumPhone);
                userInfo.setEmail(chEmail);
                dataLogin(listenSock);
                if (!recieveData(listenSock)) 
                {
                    MessageBox(NULL, L"Такого аккаунта не существует!", L"Ошибка", MB_OK | MB_ICONERROR);
                }
                else 
                {
                    SendMessage(hWnd, WM_CLOSE, 0, NULL);
                    createUserWndProc();
                }
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
        INT wmId = LOWORD(wParam);
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
            GetWindowText(GetDlgItem(hWnd, IDR_REGISTRATION_PHONE), wcPhone, USERSIZE);
            WideCharToMultiByte(codePage, 0, wcPhone, wcslen(wcPhone) + 1, buffer, USERSIZE, NULL, NULL);
            if (checkingNumberPhone(buffer) == 1)
            {
                MessageBox(NULL, L"Пользователь ввёл неправелный номер", L"Ошибка", MB_OK | MB_ICONERROR);
                SetWindowText(GetDlgItem(hWnd, IDR_REGISTRATION_PHONE), L"");
            }
            else 
            {
                INT len = strlen(buffer);
                userInfo.setNumberPhone(buffer);
                SendMessage(hWnd, WM_CLOSE, 0, NULL);
                addAdditionalInfo();
            }
        }
        break;
        case IDB_REGISTER_ATHORIZ_ACCEPT: 
        {
            if (connectToServ()) 
            {
                return 1;
            }
            CONST INT SIZE = 1024;
            WCHAR wcNumPhone[SIZE];
            WCHAR wcEMAIL[SIZE];
            CHAR chNumPhone[SIZE];
            CHAR chEMAIL[SIZE];
            GetWindowText(GetDlgItem(hWnd, IDR_LOGIN_PHONE), wcNumPhone, SIZE);
            GetWindowText(GetDlgItem(hWnd, IDR_LOGIN_EMAIL), wcEMAIL, SIZE);
            WideCharToMultiByte(codePage, 0, wcNumPhone, wcslen(wcNumPhone)+1, chNumPhone, SIZE, NULL, NULL);
            WideCharToMultiByte(codePage, 0, wcEMAIL, wcslen(wcEMAIL)+1, chEMAIL, SIZE, NULL, NULL);
            if (!checkingNumberPhone(chNumPhone) || !checkingEMail(chEMAIL))
            {
                userInfo.setNumberPhone(chNumPhone);
                userInfo.setEmail(chEMAIL);
                dataLogin(listenSock);
                if (!recieveData(listenSock)) 
                {
                    MessageBox(NULL, L"Такого аккаунта не существует.\nСоздайте аккаунт пожалуйста.", L"Ошибка", MB_OK | MB_ICONERROR);
                }
                else 
                {
                    SendMessage(hWnd, WM_CLOSE, 0, NULL);
                    createUserWndProc();
                }
            }
        }
        break;
        case IDM_MAIN_USER_LIST:
        {
            if (HIWORD(wParam) == LBN_SELCHANGE)
            //LBN_SELCHANGE - работает когда при выборе мышки мы нажимаем левую кнопку мышки
            {
                /*if (LOWORD(wParam) == VK_RBUTTON)
                {*/
                    HMENU hMenu = CreatePopupMenu();
                    AppendMenu(hMenu, MF_STRING, IDB_MODIFY_USER, L"Изменить");
                    //AppendMenu - добавляет список popup menu новые слова
                    //hMENU - handle hmenu
                    //uFlags - сюда пишем команду которая нужно выполнить
                    //uIDNewItem - id объекта
                    //lpNewItem - название объекта
                    AppendMenu(hMenu, MF_STRING, IDB_DELETE_USER, L"Удалить");
                    //TrackPopupMenu(hMenu, TPM_RIGHTALIGN, TPM_TOPALIGN, TPM_RETURNCMD, TPM_LEFTBUTTON, TPM_VERPOSANIMATION,  );
                    POINT pos;
                    GetCursorPos(&pos);
                    //GetCursorPos - сохраняет положение мышки и сохраняет данные в объекте структуры POINT
                    SetForegroundWindow(hWnd);
                    //SetForegroundWindow - выводит окно POOPUP на передний план
                    INT num = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pos.x, pos.y, 0, hWnd, NULL);
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
                //}
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
        INT num = TabCtrl_GetCurSel(tabCtrl);
        if (num == 0)
        {
            ShowWindow(GetDlgItem(hWnd, IDM_REGISTER_PHONE), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDM_REGISTER_EMAIL), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDB_REGISTER_REGIST), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDB_REGISTER_ATHORIZ_ACCEPT), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDR_REGISTRATION_PHONE), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDR_LOGIN_PHONE), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDR_LOGIN_EMAIL), SW_HIDE);
        }
        else
        {
            ShowWindow(GetDlgItem(hWnd, IDM_REGISTER_PHONE), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDM_REGISTER_EMAIL), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDB_REGISTER_REGIST), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDB_REGISTER_ATHORIZ_ACCEPT), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDR_REGISTRATION_PHONE), SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, IDR_LOGIN_PHONE), SW_SHOW);
            ShowWindow(GetDlgItem(hWnd, IDR_LOGIN_EMAIL), SW_SHOW);
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
    return 0;
}
