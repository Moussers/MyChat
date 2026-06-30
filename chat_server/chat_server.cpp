#include "framework.h"
#include "resource.h"
#include <WrittingDownLog.h>
#include <CleaningMemory.h>
#define SQL_SERVER_PASSWORD			cat3O4E& 

#pragma comment(lib, "wsock32.lib")

#define MAIN_CLASS_HEIGHT 500
#define MAIN_CLASS_WIDTH 500
#define USER_FIELD_POS_X 20 
#define LOG_FIELD_POS_X 200
#define INFO_FIELD_POS_Y 30 
#define INFO_FIELD_HEIGHT 100 
#define INFO_FIELD_WIDTH 60
#define LISTBOX_FIELD_POS_X 10
#define INFO_DISPLAY_FIELD_POS_Y 200
#define OUTPUT_FIELD_POS_Y 50
#define INFO_DISPLAY_FIELD_WIDTH 260
#define LISTBOX_FIELD_WIDTH 150
#define OUTPUT_FIELD_HEIGHT 340
#define MAIN_BUTTON_DELETE_POS_X 10
#define MAIN_BUTTON_CLEAR_LOG_POS_X 200
#define BUTTON_POS_Y 400
#define MAIN_BUTTON_DELETE_WIDTH 70
#define MAIN_BUTTON_CLEAR_LOG_WIDTH 100
#define BUTTON_HEIGHT 25
#define PORT "4000"
#define MAX_CONNECTIONS 100

VOID startServer(HWND log);
VOID appendToLog(HWND log, CONST WCHAR* message);
VOID listenClient();
VOID clientManagement(SOCKET* clientSocket);
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);
//Прототип функции - внизу пишем его расширенную версию
//LRESULT CALLBACK - функция самовызова;
HINSTANCE hInst;
//hInst - для окон
CONST WCHAR MAIN_CLASS_NAME[] = L"MainClassWIND";
//Пространство (указатель) где будут записываться наши окна, 
//хранит информацию обо всех наших окна;
//HINSTANCE hInstance – дескриптор экземпляра приложения. Этот дескриптор 
//содержит адрес начала кода программы в ее адресном пространстве. Дескриптор 
//hInstance чаще всего требуется функциям, работающим с ресурсами программы;
SOCKET listenSocket = INVALID_SOCKET; 
bool listenNewClient = false;
WSADATA wsaData;
//Структура WSADATA содержит информацию о реализации Windows Sockets.
struct addrinfo* result, * ptr, hints;
//Инициализиция WinSOCK через функцию WSAStartup
//hints - для сети

HWND logHWND;

VOID startServer(HWND log);
int checkTables();
int mysqlConnect();
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmd)
//_In_ - для передачи входящих параметров в функцию;
//_In_opt_ - для передачи входящих параметров в функцию;
//APIENTRY - это __stdcal функции стандартного вызова
{ 
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	//ZeroMemory - очищает память WNDCLASSEX заполняя массив нулями.
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	//Поле обозначения при запуске программы для иконки в вверху программы
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszClassName = MAIN_CLASS_NAME;
	wcex.hIconSm = NULL;
	//hIconSm - Поле обозначения при запуске программы для иконки внизу, то есть 
	//в системном трее.
	ATOM atomClass = RegisterClassExW(&wcex);
	hInst = hInstance;
	HWND hMainWnd = CreateWindow(MAIN_CLASS_NAME, L"Сервер", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, MAIN_CLASS_HEIGHT, MAIN_CLASS_HEIGHT, NULL, NULL, hInstance, NULL);
	//lpParam - флаг через который мы указываем особые параметры, указываем NULL потому что особые парметры пока не нужны
	CreateWindow(L"STATIC", L"Пользователи:", WS_VISIBLE | WS_CHILD, USER_FIELD_POS_X, INFO_FIELD_POS_Y, INFO_FIELD_HEIGHT, INFO_FIELD_WIDTH, hMainWnd, NULL, hInstance, NULL);
	CreateWindow(L"STATIC", L"Лог сервера:", WS_VISIBLE | WS_CHILD, LOG_FIELD_POS_X, INFO_FIELD_POS_Y, INFO_FIELD_HEIGHT, INFO_FIELD_WIDTH, hMainWnd, NULL, hInstance, NULL);
	CreateWindow(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, LISTBOX_FIELD_POS_X, OUTPUT_FIELD_POS_Y, LISTBOX_FIELD_WIDTH, OUTPUT_FIELD_HEIGHT, hMainWnd, (HMENU)IDM_MAIN_MENU_USER_LISTS, hInstance, NULL);
	logHWND = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL, INFO_DISPLAY_FIELD_POS_Y, OUTPUT_FIELD_POS_Y, INFO_DISPLAY_FIELD_WIDTH, OUTPUT_FIELD_HEIGHT, hMainWnd, (HMENU)IDM_MAIN_MENU_LOG_FIELD, hInstance, NULL);
    //ES_MULTILINE - многосточный ввод текста, каждая строка (char массив) начинается с новой строки
    //в элементе управления отображается максимально возможное количество строк, а при нажатии клавиши ENTER, когда больше 
    //строк не помещается, раздается звуковой сигнал.
	CreateWindow(L"Button", L"Удалить", WS_VISIBLE | WS_CHILD | WS_BORDER, MAIN_BUTTON_DELETE_POS_X, BUTTON_POS_Y, MAIN_BUTTON_DELETE_WIDTH, BUTTON_HEIGHT, hMainWnd, (HMENU)IDB_MAIN_BUTTON_DELETE, hInstance, NULL);
	CreateWindow(L"Button", L"Очистить лог", WS_VISIBLE | WS_CHILD | WS_BORDER, MAIN_BUTTON_CLEAR_LOG_POS_X, BUTTON_POS_Y, MAIN_BUTTON_CLEAR_LOG_WIDTH, BUTTON_HEIGHT, hMainWnd, (HMENU)IDB_MAIN_BUTTON_CLEAR_LOG, hInstance, NULL);
	ShowWindow(hMainWnd, SW_SHOWDEFAULT);
    UpdateWindow(hMainWnd);
    mysqlConnect();
    startServer(logHWND);
	MSG msg;
	while (IsWindow(hMainWnd)) 
	{
		if (GetMessage(&msg, hMainWnd, 0, 0));
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

VOID clearLog(HWND log) 
{
    CONST INT SIZE = 100;
    WCHAR str[SIZE] = {};
    wsprintf(str, L"%s", L"");
    HWND hClr = GetDlgItem(log, IDB_MAIN_BUTTON_CLEAR_LOG);
    SetWindowText(log, str);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /*if (!listenNewClient) 
    {
        listenNewClient = true;
        CreateThread(NULL, 2048, (LPTHREAD_START_ROUTINE)listenClient, NULL, 0, NULL);
    }*/
	switch (message) 
	{
	case WM_COMMAND:
	{
		switch (LOWORD(message)) 
		{
		case IDB_MAIN_BUTTON_DELETE:
			break;
		case IDB_MAIN_BUTTON_CLEAR_LOG:
            clearLog(hWnd);
			break;
		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
 
int mysqlConnect() 
{
    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* connect;
        driver = sql::mysql::get_mysql_driver_instance();
        connect = driver->connect("tcp://localhost:3306", "root", SQL_SERVER_PASSWORD);
        connect->setSchema("serv_db");
    }
    catch (sql::SQLException& ex) 
    {
        //Выводим сообщение об ошибке
        CONST INT SIZE = 1024;
        WCHAR str[SIZE];
        MultiByteToWideChar(1251, 0, ex.what(), strlen(ex.what()) + 1, str, SIZE);
        //ex.what() - возвращает строку с ошибкой.
        std::wstring n_str = str;
        std::wstring wstr = L"JDBC Error: " + n_str;
        MessageBox(NULL, wstr.c_str(), L"Ошибка", MB_OK | MB_ICONERROR);
        //c_str - получить указатель на строку.
        //c_str - Получить массив символов (char array).

    }
    return 0;
}

int checkTables() 
{
    sqlite3* db;
    CONST INT SIZE = 256;
    //char* mesError[SIZE];
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
                    "email TEXT NULL,"
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
VOID startServer(HWND log) 
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    //WSAStartup - инициализирует SOCKET
    //WSAStartup - инициализирует данные о сокете.
	if (iResult != 0)
	{
        WCHAR err[1024] = {};
		wsprintf(err, L"Ошибка инициализации данных сокета %d", iResult);
        appendToLog(log, err);
        //log - указатель на ресурс окна, то есть дескриптор окна
        //err - массив который содержит текст ошибки
		return;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	//ai_family указывает на то, что вызывающая сторона будет принимать только семейства
	//адресов AF_INET и AF_INET6. 
	hints.ai_socktype = SOCK_STREAM;
	//ai_socktype указывает на то, что вызывающая сторона будет принимать любой тип сокета.
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, PORT, &hints, &result);      //IP адрес сервера
    //getaddrinfo — это версия функции в стандарте ANSI, которая обеспечивает независимый от 
    //протокола перевод имени хоста в адрес. 
	if (iResult != 0)
	{
        WCHAR err[1024] = {};
		wsprintf(err, L"Ошибка получения серверного адреса %d", iResult);
        appendToLog(log, err);
		WSACleanup();
		return;
	}
    else 
    {
        appendToLog(log, L"Адрес успешно получен");
    }
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    //socket - создаем сокет и открываем порт, чтобы получать и отправлять данные по нему
	if (listenSocket == INVALID_SOCKET) 
	{
        WCHAR err[1024] = {};
		wsprintf(err, L"Ошибка создания сокета %d", WSAGetLastError());
		WSACleanup();
		freeaddrinfo(result);
		return;
	}
    else 
    {
        appendToLog(log, L"Сокет успешно создан");
    }
    iResult = bind(listenSocket, result->ai_addr, result->ai_addrlen);
    if (iResult == SOCKET_ERROR) 
    {
        WCHAR err[1024] = {};
        wsprintf(err, L"Ошибка при привязке сокета: %d", WSAGetLastError());
        appendToLog(log, err);
        freeaddrinfo(result);
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
        //Присваиваем сокету значение INVALID_SOCKET, как обычному массиву null\nullprt,
        //чтобы сокет не был открыт.
        return;
    }
    else 
    {
        appendToLog(log, L"Сокет успешно привязан");
    }
    freeaddrinfo(result);
    if (listen(listenSocket, MAX_CONNECTIONS) == SOCKET_ERROR)
    //listen номер 1: -  функция преключает в состояние прослушивания сокет который мы указывем
    //первым аргументом.
    //listen номер 2: - Функция listen переводит сокет в состояние ожидания входящего соединения.
    //Второй аршумент - колличество возможных подключенний.
    {
        WCHAR err[1024] = {};
        wsprintf(err, L"Ошибка режима прослушивания: %d", WSAGetLastError());
        appendToLog(log, err);
        freeaddrinfo(result);
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
        return;
    }
    else 
    {
        appendToLog(log, L"Сокет готов к прослушиванию");
    }
}
VOID appendToLog(HWND log, CONST WCHAR* message) 
{
    INT length = GetWindowTextLength(log);
    //GetWindowTextLength - функция получает длину строки дескриптора и 
    //записывает число в int переменную
    SendMessage(log, EM_SETSEL, (WPARAM)length, (LPARAM)length);
    //Ставим указатель в конец строки
    //EM_SETSEL - устанавливает указатель на конец строки куда и будем писать
    SendMessage(log, EM_REPLACESEL, FALSE, (LPARAM)message);
    //Записываем текст в конце строки
    //EM_REPLACEDSEL - добавдяет дополниетльный текст в элемент управления
    //Третьим ппараметром задаём FALSE, для флага EM_REPLACESEL нельзя отменить 
    //операцию замены текста
    //WM_SETTEXT - использщуется для того чтобы полностью заменить старый текст 
    //на новый, то есть перезаписать текст на новый
    SendMessage(log, EM_REPLACESEL, FALSE, (LPARAM)L"\n");
    //В конце добавляем новую строку для следующего текста.
}
VOID listenClient() 
{
    do {
        if (listenSocket != INVALID_SOCKET)
        {
            SOCKET clientSocket = INVALID_SOCKET;
            appendToLog(logHWND, L"Ждём  подключение клиента ...");
            clientSocket = accept(listenSocket, NULL, NULL);
            //accept - функция прослушивает сам сокет ждет пока кто-то другой подключится к нему.
            //второй параметр - альтернативный адрес.
            ///второй параметр - необязательный указатель на буфер, в который записывается адрес 
            //подключаемого объекта, известный коммуникационному уровню.
            if (clientSocket == INVALID_SOCKET)
            {
                listenNewClient = false;
                appendToLog(logHWND, L"Не удалось подключиить клиента");
            }
            else
            {
                listenNewClient = false;
                clientManagement(&clientSocket);
            }
        }
        else
        {
            listenNewClient = false;
        }
    } while (true);
}

VOID clientManagement(SOCKET* clientSocket) 
{
    char* receiveBuf = (char*)malloc(sizeof(char) * 1024);
    if (receiveBuf == NULL)
    {
        appendToLog(logHWND, L"Не хватает памяти для пользователя.");
        closesocket(*clientSocket);
        //* - операция разыменования указателя
        //closescoket - закрывает существующий сокет и освобождает память.
        *clientSocket = INVALID_SOCKET;
        return;
    }
    //Запускаем прослушивание клиента
    while (*clientSocket != INVALID_SOCKET) 
    {
        int iResult = recv(*clientSocket, receiveBuf, strlen(receiveBuf), NULL);
        if (iResult > 0)
        {
            appendToLog(logHWND, L"Сообщение успешно успешно получено");
        }
        else if (iResult == 0) 
        {
            appendToLog(logHWND, L"Соединенение с клиентом зарыто");
            closesocket(*clientSocket);
            *clientSocket = INVALID_SOCKET;
        }
        else 
        {
            appendToLog(logHWND, L"Ошибка соединения клиента");
            closesocket(*clientSocket);
            *clientSocket = INVALID_SOCKET;
        }
    }
    free(receiveBuf);
}