#include "UserInfo.h"

void UserInfo::setNumberPhone(CHAR* userPhone)
{
    strcpy_s(m_numberPhone, arraySize, userPhone);
}

CHAR* UserInfo::numberPhone()
{
    return m_numberPhone;
}

void UserInfo::setEmail(CHAR* email)
{
    strcpy_s(m_email, arraySize, email);
}

CHAR* UserInfo::email()
{
    return m_email;
}

void UserInfo::setFirstName(CHAR* userFirstName)
{
    strcpy_s(m_firstName, arraySize, userFirstName);
}

CHAR* UserInfo::firstName()
{
    return m_firstName;
}

void UserInfo::setLastName(CHAR* userLastName)
{
    strcpy_s(m_lastName, arraySize, userLastName);
}

CHAR* UserInfo::lastName()
{
    return m_lastName;
}

void UserInfo::setBirthdayDay(CHAR* userBirthdayDay)
{
    strcpy_s(m_birthdayDay, arraySize, userBirthdayDay);
}

CHAR* UserInfo::birthdayDay()
{
    return m_birthdayDay;
}

void UserInfo::setBirthdayMonth(CHAR* userBirthdayMonth)
{
    strcpy_s(m_birthdayMonth, arraySize, userBirthdayMonth);
}

CHAR* UserInfo::birthdayMonth()
{
    return m_birthdayMonth;
}

void UserInfo::setBirthdayYear(CHAR* userBirthdayYear)
{
    strcpy_s(m_birthdayYear, arraySize, userBirthdayYear);
}

CHAR* UserInfo::birthdayYear()
{
    return m_birthdayYear;
}