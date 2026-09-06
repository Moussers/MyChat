#include "framework.h"
#include "resource.h"
#include <WrittingDownLog.h>
#include <CleaningMemory.h>
#define SQL_SERVER_PASSWORD			"cat3O4E&" 

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

void startServer(HWND log);
void appendToLog(HWND log, CONST WCHAR* message);
void listenClient();
void clearLog(HWND log);
bool getSubDataFromStr(int* indexI, int* indexK, WCHAR* wcSource, WCHAR* wcDestin);
int clientManagement(SOCKET* clientSocket);
int checkTables(HWND log);
int mysqlConnect();
bool sendDataByAuthor(SOCKET* clientSocket, WCHAR* wcPhone, WCHAR* wcEmail, WCHAR* wcNickname, CHAR* status);
bool sendDataByReg(SOCKET* clientSocket, WCHAR* wcPhone, WCHAR* wcEmail, WCHAR* wcNickname, CHAR* status);
bool checkRegEntry(SOCKET* clientSocket, CHAR* recvBuf);
void checkAuthorizEntry(SOCKET* clientSocket, CHAR* recvBuf);
int checkExistEmail(WCHAR* email);
void checkContactData(SOCKET lSocket, char* recvBuf);
bool insertEntry(WCHAR* wcNumPhone, WCHAR* wcEmail, WCHAR* wcFirstName, WCHAR* wcLastName,  WCHAR* wcDay, WCHAR* wcMonth, WCHAR* wcYear);
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
enum ActionsAtServer {REGISTRATION = 0, AUTHORIZATION = 1, CHECKING_CONTACT_DATA = 2};
SOCKET listenSocket = INVALID_SOCKET; 
bool listenNewClient = false;
bool serverIsReady = false;
bool login = false;
int codePage = 1251;
WSADATA wsaData;
//Структура WSADATA содержит информацию о реализации Windows Sockets.
struct addrinfo* result, * ptr, hints;
//Инициализиция WinSOCK через функцию WSAStartup
//hints - для сети

HWND logHWND;
sql::Connection* connection;
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
    checkTables(GetDlgItem(hMainWnd, IDM_MAIN_MENU_LOG_FIELD));
    startServer(logHWND);
	MSG msg;
	while (IsWindow(hMainWnd)) 
	{
		if (GetMessage(&msg, hMainWnd, 0, 0));
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void clearLog(HWND log) 
{
    CONST INT SIZE = 100;
    WCHAR str[SIZE] = {};
    wsprintf(str, L"%s", L"");
    HWND hClr = GetDlgItem(log, IDB_MAIN_BUTTON_CLEAR_LOG);
    SetWindowText(log, str);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!listenNewClient && serverIsReady) 
    //serverIsReady - serverIsReady != 0, т.е. serverIsReady == 1
    {
        listenNewClient = true;
        CreateThread(NULL, 2048, (LPTHREAD_START_ROUTINE)listenClient, NULL, 0, NULL);
    }
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

//Подключение к серверу
int mysqlConnect() 
{
    try 
    {
        sql::mysql::MySQL_Driver* driver;
        driver = sql::mysql::get_mysql_driver_instance();
        connection = driver->connect("tcp://127.0.0.1:3306", "root", SQL_SERVER_PASSWORD);
        sql::Statement* stmt = connection->createStatement();  /*sql::Statement - хранит таблицу*/
        std::string crt = "CREATE DATABASE IF NOT EXISTS serv_db";
        stmt->execute(crt);
        connection->setSchema("serv_db");      /*Подключаемся к базе*/
        std::string nms = "SET NAMES 'cp1251'";
        stmt->execute(nms);
        delete stmt;
    }
    catch (sql::SQLException& ex) 
    {
        //Выводим сообщение об ошибке
        CONST INT SIZE = 1024;
        WCHAR str[SIZE];
        //const char* err = ex.what();
        MultiByteToWideChar(1251, 0, ex.what(), strlen(ex.what()) + 1, str, SIZE);
        //ex.what() - возвращает строку с ошибкой.
        std::wstring n_str = str;
        std::wstring wstr = L"JDBC Error: " + n_str;
        //MessageBox(NULL, wstr.c_str(), L"Ошибка", MB_OK | MB_ICONERROR);
        appendToLog(logHWND, wstr.c_str());
        //c_str - получить указатель на строку.
        //c_str - Получить массив символов (char array).
        return 1;
    }
    return 0;
}

int checkTables(HWND log) 
{
    try {
        if (connection->isClosed())
        //Метод isClosed() не пингует сервер, чтобы определить, доступен ли он. Согласно спецификации JDBC,
        //он возвращает true только в том случае, если для соединения был вызван метод closed().
        {
            mysqlConnect();
        }
        sql::Statement* stmt = connection->createStatement();       /*CreateStatement - создание объекта Statement для дальнейших запросов*/
        std::string sql_ex = "SELECT COUNT(*) FROM information_schema.tables WHERE table_schema ='serv_db' AND table_name='groups';";
        sql::ResultSet* res = stmt->executeQuery(sql_ex);
        res->next();    /*переход на следующую строку*/
                        /*sql.ResultSet перемещает курсор на одну строку вперед от его текущего положения в объекте ResultSet. В основном*/
                        /*он используется для перебора строк, возвращаемых SQL - запросом.*/
        int count = res->getInt(1);     /*1 - выбрать из первой колонки количество таблиц*/
        if (!count)
        {
            appendToLog(log, L"Таблицы не былы созданы! Создаём новые таблицы.");
            std::string createGroups = "CREATE TABLE `groups` ("
                "group_id INT AUTO_INCREMENT PRIMARY KEY NOT NULL,"
                "group_name TEXT NOT NULL);";
            if (stmt->execute(createGroups))
            //if(stmt->execute(createTable) == true)
            {
                appendToLog(log, L"Ошибка пр создании таблицы группы!");
                delete stmt;
                //stmt - удаляем stmt чтобы осовдодить ресурсы. 
                return 1;
            }
            std::string createUsers = "Create TABLE `users`("
                "user_id INT PRIMARY KEY NOT NULL,"
                "first_name varchar(256) NOT NULL,"
                "last_name varchar(256) NULL,"
                "number_phone BIGINT NOT NULL,"
                "email TEXT NULL,"
                "birthday DATE NULL,"
                "icon BINARY NULL,"
                "path_icon TEXT,"
                "BIO TEXT NULL,"
                "last_login DATETIME NULL);";
            if (stmt->execute(createUsers)) 
            {
                appendToLog(log, L"Ошибка пр создании таблицы юзер!");
                delete stmt;
                return 1;
            }
            std::string createMessages = "Create TABLE `messages`("
                "message_id INT PRIMARY KEY NOT NULL,"
                "text_field TEXT NOT NULL,"
                "file_field BINARY,"
                "sender INT,"
                "`group` INT,"
                "recipent INT,"
                "FOREIGN KEY (sender) REFERENCES `users`(user_id),"
                "FOREIGN KEY (recipent) REFERENCES users(user_id),"
                "FOREIGN KEY (`group`) REFERENCES `groups`(group_id));";
            if (stmt->execute(createMessages))
            {
                appendToLog(log, L"Ошибка при создание таблицы сообщение!");
                delete stmt;
                return 1;
            }
            std::string createContactList = "Create Table `contacts`("
                "conctact_id INT PRIMARY KEY NOT NULL,"
                "nickname VARCHAR(256),"
                "number_phone INT NOT NULL,"
                "email TEXT NULL,"
                "icon BINARY NULL);";
                "last_login DATETIME NULL);";
            if (stmt->execute(createContactList))
            {
                appendToLog(log, L"Ошибка создания таблицы group");
                delete stmt;
                return 1;
            }
        }
        delete stmt;
        connection->close();
        //закрываем соединение;
        return 0;
    }
    catch (sql::SQLException& ex) 
    {
        //Выводим сообщение об ошибке
        CONST INT SIZE = 1024;
        WCHAR str[SIZE];
        //const char* err = ex.what();
        MultiByteToWideChar(1251, 0, ex.what(), strlen(ex.what()) + 1, str, SIZE);
        //ex.what() - возвращает строку с ошибкой.
        std::wstring n_str = str;
        std::wstring wstr = L"JDBC Error: " + n_str;
        appendToLog(log, wstr.c_str());
        //MessageBox(NULL, wstr.c_str(), L"Ошибка", MB_OK | MB_ICONERROR);
        //c_str - получить указатель на строку.
        //c_str - Получить массив символов (char array).
        return 1;
    } 
}

void startServer(HWND log) 
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
    serverIsReady = true;
}

void appendToLog(HWND log, CONST WCHAR* message) 
{
    int length = GetWindowTextLength(log);
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

void listenClient() 
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

bool insertEntry(WCHAR* wcNumPhone, WCHAR* wcFirstName, WCHAR* wcLastName, WCHAR* wcEmail,  WCHAR* wcDay, WCHAR* wcMonth, WCHAR* wcYear)
{
    if (connection->isClosed()) 
    {
        mysqlConnect();
    }
    CONST int SIZE = 2000;
    try 
    {
        //падает если в массиве команды есть имя на русском
        WCHAR wcId[SIZE]{};
        CHAR chId[SIZE]{};
        CHAR chFirstName[SIZE]{};
        CHAR chLastName[SIZE]{};
        CHAR chNumPhone[SIZE]{};
        CHAR chEmail[SIZE]{};
        CHAR chDay[SIZE]{};
        CHAR chMonth[SIZE]{};
        CHAR chYear[SIZE]{};
        CHAR command[SIZE] = "SELECT MAX(user_id) FROM users";
        sql::Statement* stmt = connection->createStatement();
        sql::ResultSet* res = stmt->executeQuery(command);
        res->next();
        int id = res->getInt(1) + 1;
        wsprintf(wcId, L"%d", id);
        WideCharToMultiByte(codePage, 0, wcId, wcslen(wcId) + 1, chId, SIZE, NULL, NULL);
        strcpy_s(command, "INSERT INTO users(user_id, first_name, last_name, number_phone, email, birthday) VALUES('");
        WideCharToMultiByte(codePage, 0, wcFirstName, wcslen(wcFirstName) + 1, chFirstName, SIZE, NULL, NULL);        
        strcat_s(command, chId);
        strcat_s(command, "','");
        strcat_s(command, chFirstName);
        strcat_s(command, "','");
        WideCharToMultiByte(codePage, 0, wcLastName, wcslen(wcLastName) + 1, chLastName, SIZE, NULL, NULL);
        strcat_s(command, chLastName);
        strcat_s(command, "','");
        WideCharToMultiByte(codePage, 0, wcNumPhone, wcslen(wcNumPhone) + 1, chNumPhone, SIZE, NULL, NULL);
        strcat_s(command, SIZE, chNumPhone);
        strcat_s(command, "','");
        WideCharToMultiByte(codePage, 0, wcEmail, wcslen(wcEmail) + 1, chEmail, SIZE, NULL, NULL);
        strcat_s(command, chEmail);
        strcat_s(command, "','");
        WideCharToMultiByte(codePage, 0, wcYear, wcslen(wcYear) + 1, chYear, SIZE, NULL, NULL);
        strcat_s(command, chYear);
        strcat_s(command, "-");
        WideCharToMultiByte(codePage, 0, wcMonth, wcslen(wcMonth) + 1, chMonth, SIZE, NULL, NULL);
        strcat_s(command, chMonth);
        strcat_s(command, "-");
        WideCharToMultiByte(codePage, 0, wcDay, wcslen(wcDay) + 1, chDay, SIZE, NULL, NULL);
        strcat_s(command, chDay);
        strcat_s(command, SIZE, "');");
        //MultiByteToWideChar(codePage, 0, chCommand, strlen(chCommand) + 1, wcCommand, SIZE);
        stmt = connection->createStatement();
        stmt->execute(command);
        //executeQuery - при выполнении возвращает результат, а при insert результат не нужен
        connection->close();
        delete stmt;
        return true;
    }
    catch (sql::SQLException ex) 
    {
        WCHAR errors[SIZE]{};
        MultiByteToWideChar(codePage, 0, ex.what(), strlen(ex.what()+1), errors, SIZE);
        appendToLog(logHWND, errors);
        connection->close();
        return false;
    }   
}

int checkExistPhone(WCHAR* numberPhone) 
{
    if (connection->isClosed()) 
    //!connection..isClosed() - connection.isClosed() == 0
    //connection.isClosed() - connection.isClosed() != 0, т.е connection.isClosed() == 1
    {
        mysqlConnect();
    }
    CONST INT SIZE = 1024;
    CONST WCHAR sqlReq[] = L"SELECT COUNT(*) FROM users WHERE number_phone='";
    WCHAR wTmp[SIZE]{};
    CHAR chTmp[SIZE];
    wcscpy_s(wTmp, sqlReq);
    wcscat_s(wTmp, numberPhone);
    wcscat_s(wTmp, L"'");
    int count = 0;
    try
    {
        sql::Statement* stmt = connection->createStatement();
        WideCharToMultiByte(codePage, 0, wTmp, wcslen(wTmp) + 1, chTmp, SIZE, NULL, NULL);
        sql::ResultSet* res = stmt->executeQuery(chTmp);
        //execute - выполнение без возвращения результата
        //executeQuery - выполнение с возварщением результата
        //В объекте ResultSet итератор устаналивается на позиции перед первой строкой. 
        //И чтобы переместиться к первой строке (и ко всем последующим) необходимо вызвать 
        //метод next(). Пока в наборе ResultSet есть доступные строки, метод next будет 
        //возвращать true. 
        res->next();
        count = res->getInt(1);
        //В mySql массиве счёт начинается с единицы, а не с нуля как в обычном массиве.
        connection->close();
        return count;
    }
    catch (sql::SQLException& ex)
    {
        WCHAR errors[SIZE];
        MultiByteToWideChar(1251, 0, ex.what(), strlen(ex.what()) + 1, errors, SIZE);
        appendToLog(logHWND, errors);
        connection->close();
        return -1;
    }
}

int checkExistEmail(WCHAR* email) 
{
    if (connection->isClosed()) 
    {
        mysqlConnect();
    }
    CONST INT SIZE = 1024;
    int count = 0;
    CONST WCHAR sqlReq[] = L"SELECT COUNT(*) FROM users WHERE email='";
    WCHAR wTmp[SIZE]{};
    CHAR chTmp[SIZE];
    wcscpy_s(wTmp, sqlReq);
    wcscat_s(wTmp, email);
    wcscat_s(wTmp, L"'");
    wcscat_s(wTmp, L";");
    try 
    {
        sql::Statement* stmt = connection->createStatement();
        WideCharToMultiByte(codePage, 0, wTmp, wcslen(wTmp) + 1, chTmp, SIZE, NULL, NULL);
        sql::ResultSet* res = stmt->executeQuery(chTmp);
        res->next();
        count = res->getInt(1);
        connection->close();
        return count;
    }
    catch (sql::SQLException ex)
    {
        WCHAR errors[SIZE];
        MultiByteToWideChar(codePage, 0, ex.what(), strlen(ex.what())+1, errors, SIZE);
        appendToLog(logHWND, errors);
        connection->close();
        return -1;
    }
}

bool getSubDataFromStr(int* indexI, int* indexK, WCHAR* wcSource, WCHAR* wcDest)
{
    if (wcSource[*indexI] == L'/' || wcSource[*indexI] == L' ')
    {
        return false;
    }
    if (wcSource[*indexI] == L',') 
    {
        (*indexI)++;
    }
    while (wcSource[*indexI] != L',' && wcSource[*indexI] != L'/')
    //=! - отрицание какого-то числа, строка превращается в нулевую строку
    {
        wcDest[*indexK] = wcSource[*indexI];
        (*indexI)++;
        (*indexK)++;
    }
    wcDest[*indexK] = L'\0';
    *indexK = 0;
    return true;
}
int getUrl(CHAR* recvBuf) 
{
    int i = 0;
    int p = 0;
    while (recvBuf[i] != '/')
    {
        i++;
    }
    i++;
    CONST INT SIZE = 1024;
    CHAR command[SIZE];
    while (recvBuf[i] != '\0') 
    {
        command[p] = recvBuf[i];
        i++;
        p++;
    }
    command[p] = '\0';
    if(!strcmp(command, "registration"))
    {
        return 0;
    }
    if (!strcmp(command, "login")) 
    {
        return 1;
    }
    if (!strcmp(command, "checkingContactData")) 
    {
        return 2;
    }
    return -1;
}

bool setData(WCHAR* wcSource, WCHAR* wcDest, int sizeStr) 
{
    if (wcscmp(wcDest, L"")) 
    {
        wcscat_s(wcSource, sizeStr, wcDest);
        wcscat_s(wcSource, sizeStr, L" ");
        return true;
    }
    return false;
}

bool sendDataByAuthor(SOCKET* clientSocket, WCHAR* wcNumPhone, WCHAR* wcEmail, WCHAR* wcNickname, CHAR* status) 
{
    CONST INT SIZE = 1024;
    WCHAR buf[SIZE]{};
    CHAR answer[SIZE]{};
    if (!strcmp(status, "NOEXIST"))
    {
        wcscpy_s(buf, L"Учетная запись: ");
        setData(buf, wcNumPhone, SIZE);
        wcscat_s(buf, L"не сущеcтвует на сервере!");
        //wcscat_s(buf, L"\0");
    }
    if (!strcmp(status, "EXIST")) 
    {
        wcscpy_s(buf, L"Данная учетная запись: ");
        setData(buf, wcNumPhone, SIZE);
        setData(buf, wcEmail, SIZE);
        setData(buf, wcNickname, SIZE);
        wcscat_s(buf, L"существует!");
        //wcscat_s(buf, L"\0");
    }
    appendToLog(logHWND, buf);
    strcpy_s(answer, status);
    strcat_s(answer, "/");
    strcat_s(answer, "AUTHORIZATION");
    int iResult = send(*clientSocket, answer, strlen(answer), 0);
    if (iResult == INVALID_SOCKET) 
    {
        appendToLog(logHWND, L"Ошибка при отправке данных клиенту!");
        return false;
    }
    return true;
}

bool sendDataByReg(SOCKET* clientSocket, WCHAR *wcNumPhone, WCHAR* wcEmail, WCHAR *wcNickname, CHAR* status)
{
    CONST INT SIZE = 1024;
    WCHAR buf[SIZE]{};
    CHAR answer[SIZE];
    CHAR extraInfo[SIZE];
    if (!strcmp(status, "EXIST")) 
    {
        //wsprintf(buf, L"Данная учетная запись: %s %s %s сущетсвует!", wcNumPhone, wcEmail, wcNickname);
        //s - string (char arrya)
        //ws - wide sting (wide char array)
        wcscpy_s(buf, L"Данная учетная запись: ");
        wcscat_s(buf, L"\0");
        setData(buf, wcNumPhone, SIZE);
        setData(buf, wcEmail, SIZE);
        setData(buf, wcNickname, SIZE);
        wcscat_s(buf, L"существет!");
        wcscat_s(buf, L"\0");
    }
    if (!strcmp(status, "CREATED")) 
    {
        wcscpy_s(buf, L"Учетная запись успешно добавлена на сервер.");
    }
    appendToLog(logHWND, buf);
    strcpy_s(answer, status);
    strcat_s(answer, "/");
    strcat_s(answer, "REGISTRATION");
    if(send(*clientSocket, answer, strlen(answer)+1, 0) == INVALID_SOCKET)
    {
        appendToLog(logHWND, L"Ошибка при отправке регистрационных данных клиенту!");
        return false;
    }
    return true;
}

void checkContactData(SOCKET* lSocket, char* recvBuf) 
{
    CONST INT SIZE = 2000;
    WCHAR wcBuf[SIZE]{};
    WCHAR wcFirstName[SIZE]{};
    WCHAR wcLastName[SIZE]{};
    WCHAR wcNumPhone[SIZE]{};
    CHAR status[SIZE]{};
    int i = 0;
    int k = 0;
    MultiByteToWideChar(codePage, 0, recvBuf, strlen(recvBuf)+1, wcBuf, SIZE);
    getSubDataFromStr(&i, &k, wcBuf, wcFirstName);
    getSubDataFromStr(&i, &k, wcBuf, wcLastName);
    getSubDataFromStr(&i, &k, wcBuf, wcNumPhone);
    if (checkExistPhone(wcNumPhone)) 
    {
        strcpy_s(status, "EXIST");
        strcat_s(status, "/CONTACTS");
        int iResult = send(*lSocket, status, strlen(status) + 1, 0);
        if (iResult == INVALID_SOCKET) 
        {
            appendToLog(logHWND, L"Ошибка отправки данных клиенту для подтвержения существующего контакта");
        }
    }
    else 
    {
        strcpy_s(status, "NOEXIST");
        strcat_s(status, "/CONTACTS");
        int iResult = send(*lSocket, status, strlen(status) + 1, 0);
        if (iResult == INVALID_SOCKET) 
        {
            appendToLog(logHWND, L"Ошибка отправки данных клиенту для информирования не существующего контакта");
        }
    }
}

void checkAuthorizEntry(SOCKET* clientSocket, CHAR* recvBuf) 
{
    login = true;
    CONST INT SIZE = 1024;
    WCHAR wcBuf[SIZE];
    WCHAR wcNumPhone[SIZE]{};
    WCHAR wcEmail[SIZE]{};
    WCHAR wcNickname[SIZE]{};
    CHAR status[SIZE];
    MultiByteToWideChar(codePage, 0, recvBuf, strlen(recvBuf)+1, wcBuf, SIZE);
    int i = 0;
    int k = 0;
    getSubDataFromStr(&i, &k, wcBuf, wcNumPhone);
    if (getSubDataFromStr(&i, &k, wcBuf, wcEmail));
    if (checkExistPhone(wcNumPhone) || checkExistEmail(wcEmail))
    {
        strcpy_s(status, "EXIST");
        int len = strlen(status);
        status[len] = '\0';
        sendDataByAuthor(clientSocket, wcNumPhone, wcEmail, wcNickname, status);
    }
    else 
    {
        strcpy_s(status, "NOEXIST");
        int len = strlen(status);
        status[len] = '\0';
        sendDataByAuthor(clientSocket, wcNumPhone, wcEmail, wcNickname, status);
    }
    login = false;
}

bool checkRegEntry(SOCKET* clientSocket, CHAR* recvBuf) 
{
    login = true;
    CONST INT SIZE = 1024;
    WCHAR wcFirstName[SIZE]{};
    WCHAR wcLastName[SIZE]{};
    WCHAR wcNumPhone[SIZE]{};
    WCHAR wcEmail[SIZE]{};
    WCHAR wcDay[SIZE]{};
    WCHAR wcMonth[SIZE]{};
    WCHAR wcYear[SIZE]{};
    WCHAR wcBuf[SIZE]{};
    CHAR status[SIZE]{};
    MultiByteToWideChar(codePage, 0, recvBuf, strlen(recvBuf) + 1, wcBuf, SIZE);
    int i = 0;
    int k = 0;
    getSubDataFromStr(&i, &k, wcBuf, wcNumPhone);
    getSubDataFromStr(&i, &k, wcBuf, wcEmail);
    getSubDataFromStr(&i, &k, wcBuf, wcFirstName);
    getSubDataFromStr(&i, &k, wcBuf, wcLastName);
    getSubDataFromStr(&i, &k, wcBuf, wcDay);
    getSubDataFromStr(&i, &k, wcBuf, wcMonth);
    getSubDataFromStr(&i, &k, wcBuf, wcYear);
    if (checkExistPhone(wcNumPhone) || checkExistEmail(wcEmail))
    {
        strcpy_s(status, SIZE, "EXIST");
        int res = strlen(status);
        status[res] = '\0';
        sendDataByReg(clientSocket, wcNumPhone, wcEmail, wcFirstName, status);
    }
    else
    {
        if (insertEntry(wcNumPhone, wcFirstName, wcLastName,  wcEmail, wcDay, wcMonth, wcYear))
        {
            strcpy_s(status, SIZE, "CREATED");
            int res = strlen(status);
            status[res] = '\0';
            sendDataByReg(clientSocket, wcNumPhone, wcEmail, wcFirstName, status);
        }
    }
    login = false;
    return true;
}

int clientManagement(SOCKET* clientSocket) 
{
    CONST INT SIZE = 1024;
    CHAR recvBuf[SIZE];
    WCHAR wcBuf[SIZE];
    WCHAR wcTmp[SIZE];
    int len = 0;
    if (recvBuf == NULL)
    {
        appendToLog(logHWND, L"Не хватает памяти для пользователя.");
        closesocket(*clientSocket);
        //* - операция разыменования указателя
        //closescoket - закрывает существующий сокет и освобождает память.
        *clientSocket = INVALID_SOCKET;
        return 1;
    }
    //Запускаем прослушивание клиента
    while (*clientSocket != INVALID_SOCKET) 
    {
        int iResult = recv(*clientSocket, recvBuf, SIZE, NULL);
        recvBuf[iResult] = '\0';
        //iResult - конец строки
        if (iResult > 0)
        {
            appendToLog(logHWND, L"Сообщение успешно успешно получено\n");
            if (!login) 
            {
                int res = getUrl(recvBuf);
                ActionsAtServer action = static_cast<ActionsAtServer>(res);
                switch (action) 
                {
                case REGISTRATION:
                {
                    checkRegEntry(clientSocket, recvBuf);
                }
                break;
                case AUTHORIZATION:
                {
                    checkAuthorizEntry(clientSocket, recvBuf);
                }
                break;
                case CHECKING_CONTACT_DATA: 
                {
                    checkContactData(clientSocket, recvBuf);
                }
                break;
                default:
                    return 1;
                }
            }
            //!Провериить существование пользователя в базе сервера, по номеру телефона или почте, через две отдельные функции
            //Если запись сущетсвует то вернуть приложению-клиенту, что запись аккаунт существует
            //Во время авторизации (не регистрации), проверить через отдельную функцию сущетсвует пользватель, или нет.
        }
        else if (iResult == 0) 
        {
            appendToLog(logHWND, L"Соединенение с клиентом закрыто");
            closesocket(*clientSocket);
            *clientSocket = INVALID_SOCKET;
            return 1;
        }
        else 
        {
            appendToLog(logHWND, L"Ошибка соединения клиента");
            closesocket(*clientSocket);
            *clientSocket = INVALID_SOCKET;
            return 1;
        }
    }
    free(recvBuf);
    return 0;
}