/* mChef - ChefStrings.h
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
#ifndef __CEF_STRING_WRAPPER_H__
#   define __CEF_STRING_WRAPPER_H__

/**
 * CEF_String. Wrapper Class for Cef String.
 *  This should help when converting.
 */
class CEF_String
{
private:
    cef_string_t _cstData;                        // The current string.
    
public:
    CEF_String(void);                            // initializes for empty string
    CEF_String(const wchar_t *);                // initializes based on cef_string (use this for WCHAR string, just cast it)
    CEF_String(const char *);                    // initializes based on char *
    
    // Special Constructors
    CEF_String(long lData, int radix = 10);        // initialize from a long
    CEF_String(int iData, int radix = 10);        // initialize from a int
    CEF_String(CEF_String &);                    // Copy Constructor

    ~CEF_String(void);                            // pointer data

    char *ToMultiByte(void);                    // Converts to multi-byte, this is returns a new char *pointer that must be deleted
    cef_string_t ToCefString(void);                // Returns a copy of the internal ceef_string_t modifications will not effect class
    const wchar_t *ToWideChar(void);            // Returns wchar *
    
    int GetLength(void);                        // Returns length of internal string
    
    bool Reallocate(wchar_t *);                    // Reallocate data with a new wchar string
    bool Reallocate(char *);                    // Reallocate data with a new char *
};

#endif

/* EOF */

