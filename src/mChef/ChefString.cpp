/* mChef - ChefString.cpp
 * Copyright (C) 2009 Genscripts
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "StdAfx.h"
#include "h.h"

/**
 * CEF_String. Constructor #1
 */
CEF_String::CEF_String(void)
    : _cstData(cef_string_alloc(L""))
{ 
}

/**
 * CEF_String. Constructor #2
 *
 * @param str   Wide Character String
 */
CEF_String::CEF_String(const wchar_t *str)
    : _cstData(cef_string_alloc(str))
{
}

/**
 * CEF_String.  Constructor #3
 *
 * @param str   Multibyte String
 */
CEF_String::CEF_String(const char *str) 
{
    // First Convert String to WCHAR
    // Then Allocate a new CefString based on That
    wchar_t *wch = new wchar_t[strlen(str)+1];
    memset(wch,0,sizeof(wchar_t) * (strlen(str)+1));
    _CHEFSSERT(MultiByteToWideChar(CP_UTF8, 0, str, (strlen(str) + 1), wch, (strlen(str)+1)));

    // now we convert the WCHAR to a cef_string_t
    _CHEFSSERT(this->_cstData = cef_string_alloc(wch));
    delete [] wch;

}

/**
* CEF_String.  Constructor #4
*
* @param lData Long To Convert
* @param radix base of long
*/
CEF_String::CEF_String(long lData, int radix) // initialize from a long
{
    wchar_t *wch = new wchar_t[65];
    _ltow_s(lData,wch,65,radix);
    _CHEFSSERT(this->_cstData = cef_string_alloc(wch));
    delete [] wch;
}

/**
* CEF_String.  Constructor #5
*
* @param iData int To Convert
* @param radix base of int
*/
CEF_String::CEF_String(int iData, int radix) // initialize from a long
{
    wchar_t *wch = new wchar_t[65];
    _ltow_s(iData,wch,65,radix);
    _CHEFSSERT(this->_cstData = cef_string_alloc(wch));
    delete [] wch;
}

/**
 * CEF_String.  Constructor #6
 *
 * @param old   Old CEF_String
 */
CEF_String::CEF_String(CEF_String &old)
    : _cstData(cef_string_alloc(old.ToWideChar()))
{
}

/**
 * ~CEF_String. Deconstructor.
 */
CEF_String::~CEF_String(void)
{
    cef_string_free(this->_cstData);
}

/**
 * ToMultiByte. Converts the current string to multibyte.
 *
 * @return multibyte string
 */
char *CEF_String::ToMultiByte(void) 
{  
    int curLen = cef_string_length(this->_cstData);
    int newLen = 0;
    char *data = NULL;

    if ((newLen = WideCharToMultiByte(CP_UTF8,0,
        (WCHAR*)this->_cstData,curLen,0,0,NULL,NULL))>0) {
            data = new char[newLen+1];
        /* This function will work faster if the last 2 parameters are NULL. 
         * We will let the system decide what character to use if it can't
         * decode one. */
       if (WideCharToMultiByte(CP_UTF8,0,(WCHAR*)this->_cstData,
           curLen+1,data,newLen+1, NULL,NULL) == 0) {
               CF_DB(ChefGetLastError();)
       }
    } else {
        CF_DB(printf("Unable to get the size needed for new string!\n");
              ChefGetLastError());
    }
        

    return data;
}

/**
 * ToWideChar. Converts the current string to wide.
 *
 * @return wide string
 */
const wchar_t *CEF_String::ToWideChar(void)
{
    return (const WCHAR*)this->_cstData;
}

/**
 * ToCefString. Converts the current string to a cef_string_t
 *
 * @return cef_string_t
 */
cef_string_t CEF_String::ToCefString(void)
{
    return cef_string_alloc(this->ToWideChar());
}

/**
 * GetLength. Calculates the length of the current string.
 *
 * @return length
 */
int CEF_String::GetLength() { return cef_string_length(this->_cstData); }

/**
 * Reallocate. Reallocates space for a multi byte string.
 *
 * @param str       The string that we are allocating space for.
 *
 * @return true     If we were able to
 * @return false    If we failed
 */
bool CEF_String::Reallocate(char *str)
{
    wchar_t *wch = new wchar_t[strlen(str)+1];

    memset(wch,0,sizeof(wchar_t) * (strlen(str)+1));
    _CHEFSSERT(MultiByteToWideChar(CP_UTF8,0,str,strlen(str)+1,wch,strlen(str)+1));

    // now we convert the WCHAR * to a new cef_string_t
    if (cef_string_realloc(&this->_cstData,wch) == 1)
        return true;

    return false;
}

/**
 * Reallocate. Reallocates space for a wide string.
 *
 * @param wstr      The string that we are allocating space for.
 *
 * @return true     If we were able to
 * @return false    If we failed
 */
bool CEF_String::Reallocate(wchar_t *wstr)
{
    if (cef_string_realloc(&this->_cstData,wstr) == 1)
        return true;

    return false;
}

/* EOF */

