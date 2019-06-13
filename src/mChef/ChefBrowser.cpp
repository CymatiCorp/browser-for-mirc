/* mChef - ChefBrowser.cpp
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
#include <commctrl.h>

/**
 * BrowserThreadCall. This is our threaded function used to create a browser.
 */
void WINAPI BrowserThreadCall(LPBrowserThread curBrowser)
{
    CF_DB_START

    CreateBrowser(curBrowser->m_hWnd, curBrowser->m_WindowName,
        curBrowser->m_CallBack, curBrowser->m_URL, curBrowser->m_Flags,
        curBrowser->m_hwResizeTarget);

    curBrowser->m_Flags = 0;
    curBrowser->m_hWnd = curBrowser->m_hwResizeTarget = 0;

    Free(curBrowser->m_CallBack);
    Free(curBrowser->m_WindowName);
    Free(curBrowser->m_URL);
    Free(curBrowser);

    CF_DB_END
}

/**
 * SmartCreateBrowser. Create a BrowserThread instance so as to not hang up mIRC
 *  due to user error.
 */
void SmartCreateBrowser(HWND hwParent, char *name, char *alias, char *url, long flags, HWND hwResizeTarget)
{
    CF_DB_START

    LPBrowserThread curBrowser = (LPBrowserThread)ChefMalloc(sizeof(BrowserThread));

    curBrowser->m_hWnd = hwParent;
    curBrowser->m_WindowName = ChefStrDup(name);
    curBrowser->m_CallBack = ChefStrDup(alias);
    curBrowser->m_URL = *url ? ChefStrDup(url) : NULL;
    curBrowser->m_Flags = flags;
    curBrowser->m_hwResizeTarget = hwResizeTarget;

    SmartCreateThread<LPBrowserThread>(&BrowserThreadCall, curBrowser);

    CF_DB_END
}

/**
 * CreateBrowser.  Parses the data from mIRC.
 *
 * @param hwParent            Parent window of browser 
 * @param name                Name of Browser
 * @param url                 Starting URL
 * @param hwResizeTarget      A target whose size the browser will copy when the parent receives WM_SIZE  
 *
 * @return Success/Fail message
 */
void CreateBrowser(HWND hwParent, char *name, char *alias, char *url, long flags, HWND hwResizeTarget)
{
    CF_DB_START

        lpChef curChef = ChefCreate(hwParent, name, alias , hwResizeTarget);
    curChef->flags = flags; // moved up top because I needed it

    if (IsListBox(curChef) || IsDialog(curChef)) // this will copy the resize targets left and top (for example in case its a listbox)
    {
        RECT rc;
        GetWindowRect(hwResizeTarget,&rc);
        ScreenToClient(hwParent,(LPPOINT)&rc.left);
        ScreenToClient(hwParent,(LPPOINT)&rc.right);

        curChef->mWindowInfo->m_x = rc.left;
        curChef->mWindowInfo->m_y = rc.top;
        curChef->mWindowInfo->m_nWidth = rc.right-rc.left;
        curChef->mWindowInfo->m_nHeight = rc.bottom-rc.top;
    }

    CF_DB(printf("Run Create Browser\n"));
#if 0
    if (HasAddressBar(curChef))
    {
        CF_DB(printf("Has Address Bar were running\n"));

        curChef->mWindowInfo->m_y += 25;
        curChef->mWindowInfo->m_nHeight -= 25;

        INITCOMMONCONTROLSEX ex;
        ex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        ex.dwICC = ICC_BAR_CLASSES;
        BOOL Inited = InitCommonControlsEx(&ex);
        CF_DB(printf("Initied Common Controls for AddressBar: %i\n",Inited));

        TBBUTTON *btns = new TBBUTTON[4];
        memset(btns,0,sizeof(TBBUTTON)*4);
        btns[0].iBitmap =  250;
        btns[0].idCommand = 0;
        btns[0].fsState = TBSTATE_ENABLED;
        btns[0].fsStyle = BTNS_SEP;
        btns[0].iString = -1;

        // Buffers for the buttons
        wchar_t wcBack[20], wcForward[20], wcGo[20];
        lstrcpy(wcBack,L"Back"); lstrcpy(wcForward,L"Forward"); lstrcpy(wcGo,L"Go");

        btns[1].iBitmap =  I_IMAGENONE;
        btns[1].idCommand = MC_TOOLBAR_BACK;
        btns[1].fsState = TBSTATE_ENABLED;
        btns[1].fsStyle = BTNS_BUTTON|BTNS_SHOWTEXT|BTNS_AUTOSIZE ;
        btns[1].iString = (INT_PTR)wcBack;

        btns[2].iBitmap =  I_IMAGENONE;
        btns[2].idCommand = MC_TOOLBAR_BACK;
        btns[2].fsState = TBSTATE_ENABLED;
        btns[2].fsStyle = BTNS_BUTTON|BTNS_SHOWTEXT|BTNS_AUTOSIZE ;
        btns[2].iString = (INT_PTR)wcForward;

        btns[3].iBitmap =  I_IMAGENONE;
        btns[3].idCommand = MC_TOOLBAR_BACK;
        btns[3].fsState = TBSTATE_ENABLED;
        btns[3].fsStyle = BTNS_BUTTON|BTNS_SHOWTEXT|BTNS_AUTOSIZE ;
        btns[3].iString = (INT_PTR)wcGo;

        if ((curChef->m_Toolbar = CreateToolbarEx(curChef->m_hWnd, WS_CHILD | WS_VISIBLE | TBSTYLE_LIST, NULL, 
            0, NULL, NULL, btns, 4, 0, 0, 100, 30, sizeof (TBBUTTON))) == NULL) {
                CF_DB(printf("Tool bar creation failed\n"));

        }

        CreateWindowEx(0L, L"Edit", NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_LEFT, 0,		// now we just catch when buttons are clicked and do what it is we do
            0, 250, 30, curChef->m_Toolbar, (HMENU)MC_TOOLBAR_EDIT, 0, 0 );
    }
#endif

    CEF_String cs(url != NULL ? url : "about:blank");
    cef_browser_create(curChef->mWindowInfo,0, CreateMyHandler(), 
        (const WCHAR*)cs.ToWideChar());

    if (IsDialog(curChef)) {
        long id = GetWindowLong(hwResizeTarget,GWL_ID);
        SetWindowLongPtr(hwResizeTarget,GWL_WNDPROC,(LONG)curChef->wpResizeTarget);
        DestroyWindow(hwResizeTarget);
        SetWindowLong(hwResizeTarget,GWL_ID,id);
        curChef->m_ResizeTarget = NULL;
        curChef->wpResizeTarget = NULL;
    }

    CF_DB_END
}
// EOF

