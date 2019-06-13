/* mChef - ChefMisc.cpp
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
#include <direct.h>
#include "h.h"

/**
 * wBuildPath. Builds a path appending "wDir" to the end..
 *
 * @param wPtr              Pointer that we are going to allocate memory to.
 * @param wDir              Directory that should be "appended to," this cannot be NULL.
 *
 * @return wPtr             New Pointer.
 */
wchar_t *wBuildPath(wchar_t *wPtr, const wchar_t *wDir)
{
    CF_DB_START
    wPtr = new wchar_t[MAX_PATH];
    wchar_t *buffer;

    buffer = _wgetcwd(NULL, 0);

    swprintf_s(wPtr, MAX_PATH, L"%s\\%s", buffer, wDir);

    MyFree(buffer);

    CF_DB(fwprintf(stderr, L"wBuildPath: %s\n", wPtr));

    CF_DB_END
    return wPtr;
}

/**
 * BuildPath. Builds a path appending "Dir" to the end..
 *
 * @param Ptr              Pointer that we are going to allocate memory to.
 * @param Dir              Directory that should be "appended to," this cannot be NULL.
 *
 * @return Ptr             New Pointer.
 */
char *BuildPath(char *Ptr, const char *Dir)
{
    CF_DB_START
    Ptr = new char[MAX_PATH];
    char *buffer;

    buffer = _getcwd(NULL, 0);

    sprintf_s(Ptr, MAX_PATH, "%s\\%s", buffer, Dir);

    MyFree(buffer);

    CF_DB(fprintf(stderr, "BuildPath: %s\n", Ptr));

    CF_DB_END
    return Ptr;
}

/**
 * IsAllNum. Checks to see if a string is all numbers or not.
 *
 * @param String
 *
 * @return false
 * @return true
 */
bool IsAllNum(char *String)
{
    for (int i = 0;(i < (int)strlen(String)) && (String[i] != '\0'); i++)
        if (!isdigit(String[i]))
            return false;
    
    return true;
}


/**
 * ChefSetFlags. Sets the flags accordingly.
 */
bool ChefSetFlags(lpChef curChef, const char *lpFlag, bool set)
{
    for (int i = 0; g_curFlags[i].FlagName; i++) {
        if (_stricmp(g_curFlags[i].FlagName, lpFlag) == 0) {
            if (set)
                curChef->flags |= g_curFlags[i].flag;
            else
                curChef->flags &= ~g_curFlags[i].flag;

            return true;
        }
    }

    return false;
}

#ifdef CHEF_DEBUG
/**
 * ChefGetLastError. Retrieves the last error and prints it to the console window.
 */
void ChefGetLastError()
{
    DWORD dwError = GetLastError();
    LPSTR lpErrorMsg = Gen::GetLastErrorMessage(dwError);

    printf("Error Code: %d, Error Message: %s\n", dwError, lpErrorMsg);
    LocalFree(lpErrorMsg);
}
#endif

/* EOF */

