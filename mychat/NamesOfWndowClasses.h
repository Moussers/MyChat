#pragma once
#include <Windows.h>
#include "PositionsButtonsAndWindows.h";
//windows.h является Windows-конкретный файл заголовков для языка C программирования,
//который содержит заявления для всех функций в Windows API,

//CONST WCHAR USER_LIST_CLASS_NAME[] = L"UserListWindow";
CONST WCHAR szUserListClassName[MAX_LOADSTRING] = L"UserListWindow";
CONST WCHAR szUserAccountClassName[MAX_LOADSTRING] = L"AddingUserAccount";
CONST WCHAR szInfoModificationClass[MAX_LOADSTRING] = L"ModifingUserInfo";
CONST WCHAR szInfoAboutProgram[MAX_LOADSTRING] = L"AuthorProgram";
CONST WCHAR szAutorization[MAX_LOADSTRING] = L"Autorization_window_class";
CONST WCHAR szWndAthorizationClass[MAX_LOADSTRING] = L"Extra_authorization_window_class";
CONST WCHAR szAthorizationTitle[MAX_LOADSTRING] = L"Авторизация";
CONST WCHAR szAdditionalInfoClass[MAX_LOADSTRING] = L"Additional_extra_info_class";
CONST WCHAR szNicknameClass[MAX_LOADSTRING] = L"Nickname_window_class";