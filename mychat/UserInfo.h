#pragma once
#ifndef _USERINFO_CLASS__
#define _USERINFO_CLASS__
#include "framework.h"
CONST INT arraySize = 2000;

class UserInfo
{
private:
    CHAR m_numberPhone[arraySize];
    CHAR m_email[arraySize];
    CHAR m_firstName[arraySize];
    CHAR m_lastName[arraySize];
    CHAR m_birthdayDay[arraySize];
    CHAR m_birthdayMonth[arraySize];
    CHAR m_birthdayYear[arraySize];
public:
    UserInfo() : m_numberPhone(""), m_email(""), m_firstName(""), m_lastName(""), m_birthdayDay(""), m_birthdayMonth(""), m_birthdayYear("") {};
public:
    void setNumberPhone(CHAR* numberPhone);
    CHAR* numberPhone();
    void setEmail(CHAR* email);
    CHAR* email();
    void setFirstName(CHAR* userFirstName);
    CHAR* firstName();
    void setLastName(CHAR* userLastName);
    CHAR* lastName();
    void setBirthdayDay(CHAR* userBirthdayDay);
    CHAR* birthdayDay();
    void setBirthdayMonth(CHAR* userBirthdayMonth);
    CHAR* birthdayMonth();
    void setBirthdayYear(CHAR* userBirthdayYear);
    CHAR* birthdayYear();
};
#endif