/* mChef - SubClass.cpp
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
#include <time.h>

/**
 * SubWndProc. Sub Class Window Procedure Class for mIRC
 *
 * @param hWnd
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return 0
 * @return CallWindowProc()
 */
LRESULT CALLBACK SubWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    lpChef curChef = ChefFind(hWnd);

    if (!curChef) {
        CF_DB(printf("ERROR: WndProc called on a Window (HWND: %08X) that we are not attached to!\n", hWnd));
        return 0; /* This should NOT happen!! */
    }
    switch (message)
    {
    case WM_COMMAND:
        {
		    switch(wParam)
		    {
		    case MC_TOOLBAR_BACK:
			    if (curChef->mBrowser->can_go_back(curChef->mBrowser))
				    curChef->mBrowser->go_back(curChef->mBrowser);
			    break;
		    case MC_TOOLBAR_FORWARD:
			    if (curChef->mBrowser->can_go_forward(curChef->mBrowser))
				    curChef->mBrowser->go_forward(curChef->mBrowser);
			    break;
		    case MC_TOOLBAR_REFRESH:
			    curChef->mBrowser->reload(curChef->mBrowser);
			    break;
		    case MC_TOOLBAR_GO:
			    {
				    cef_frame_t *frame = curChef->mBrowser->get_main_frame(curChef->mBrowser);
				    wchar_t  text[255];
				    GetWindowText(GetDlgItem(curChef->m_Toolbar,MC_TOOLBAR_EDIT),text,255);
				    frame->load_url(frame,text);
			    }
			    break;
		    }
        }
		break;
    case WM_SIZE:
        {
            if (curChef->m_ResizeTarget)        // if theres a resize target use it instead of this cruddy function
                break;

            SetWindowPos(curChef->m_BrowserHwnd, NULL, 0, 0, LOWORD(lParam),
                HIWORD(lParam), SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
        }
        break;
    case WM_PAINT:
    case WM_SETFOCUS:
    case WM_ERASEBKGND:
        break;
    }
    
    /* mIRC Can't handle our attached dialogs, so we send them to DefWindowProc instead. */
    return !(IsDialog(curChef)) ? CallWindowProc(curChef->OldProc,hWnd, message, wParam, lParam) :
        DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK ResizeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    lpChef curChef = ChefFindByResize(hWnd);

    if (!curChef) {
        CF_DB(printf("\nERROR: ResizeWndProc called on a Window (HWND: %08x) that we are not attached to!\n", hWnd));
        return 0; /* This should NOT happen!! */
    }
    switch (message)
    {
    case WM_WINDOWPOSCHANGED:
        LPWINDOWPOS pos = (LPWINDOWPOS)lParam;

        SetWindowPos(curChef->m_BrowserHwnd, NULL, pos->x, pos->y, pos->cx,
            pos->cy, SWP_NOACTIVATE | SWP_NOZORDER);

        break;
    }

    return CallWindowProc(curChef->wpResizeTarget,hWnd, message, wParam, lParam);
}

/* EOF */

