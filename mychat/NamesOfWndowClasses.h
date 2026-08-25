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
//CONST CHAR* - двухмерный массив.
CONST WCHAR* numberOfDays[MAX_LOADSTRING] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10", L"11", 
L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19", L"20", L"21", L"22", L"23", L"24", L"25", L"26", L"27",
L"28", L"29", L"30", L"31"};
CONST WCHAR* numberOfMonths[MAX_LOADSTRING] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10", L"11", 
L"12"};
CONST WCHAR* numberOfDates[MAX_LOADSTRING] = { L"1950", L"1951", L"1952", L"1953", L"1954", L"1955", L"1956", 
L"1957", L"1958", L"1959", L"1960", L"1961", L"1962", L"1963", L"1964", L"1965", L"1966", L"1967", L"1968", L"1969", 
L"1970", L"1971", L"1972", L"1973", L"1974", L"1975", L"1976", L"1977", L"1978", L"1979", L"1980", L"1981", L"1982",
L"1983", L"1984", L"1985", L"1986", L"1987", L"1988", L"1989", L"1990", L"1991", L"1992", L"1993", L"1994", L"1995",
L"1996", L"1997", L"1998", L"1999", L"2000", L"2001", L"2002", L"2003", L"2004", L"2005", L"2006", L"2007", L"2008",
L"2009", L"2010", L"2011", L"2012", L"2013", L"2014", L"2015", L"2016", L"2017", L"2018", L"2019", L"2020", L"2021", 
L"2022", L"2023", L"2024", L"2025", L"2026", L"\0"};