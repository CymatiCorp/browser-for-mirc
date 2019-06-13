/* mChef - h.h
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

#ifndef __H_H__
#   define __H_H__

#include "ChefStruct.h"
#include "ChefMacros.h"
#include "ChefStrings.h"
#include "ChefSignals.h"
#include "ChefExtern.h"

/**
 * SmartCreateThread. Template function used to create a threaded ptrFunc call.
 *
 *  @param ptrFunc      Pointer to the function that's being threaded.
 *  @param ptrParam     Parameters that the function needs/wants.
 *  @param dwFlags      Thread Creation flags, if any.
 */
template <class T>
inline void SmartCreateThread(void *ptrFunc, T ptrParam, DWORD dwFlags  = 0)
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ptrFunc, ptrParam, dwFlags, NULL);
}

#endif

/* EOF */

