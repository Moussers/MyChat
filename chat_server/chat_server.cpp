#include "framework.h"
#include "resource.h"

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
WSADATA wsData;
struct addrinfo* result, * ptr, hints;
//hints - для сети
HWND LogHWND;
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
	CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, INFO_DISPLAY_FIELD_POS_Y, OUTPUT_FIELD_POS_Y, INFO_DISPLAY_FIELD_WIDTH, OUTPUT_FIELD_HEIGHT, hMainWnd, (HMENU)IDM_MAIN_MENU_LOG_FIELD, hInstance, NULL);
	CreateWindow(L"Button", L"Удалить", WS_VISIBLE | WS_CHILD | WS_BORDER, MAIN_BUTTON_DELETE_POS_X, BUTTON_POS_Y, MAIN_BUTTON_DELETE_WIDTH, BUTTON_HEIGHT, hMainWnd, (HMENU)IDB_MAIN_BUTTON_DELETE, hInstance, NULL);
	CreateWindow(L"Button", L"Очистить лог", WS_VISIBLE | WS_CHILD | WS_BORDER, MAIN_BUTTON_CLEAR_LOG_POS_X, BUTTON_POS_Y, MAIN_BUTTON_CLEAR_LOG_WIDTH, BUTTON_HEIGHT, hMainWnd, (HMENU)IDB_MAIN_BUTTON_CLEAR_LOG, hInstance, NULL);
	ShowWindow(hMainWnd, SW_SHOWDEFAULT);
	MSG msg;
	while (IsWindow(hMainWnd)) 
	{
		if (GetMessage(&msg, hMainWnd, 0, 0));
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
VOID startServer(HWND log);
VOID checkTable(int );
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_COMMAND:
	{
		switch (LOWORD(message)) 
		{
		case IDB_MAIN_BUTTON_DELETE:
			break;
		case IDB_MAIN_BUTTON_CLEAR_LOG:
			break;
		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
VOID startServer(HWND log) 
{

}