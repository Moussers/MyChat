// mychat.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "mychat.h"
#include "winsock2.h"       //Бибилиотека для работы с сетью
#define MAX_LOADSTRING 100

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

    // TODO: Разместите код здесь.

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

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
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

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   HWND rich = CreateWindow(L"RichEdit", )
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                SendPost(NULL);
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
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
