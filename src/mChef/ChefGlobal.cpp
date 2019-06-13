/* mChef - ChefGlobal.cpp
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

/* Global Scope Definitions */

/**
 * g_lpCachePath.   Our cache path.
 */
wchar_t *g_lpCachePath = NULL;

/**
 * g_mIRC_hWnd. Handle to the mIRC Window.
 */
HWND g_mIRC_hWnd = NULL;

/**
 * g_bCefInit. Boolean indicating whether CEF has been initalized or not.
 */
bool g_bCefInit = false;

/**
 * g_bClearCacheOnExit. Boolean indicating if we should clear our cache on exit.
 */
bool g_bClearCacheOnExit = false;


/**
 * g_bCacheInMemory. Boolean indicating if we are using memory cache.
 */
bool g_bCacheInMemory = false;

/**
 * g_Signal. Global instance of our signals class.
 */
ChefSignals *g_Signal = NULL;

/**
 * g_curFlags. Table used for parsing "SetFlags."
 */
FlagsTable g_curFlags[] = { 
    { "CanFocus",       FLAG_CANFOCUS },
    { "CanFocusWidget", FLAG_CANFOCUSWIDGET },
    { 0 }
};


/**
 * g_iChefCount. Integer variable used to keep an accurate count of how many
 *  Chef instances we have attached.
 */
unsigned int g_iChefCount = 0;


/* EOF */

