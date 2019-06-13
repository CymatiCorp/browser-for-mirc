/* mChef - ChefMemory.cpp
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
 * ChefPanic. Unloads our DLL in Panic.
 */
void ChefPanic()
{
    UnloadDll(1);
}

/**
 * ChefMalloc. Allocates space and returns the pointer to it.  Exits out if there is insufficient memory.
 *
 * @param size      Size to be allocated.
 *
 * @return pointer  To the allocated memory
 */
void *ChefMalloc(size_t size)
{
    CF_DB_START
    void *ptr = malloc(size);

    if (ptr == NULL) {
        ChefPanic();
        exit(EXIT_FAILURE);
    }

    CF_DB_END
    return ptr;
}

/**
 * ChefCalloc. Allocates an array in memory with all the elements already initialized to 0.  
 *
 * @param  numOfElements     Number of Elements
 * @param  Size              Size of each element
 *
 * @return pointer           To the allocated memory
 */
void *ChefCalloc(size_t numOfElements, size_t Size)
{
    CF_DB_START
    void *ptr = calloc(numOfElements, Size);

    if (ptr == NULL) {
        ChefPanic();
        exit(EXIT_FAILURE);
    }

    CF_DB_END
    return ptr;
}

/**
 * ChefStrdup. Duplicates a string.
 *
 * @param _string   The string to be duplicated.
 *
 * @return ptr      Of the allocated memory.
 */
char *ChefStrDup(const char *_string)
{
    CF_DB_START
    char *ptr = _strdup(_string);

    if (ptr == NULL) {
        ChefPanic();
        exit(EXIT_FAILURE);
    }

    CF_DB_END
    return ptr;
}

/* EOF */
