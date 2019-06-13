/* mChef - Main.cpp
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
#include <shellapi.h>
#include <shlwapi.h>
#include "h.h"
#include "Chef.h"

using namespace Gen;

#ifdef CHEF_DEBUG
    CGenConsole *m_Console = NULL;
#endif

/**
 * Parse.  Parses the data from mIRC.
 *
 * @param mIRCData      Pointer to the data mIRC sent us
 * @param buffer        Pointer to the buffer that we should populate
 * @param params        Pointer to the number of parameters
 */
void Parse(char *mIRCData, char *buffer[CHEF_MAXPARAMS], int *params)
{
    // Lets account for completely blank parameters
    if (strlen(mIRCData) == 0) {
        *params = 0;
        return;
    }
    memset(buffer, 0, CHEF_MAXPARAMS * sizeof(char *));
    std::string s;
    int p = 0;
    for (int i = 0; i <= (int)strlen(mIRCData) && p < CHEF_MAXPARAMS; i++)
    {
        char c = mIRCData[i];

        if (i != (int)strlen(mIRCData) && c == '\\' && mIRCData[i+1] == '\\')
        {
            s.push_back(c);
            i++;
        }
        else if (i != (int)strlen(mIRCData) && c == '\\' && mIRCData[i+1] == '|')
        {
            s.push_back('|');
            i++;
        }
        else if (c == '|' || c == '\0')
        {
            buffer[p] = new char[s.length()+1];
            strcpy(buffer[p],s.c_str());
            s.clear();
            p++;
        }
        else
            s.push_back(c);
    }
    *params = p;
}

/**
 * LoadDLL. Initializes our DLL and the Chef.
 *
 * @param pLoadInfo     mIRC LOADINFO Structure.
 */
void __stdcall LoadDll(Gen::LoadInfo *pLoadInfo)
{
    CF_DB(m_Console = new CGenConsole(true); m_Console->OpenConsole(););

    OleInitialize(NULL);
    CF_DB(freopen("stderr.txt", "wt", stderr));
    CF_DB(fprintf(stderr, "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"));
    CF_DB(fprintf(stderr, "Begin mChef Debug: %s\n\n", __TIMESTAMP__));
    
    g_mIRC_hWnd = pLoadInfo->mHwnd;
}


/**
 * UnloadDLL. Checks to see if mIRC is trying to unload us after 10mins. If so, tell it to keep us loaded.
 *
 * @param mTimeout   mIRC Fills this in for us.
 */
int __stdcall UnloadDll(int mTimeout)
{

    if (mTimeout == 1)
        return 0;

    CF_DB(printf("Popping Chef vectors...\n"));
    ChefPopThemAll(); // Good reason to do this before cef_shutdown (has to release the reference of the handler or we get fail)

    CF_DB(printf("Shutting down Cef...\n"));
    cef_shutdown();       

    if (g_bClearCacheOnExit && !g_bCacheInMemory && g_lpCachePath) {
        /* Create double null terminated string that's needed for SHFileOperation. */
        WCHAR wCachePath[MAX_PATH] = {0};
        wnsprintf(wCachePath, MAX_PATH, L"%s", g_lpCachePath);
        SHFILEOPSTRUCT fileOp =
        {
            NULL,
            FO_DELETE,
            wCachePath,
            L"",
            FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
            false,
            0,
            L""
        };
        SHFileOperation(&fileOp);
    }

    CF_DB(printf("Popping all Base vectors\n"));
    BasePopThemAll();            // Good reason to do this after cef_shutdown, cef will release a reference to our handler and if we do this before we will make that pointer invalid
                                // In a correcth world this hsould have nothing left to delete, since all items should be released to 0 to skip the debug assert's in cef_shutdown

    CF_DB(printf("Closing Handles and Signal System...\n"));
    delete g_Signal;

    OleUninitialize();
    MyFreeEx(g_lpCachePath);
    CF_DB(printf("Terminating mChef.dll...OK\nClosing down stderr...\n"));

    CF_DB(
        fclose(stderr);
        if (m_Console)
            delete m_Console;
    );


    return 1;
}

/**
 * Attach.
 *
 *  parv[0]     parv[1]     parv[2]     parv[3]     parv[4]
 *  Handle      Name        CallBack    [URL]       [ListBox?]
 *  Handle      Name        DlgCtrl     CallBack    [URL]
 *
 */
int __stdcall Attach(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START;
    SanityCheck();
    ParseParameters(data);

    /* Minimum number of parameters required. */
    if (parc < 3) {
        sprintf(data, "C_ERR Attach: Invalid Parameters.");
        return 3;
    }

    /* Check Window Name*/
    if (!*parv[1] || isdigit(*parv[1])) {
        sprintf(data,"C_ERR Attach: Invalid Window Name.  A Window name MUST start with a letter.");
        return 3;
    }

    HWND m_hWnd = (HWND)atol(parv[0]);

    CheckWindowByHandle(m_hWnd);
    CheckWindowByName(parv[1]);

    LONG m_WindowType = FLAG_PICWIN; 
    HWND m_hwFound = NULL;

    if ((m_hwFound = FindWindowEx(m_hWnd, NULL, L"Static", NULL))) {
        if ((parc == 5) && (!(m_hwFound = FindWindowEx(m_hWnd, NULL, L"ListBox", NULL)))) {
            sprintf(data, "C_ERR Attach: Could not find ListBox in specified window handle");
            CF_DB_END;
            return 3;
        }
        else
            m_WindowType = FLAG_LISTBOX;
    }
    else if ((parc == 5) && (m_hwFound = FindWindowEx(m_hWnd, NULL, L"ListBox", NULL))) {
        m_WindowType = FLAG_LISTBOX;
    }
    else if ((IsAllNum(parv[2]))) {
        if (!(m_hwFound = GetDlgItem(m_hWnd, 6000 + atoi(parv[2])))) {
            sprintf(data, "C_ERR Attach: Unable to find Dialog Item");
            CF_DB_END;
            return 3;
        }
        m_WindowType = FLAG_DIALOG;
    }
    else {
        sprintf(data, "C_ERR Attach: Could not find Static/Invalid Parameters.");
        CF_DB_END;
        return 3;
    }
    
    // m_WindowType |= FLAG_ADDRESSBAR;

    if (m_hwFound)
        ShowWindow(m_hwFound, SW_HIDE);

    int iSanity = !(m_WindowType & FLAG_DIALOG) ? 0 : 1;
    char *m_URL = parv[3+iSanity] ? parv[3+iSanity] : NULL;
    char *m_CallBack = parv[2+iSanity];

    SmartCreateBrowser(m_hWnd, parv[1], m_CallBack, m_URL, m_WindowType, m_hwFound);

    sprintf(data, "C_OK Attach:");
    CF_DB_END;
    return 3;
}

/**
 * Detach. Detaches CEF out of a mIRC Specified Window.
 *      mIRC Parameters: param[0] = Window Handle/Name of Window
 */
int __stdcall Detach(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);

    if (parc > 1) {
        sprintf(data,"C_ERR Detach Invalid Parameters. Usage: $dll(mChef.dll,Detach,<HWND/WindowName>)");
        CF_DB_END;
        return 3;
    }
    lpChef curChef = NULL;
    if (!(curChef = ChefFindEx(parv[0])) || !curChef->mBrowser) {
        sprintf(data, "C_ERR Detach NoSuchBrowser :The HWND or Window Name you specified does not exist.");
        CF_DB_END;
        return 3;
    }
    if (curChef->m_ResizeTarget) 
        ShowWindow(curChef->m_ResizeTarget,SW_SHOWNORMAL);

    // Tell CEF that we are done with this window.
    SendMessage(curChef->m_BrowserHwnd, WM_DESTROY, 0, 0);
    ChefPop(curChef);

    sprintf(data, "C_OK Detach %s", parv[0]);

    CF_DB_END
    return 3;
}

/**
 * Navigate. Navigates a Chef to an address.
 */
int __stdcall Navigate(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);

    if (parc < 2) {
        sprintf(data, "C_ERR navigate: Invalid Parameters. Usage: $dllcall(mChef.dll,Navigate,Handle or Name|http://www.whereto.com/)");
        CF_DB_END;
        return 3;
    }

    lpChef curChef = NULL;
    if (!(curChef = ChefFindEx(parv[0])) || !curChef->mBrowser) {
        sprintf(data, "C_ERR NoBrowser");
        CF_DB_END;
        return 3;
    }

    cef_frame_t *CefFrame = curChef->mBrowser->get_main_frame(curChef->mBrowser);
    if (!CefFrame) {
        sprintf(data, "C_ERR NoFrame");
        CF_DB_END;
        return 3;
    }

    CefFrame->load_url(CefFrame, CEF_String(parv[1]).ToWideChar());
    BaseRelease(CefFrame);
    sprintf(data, "C_OK %s", parv[0]);

    CF_DB_END
    return 3;
}

/**
 * IsAttached. Returns if Chef attached to HWND/Name already.
 *
 * Comment: Mostly useful if defining custom /echo alias in mIRC (added in r119-JD).
 * Params: HWND|Name
 * Returns: "C_OK HWND|Name" if attached, or "C_ERR HWND|Name"
 */
int __stdcall IsAttached(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);

    if (parc != 1) {
        sprintf(data, "C_ERR isattached: Invalid Parameters. Usage: $dll(mChef.dll,IsAttached,Handle or Name)");
        CF_DB_END;
        return 3;
    }

    lpChef curChef = NULL;
    if (!(curChef = ChefFindEx(parv[0])) || !curChef->mBrowser) 
        sprintf(data, "C_ERR %s", parv[0]); //No browser @ NAME|HWND
    else
        sprintf(data, "C_OK %s", parv[0]); //Browser found @ NAME|HWND

    CF_DB_END
    return 3;
}

/**
* TitleBar. Retrieves or Sets the titlebar for a window
*/
int __stdcall TitleBar(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);

    if (parc < 1 || parc > 2) {
        sprintf(data, "C_ERR titlebar: Invalid Parameters. Usage: $dllcall(mChef.dll,TitleBar,Handle or Name|[optional text to set]");
        CF_DB_END;
        return 3;
    }

    /* Check to see if the user is specifying window by Name or by Handle. */
    HWND hwWindow = NULL;

    if (IsAllNum(parv[0])) {
        hwWindow = (HWND)atol(parv[0]);
    } else {
        lpChef curChef = ChefFind(parv[0]);
        hwWindow = curChef ? curChef->m_hWnd : NULL;
    }
    CheckWindowByHandle(hwWindow);

    if (parc == 1) {
        GetWindowTextA(hwWindow,data,512);
    } else {
        SetWindowTextA(hwWindow,parv[1]);
        sprintf(data,"C_OK TitleBarChanged %s %s",parv[0],parv[1]);
    }

    CF_DB_END
    return 3;
}

/**
* ExecScript. Executes javascript on the specified Chef to window
* Params: HWND(orName)|Script
*/
int __stdcall ExecScript(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);
    
    if (parc < 2) {
        sprintf(data, "C_ERR ExecScript: Invalid Parameters. Usage: $dllcall(mChef.dll,ExecScript,Handle or Name|script)");
        CF_DB_END;
        return 3;
    }

    lpChef curChef = NULL;
    if (!(curChef = ChefFindEx(parv[0])) || !curChef->mBrowser) {
        sprintf(data, "C_ERR NoBrowser");
        CF_DB_END;
        return 3;
    }

    cef_frame_t *CefFrame = curChef->mBrowser->get_focused_frame(curChef->mBrowser);
    if (!CefFrame) {
        sprintf(data, "C_ERR NoFrame");
        CF_DB_END;
        return 3;
    }

    CefFrame->execute_java_script(CefFrame,CEF_String(parv[1]).ToWideChar(),NULL,0);
    BaseRelease(CefFrame);

    sprintf(data,"C_OK Script Completed Successfully");

    CF_DB_END;
    return 3;
}

/**
 * mChef_Init. Allows mIRC users to initialize mChef and specify a cache directory.
 */
int __stdcall mChef_Init(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    ParseParameters(data);

    if (g_bCefInit) {
        sprintf(data, "C_ERR AlreadyInitialized");
        CF_DB_END;
        return 3;
    }

    if ((*data) && (!g_lpCachePath)) {
        g_lpCachePath = (wchar_t *)new wchar_t[MAX_PATH];
        swprintf(g_lpCachePath, MAX_PATH, L"%s", CEF_String(parv[0]).ToWideChar());
        g_lpCachePath[wcslen(g_lpCachePath)+1]='\0';
        ChefInitialize(false);
    } else if (parc == 0) {
        ChefInitialize(true);
    }
    sprintf(data, "%s", g_bCefInit ? "C_OK" : "C_ERR");

    CF_DB_END
    return 3;
}

/**
 * Version
 */
int __stdcall Version(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START

    sprintf(data, "C_OK mChef.dll - An mIRC Chromium Embedded Framework Implementation [Version %s]", MCHEF_VERSION_STR);

    CF_DB_END
    return 3;
}

/**
 * SetFlags
 */
int __stdcall SetFlags(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);

    if (parc < 2) {
        sprintf(data, "C_ERR SetFlags: Invalid Parameters. Usage: $dllcall(mChef.dll,SetFlags,Handle or Name|+/-FLAG)");
        CF_DB_END;
        return 3;
    }

    lpChef curChef = NULL;
    if (!(curChef = ChefFindEx(parv[0]))) {
        sprintf(data, "C_ERR NoBrowser");
        CF_DB_END;
        return 3;
    }

    for (int i = 1; i < parc; i++) {
        /* Unknown flag. Continue. */
        if ((*parv[i] != '-') && (*parv[i] != '+'))
            continue;

        bool bSet = (*parv[i] == '+') ? true : false;
        *parv[i]++;

        /* Sanity Check */
        if (!*parv[i])
            continue;

        ChefSetFlags(curChef, parv[i], bSet);

    }
    sprintf(data,"C_OK SetFlags %s", parv[0]);

    CF_DB_END
    return 3;
}

/**
 * Mark
 */
int __stdcall Mark(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);

    if (!*data) {
        sprintf(data, "C_ERR Mark: Invalid Parameters. Usage: $dllcall(mChef.dll,Mark,Handle or Name)");
        CF_DB_END;
        return 3;
    }
    
    lpChef curChef = NULL;
    if (!(curChef = ChefFindEx(parv[0])) || !curChef->mBrowser) {
        sprintf(data, "C_ERR NoBrowser");
    } else if (parc == 1) {
        sprintf(data, "C_OK Mark %s %s", parv[0], curChef->mark ? curChef->mark : "");
    } else {
        Free(curChef->mark);
        if (!*parv[1]) {
            sprintf(data, "C_OK Mark %s Clear", parv[0]);
        } else {
            curChef->mark = ChefStrDup(parv[1]);
            sprintf(data, "C_OK Mark %s %s", parv[0], curChef->mark);
        }
    }

    CF_DB_END
    return 3;
}

/**
* PrintWindowView
*/
int __stdcall PrintWindowView(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);
    
    if (!*data) {
        sprintf(data, "C_ERR PrintWindowView: Invalid Parameters. Usage: $dllcall(mChef.dll,PrintWindowView,Handle or Name)");
        CF_DB_END;
        return 3;
    } 

    lpChef curChef = NULL;
    if (!(curChef = ChefFindEx(parv[0])) || !curChef->mBrowser) {
        sprintf(data, "C_ERR NoBrowser");
    } else {
        ChefPrint(curChef, IMAGE_PNG, false);
        sprintf(data, "C_OK PrintWindowView %s", parv[0]);
    }

    CF_DB_END
    return 3;
}

/**
 * ChefBrowser.
 *
 *  parv[0]     parv[1]
 *  N/Name      hWnd
 *              bWnd
 *              Type
 *              URL
 *              Loading
 *              Forward
 *              Back
 */
int __stdcall ChefBrowser(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START
    SanityCheck();
    ParseParameters(data);

    if (parc < 2) {
        if (!IsAllNum(parv[0])) {
            sprintf(data, "0");
            CF_DB_END
            return 3;
        }

        int iRequest = atoi(parv[0]);
        lpChef m_curChef = NULL;

        if ((iRequest <= 0) || !(m_curChef = ChefFindCount(iRequest))) {
            sprintf(data, "%i", g_iChefCount);
            CF_DB_END
            return 3;
        }

        sprintf(data, "%s", m_curChef->m_WindowName);
        CF_DB_END
        return 3;
    }
    static lpChef curChef = NULL;

    if (!parv[1] || curChef == NULL && !(curChef = ChefFindEx(parv[0])) ||
        (_stricmp(curChef->m_WindowName, parv[0])!=0 && 
        !(curChef = ChefFindEx(parv[0])))) {
            sprintf(data, "CouldNotFindWindow");
    }
    else if (_stricmp(parv[1], "hWnd") == 0) {
        sprintf(data, "%d", curChef->m_hWnd);
    }
    else if (_stricmp(parv[1], "bWnd") == 0) {
        sprintf(data, "%d", curChef->m_BrowserHwnd);
    }
    else if (_stricmp(parv[1], "Type") == 0) {
        if (IsPicWin(curChef))
            sprintf(data, "1");
        else if (IsDialog(curChef))
            sprintf(data, "2");
        else
            sprintf(data, "3");
    }
    else if (_stricmp(parv[1], "URL") == 0) {
        cef_frame_t *curFrame = curChef->mBrowser->get_main_frame(curChef->mBrowser);
        if (!curFrame) {
            sprintf(data, "NoURL");
        }
        else {
            CEF_String cs(curFrame->get_url(curFrame));
            sprintf(data, "%s", cs.ToMultiByte());
        }
    }
    else if (_stricmp(parv[1], "Loading") == 0) {
        sprintf(data, "%i", curChef->bIsLoading ? 1 : 0);
    }
    else if (_stricmp(parv[1], "Forward") == 0) {
        sprintf(data, "%i", curChef->bCanGoForward ? 1 : 0);
    }
    else if (_stricmp(parv[1], "Back") == 0) {
        sprintf(data, "%i", curChef->bCanGoBack ? 1 : 0);
    }
    else {
        sprintf(data, "UnknownProperty: %s", parv[1]);
    }

    CF_DB_END
    return 3;
}

/**
 * ClearCache
 */
int __stdcall ClearCache(HWND mWnd, HWND aWnd, char *data, char *parms, BOOL show, BOOL nopause)
{
    CF_DB_START;
    SanityCheck();

    g_bClearCacheOnExit = true;

    sprintf(data, "C_OK");

    CF_DB_END;
    return 3;
}

/* EOF */

