// mychat.cpp : Определяет точку входа для приложения.

#include "framework.h"
#include "mychat.h"
#include "winsock2.h"               //Бибилиотека для работы с сетью
#define MAX_LOADSTRING 100
#define MAIN_WINDOW_POSITION_X 820
#define MAIN_WINDOW_POSITION_Y 580
#define WIDTH_INPUT_FIELD 800
#define WIDTH_INFO_FIELD 790
#define HEIGT_INFO_FIELD 440
#define INFO_FIELD_POS_X 5
#define INFO_FIELD_POS_Y 10

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
int checkTables() 
{
    sqlite3* db;
    int res = sqlite3_open("data.db", &db);
    if (res)
    {
        MessageBox(NULL, L"База данных не подключена!", L"Ошибка!", MB_OK | MB_ICONERROR);
        sqlite3_close(db);
        //sqlite3_close - прерывает связь с базой данных
        return 1;
    }
    const char* sqlEx = "SELECT COUNT(*) FROM sql_master WHERE type = 'table' AND name = 'group';";
    //sqlite_master - хранит количество таблиц
    sqlite3_stmt* nextRow;
    if (sqlite3_prepare_v2(db, sqlEx, -1, &nextRow, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(nextRow);
        if (curRow == SQLITE_ROW)
        {
            INT countRows = sqlite3_column_int(nextRow, 0);
            //sqlite3_column_int - выодит текущий индекс колонки
            if (countRows == 0)
            {
                MessageBox(NULL, L"Ни одной группы не найдено!\nСоздаём новую...", L"Информация", MB_OK | MB_ICONERROR);
                const char* createTable = "CREATE TABLE groups (group_id PRIMARY KEY NOT NULL, group_name TEXT NOT NULL);";
                INT status = sqlite3_exec(db, createTable, NULL, NULL, NULL);
                if (status == SQLITE_OK) 
                {
                    MessageBox(NULL, L"Таблица группа создана", L"Информаация", MB_OK | MB_ICONERROR);
                }
            }
        }
        sqlite3_finalize(nextRow);
        //sqlite3_finalize - очищает память от переменной.
    }
    const char* test_table = "SELECT COUNT(*) sqlite_master WHERE type = 'table' and name = 'users';";
    if (sqlite3_prepare_v2(db, test_table, 1, &nextRow, NULL) == SQLITE_OK)
    {
        INT curRow = sqlite3_step(nextRow);
        if (curRow == SQLITE_ROW) 
        {
            INT countRows = sqlite3_column_int(nextRow, 0);
            if (countRows == 0) 
            {

                const char* createTable = 
                    "CREATE TABLE USERS (user_id INT PRIMARY KEY NOT NULL,"
                    "first_name TEXT NOT NULL,"
                    "last_name TEXT NOT NULL,"
                    "middle_name TEXT,"
                    "phone_number INT NOT NULL,"
                    "mail TEXT NOT NULL,"
                    "path_icon TEXT,"
                    "icon BLOB,"
                    "status INT NOT NULL);";
                INT status = sqlite3_exec(db, createTable, NULL, NULL, NULL);
                if (status == SQLITE_OK) 
                {
                    MessageBox(NULL, L"Таблица пользователь создана", L"Информация", MB_OK | MB_ICONINFORMATION);
                }
            }
        }
        sqlite3_finalize(nextRow);
    }
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
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYCHAT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
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
    int result = sqlite3_open("DatabaseMessanger.db", &db);     //sqlite_pen - открывает если файл найден или если файл не найден, тогда создааёт его
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
    userWnd.lpszClassName = L"WindowUsers";
    ATOM r = RegisterClassEx(&userWnd);
    HWND winUser = CreateWindow(L"WindowUsers", L"Пользователи", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 360, 400, hWnd, 0, GetModuleHandle(NULL), NULL);
    //WS_OVERLAPPEDWINDOW - добавляет в окну занички закрыть, расширить, свернуть делая окно самостоятельным.
    ShowWindow(winUser, SW_SHOWDEFAULT);
    MSG msg;
    while (IsWindow(winUser)) 
    {
        if (GetMessage(&msg, winUser, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    HWND listUser = CreateWindow(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE, 0, 0, 270, 320, winUser, NULL, 0, GetModuleHandle(NULL), NULL);
    const char* findTableUs = "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' and name = 'users'";
    //sqlite3_stmt* sql_st;
    ////sqlite3_stmt - структура где хранится информация об каждой из таблиц
    //if (sqlite3_prepare_v2(db, findTableUs, -1, &sql_st, NULL) == SQLITE_OK)
    ////sqlite3_prepare_v2 - создает структур откуда мы будем брать наши результаты
    //{
    //    int rc = sqlite3_step(sql_st);
    //    //sqlite3_step - двигаемся по записям из таблицы вынимая, каждую запись
    //    if (rc == SQLITE_ROW)
    //    //SQLITE_ROW - идентификатор строки
    //}
    sqlite3_close(db);
    return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, MAIN_WINDOW_POSITION_X, MAIN_WINDOW_POSITION_Y, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   HWND storyField = CreateWindow(L"STATIC", L"", WS_VISIBLE| WS_CHILD| WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_WANTRETURN | ES_AUTOVSCROLL, INFO_FIELD_POS_X, INFO_FIELD_POS_Y, WIDTH_INFO_FIELD, HEIGT_INFO_FIELD, hWnd, 0, hInstance, NULL);
   HWND text = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 2, 460, WIDTH_INPUT_FIELD, 20, hWnd, 0, hInstance, NULL);
   HWND button = CreateWindow(L"Button", L"Отправить", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 490, 100, 20, hWnd, 0, hInstance, NULL);
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
            case IDM_USERS:
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
