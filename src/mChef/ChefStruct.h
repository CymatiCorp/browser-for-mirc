/* mChef - ChefStruct.h
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
#ifndef __STRUCT_H__
#   define __STRUCT_H__

/**
 * Base Type Enumerator.
 */
enum BaseType {
    BaseCef = 0,
    BaseCefBrowser,
    BaseCefHandler,
    BaseV8Value,
    BaseV8Handler,
    BaseUnknown
};

/**
 * Image Type Enumerator.
 */
enum ImageType {
    IMAGE_BMP = 0,   /* Bitmap Image */
    IMAGE_PNG        /* PNG Image */
};

/**
 * Chef Structure.
 */
typedef struct chef_t 
{
    cef_browser_t *mBrowser;            /* Browser Window... */
    cef_window_info_t *mWindowInfo;     /* Info about the window */

    WNDPROC OldProc;                    /* Old Window Procedure */
    WNDPROC wpResizeTarget;             /* Old WinProc for the resize target */

    char *m_WindowName;                 /* The name of our window */
    char *callbackAlias;                /* Holds the alias to call back */
    char *mark;                         /* Mark the mChef Window */

    HWND m_hWnd;                        /* Handle to the window */
    HWND m_BrowserHwnd;                 /* Handle to our browser */
    HWND m_ResizeTarget;                /* Handle to a resize target */
	HWND m_Toolbar;						/* Handle to the toolbar */

    long flags;                         /* What type of Window is this? */

    bool bIsLoading;                    /* Are we currently loading? */
    bool bCanGoBack;                    /* Can we go back */
    bool bCanGoForward;                 /* Can we go forward? */
} Chef, *lpChef;

/**
 * BrowserThread.   This structure is used when threading the creation of a browser.
 *  Why is this necessary? In case some users call $dll($mChef.dll,Attach) instead of DLL Call,
 *  this way we don't hang up (or stall) mIRC.
 */
typedef struct tagBrowserThread
{
    HWND m_hWnd;            //Parent Window of the Chef
    HWND m_hwResizeTarget;  //Target whose size the browser will copy when the parent receives WM_SIZE 

    char *m_WindowName;     //User given name for the Chef
    char *m_CallBack;       //mIRC Alias Callback for the Chef signals
    char *m_URL;            //Initial browsing specified URL Given by user.

    long m_Flags;           //Flags passed by Initial call.
    
} BrowserThread, *LPBrowserThread;

/**
 * PrinterThread. Used for passing arguments to the threaded printer.
 */
typedef struct tagPrinterThread
{
    lpChef      curChef;         //Chef instance that we are going to "print."
    ImageType   m_Type;          //What type of image are we going to print?
    bool        bFullView;       //Are we printing the entire thing, or just our current view of it?

} PrinterThread, *LPPrinterThread;

/**
 * Internal Base Structure
 */
typedef struct tagBases
{
    volatile LONG iReferenceCount;      /* Amount of times that it is being referenced */
    cef_base_t *base;                   /* The base pointer */
} Bases, *lpBases;

/**
 * FlagsTable. Used for parsing SetFlags.
 */
typedef struct tagFlagsTable
{
    const char *FlagName;
    long flag;
} FlagsTable;

#endif

/* EOF */

