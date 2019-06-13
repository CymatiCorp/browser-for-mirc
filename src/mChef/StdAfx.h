/* mChef - StdAfx.h
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
#pragma once


#ifndef WINVER                         
#   define WINVER       0x0501    // Windows XP or later.
#endif

#ifndef _WIN32_WINNT            
#   define _WIN32_WINNT 0x0501   // Windows XP or later.  
#endif

#ifndef _WIN32_WINDOWS
#   define _WIN32_WINDOWS 0x0410 // Windows ME or later.
#endif

#ifdef __INTEL_COMPILER
#   pragma warning( disable : 181  )
#   pragma warning( disable : 593  )
#   pragma warning( disable : 869  )
#   pragma warning( disable : 981  )
#   pragma warning( disable : 1418 )
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>
#include <vector>

#define USING_CEF_SHARED      1
#include "cef_capi.h"

#define MIRC_DLL
#include "mlib/mlib.hpp"

/* EOF */
