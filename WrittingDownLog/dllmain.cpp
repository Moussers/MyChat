// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>

__declspec(dllexport) void writtingDownLog(const WCHAR* record);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;

}
VOID writtingDownLog(const WCHAR* record)
{
    HANDLE logFile = CreateFile(L"log.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    //dwDesiredAccess - предоставл€ет атрибуты, иначе говор€ права доступа на чтение и запись файлу.
    //dwShareMode - запрещает повторно открывать файл.
    //lpSecurityAttributes - отвечает за настройки безопасности файла, если поставить NULL, тогда 
    //примен€ютс€ стандартные настройки безопасности к файлу при его создании.
    //dwCreationDisposition - флаг определ€ющий тип взаимодействи€ с файлом: создание или открытие;
    //CREATE_NEW - создание нового файла;
    //OPEN_EXISTS - открыть существующий файл.
    //dwFlagsAndAttributes - предоставл€ает права программе дл€ взаимодействи€ с файлом, обычно 
    //используетс€ флаг FILE_ATTRIBUTE_NORMAL, дающий стандартные права дл€ чтени€ и записи программе 
    //и ничего дополнительного.
    //hTemplateFile - работа с шифрованным файлом: чтение и запись, при создании временного файла.
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
        WriteFile(logFile, (LPCVOID)record, wcslen(record) * 2, NULL, NULL);
        WriteFile(logFile, L"\n", 2, NULL, NULL);
        CloseHandle(logFile);
    }
}
