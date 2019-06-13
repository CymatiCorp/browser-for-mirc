/* mChef - Chef.cpp
 * Copyright (C) 2009  Genscripts
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

using namespace std;

/**
 * ChefWindows. Vector.
 *  When we attach to a window, we will create an lpChef pointer 
 *  and store them within this vector.
 */
static vector<lpChef> ChefWindows;

/**
 * ChefInsert.  Inserts a Chef into our vector.
 *
 * @param curChef   Chef Pointer
 */
inline void ChefInsert(lpChef curChef)
{
    CF_DB_START
    ChefWindows.push_back(curChef);
    CF_DB_END
}

/**
 * ChefFindBrowser. Finds a Chef by browser handle.
 *
 * @param m_browser_hWnd    Browser's handle
 *
 * @return lpChef           Chef Pointer
 * @return NULL             If nothing was found
 */
lpChef ChefFindBrowser(const HWND m_BrowserHwnd)
{
    CF_DB_START
    if (!m_BrowserHwnd)
        return NULL;

    try {
        for (vector<lpChef>::iterator it = ChefWindows.begin(); it != ChefWindows.end(); it++) {
            if ((*it)->m_BrowserHwnd == m_BrowserHwnd) {
                CF_DB_END
                return (*it);
            }
        }
    }
    catch (std::out_of_range) {
        CF_DB(printf ("VECTOR OUT OF RANGE!!!!!!!!!!!!!!!!!!\n"));
        CF_DB(__debugbreak());
    }

    CF_DB_END
    return NULL;
}

/**
 * ChefFind.  Finds a Chef by Window Handle.
 *
 * @param m_hWnd    Window Handle.
 *
 * @return lpChef   Chef Pointer
 * @return NULL     Nothing was found
 */
lpChef ChefFind(const HWND m_hWnd)
{
    if (!m_hWnd)
        return NULL;

    for (vector<lpChef>::iterator it = ChefWindows.begin(); it != ChefWindows.end(); it++) {
        if ((*it)->m_hWnd == m_hWnd) {
            return (*it);
        }
    }

    return NULL;
}

/**
 * ChefFind.  Finds a Chef by Window Name.
 *
 * @param m_WindowName       Window Name
 *
 * @return lpChef            Chef Pointer
 * @return NULL              Nothing was found
 */
lpChef ChefFind(const char *m_WindowName)
{
    if (!*m_WindowName) {
        return NULL;
    }

    for (vector<lpChef>::iterator it = ChefWindows.begin(); it != ChefWindows.end(); it++)  {
        if (_stricmp((*it)->m_WindowName, m_WindowName) == 0) {
            return (*it);
        }
    }
    return NULL;
}

/**
 * ChefFindCount.  Finds a Chef by count.
 *
 * @param iCount             Count
 *
 * @return lpChef            Chef Pointer
 * @return NULL              Nothing was found
 */
lpChef ChefFindCount(const int iCount)
{
    int c = 0;

    for (vector<lpChef>::iterator it = ChefWindows.begin(); it != ChefWindows.end(); it++) {
        if (c++ == (iCount-1))
            return (*it);
    }

    return NULL;
}

/**
* ChefFindByResize.  Finds a Chef by Resize Target
*
* @param m_hWnd    Resize Target Window Handle.
*
* @return lpChef   Chef Pointer
* @return NULL     Nothing was found or m_hWnd was NULL
*/
lpChef ChefFindByResize(const HWND m_hWnd)
{
    if (m_hWnd) {
        for (vector<lpChef>::iterator it = ChefWindows.begin(); it != ChefWindows.end(); it++) {
            if ((*it)->m_ResizeTarget == m_hWnd) {
                return (*it);
            }
        }
    }

    return NULL;
}

/**
 * ChefFindEx. Finds a Chef by either Window Name or Handle.
 *
 * @param ptr       Pointer to the string that contains the handle or window name.
 *
 * @return lpChef
 */
lpChef ChefFindEx(char *ptr)
{
    CF_DB_START
    lpChef curChef = NULL;

    if (IsAllNum(ptr)) 
        curChef = ChefFind((HWND)atol(ptr));
    else 
        curChef = ChefFind(ptr);
    
    CF_DB_END
    return curChef;
}

/**
 * ChefDelete.  Deletes a Chef.
 *
 * @param curChef   Pointer of Chef to be deleted.
 */
void ChefDelete(lpChef curChef)
{
    CF_DB_START

    /* For bama:
     * if (HasAddressBar(curChef))
     *   KillTimer(curChef->m_hWnd, 1);
     */
    if (curChef->OldProc) {
        CF_DB(printf("[ChefDelete:%08X] Restoring old window procedure.\n", curChef->m_hWnd));
        SetWindowLongPtr(curChef->m_hWnd, GWL_WNDPROC, (LONG_PTR)curChef->OldProc);
     }

    if (curChef->wpResizeTarget) {
        CF_DB(printf("[ChefDelete:%08X] Restoring old resize target.\n", curChef->m_hWnd));
        SetWindowLongPtr(curChef->m_ResizeTarget, GWL_WNDPROC, (LONG_PTR)curChef->wpResizeTarget);
    }

    BaseRelease(curChef->mBrowser);

    curChef->mBrowser = NULL;
    curChef->OldProc = curChef->wpResizeTarget = NULL;

    Free(curChef->mWindowInfo);
    Free(curChef->callbackAlias);
    Free(curChef->mark);
    Free(curChef->m_WindowName);
    Free(curChef);

    g_iChefCount--;
    CF_DB(printf("g_iChefCount: %i\n", g_iChefCount));

    CF_DB_END
}

/**
 * ChefPop.  Deletes a single Chef from our vector.
 *
 * @param curChef   Pointer to the chef to be deleted.
 */
void ChefPop(lpChef curChef)
{
    CF_DB_START
    for (vector<lpChef>::iterator it = ChefWindows.begin(); it != ChefWindows.end(); it++ )
    {
        if ((*it)->m_hWnd == curChef->m_hWnd) {
            ChefWindows.erase(it);
            ChefDelete(curChef);
            CF_DB_END
            break;
        }
    }
    CF_DB_END
}

/**
* ChefPopThemAll.  Deletes All Chefs
*
* @param curChef   Pointer to the chef to be deleted.
*/
void ChefPopThemAll()
{
    CF_DB_START
    lpChef curChef = NULL;

    while (!ChefWindows.empty()) {
        curChef = ChefWindows.front();
        ChefWindows.erase(ChefWindows.begin());
        // Just in case the end-user never closed all windows, we tell CEF to close them.
        SendMessage(curChef->m_BrowserHwnd, WM_DESTROY, 0, 0);
        ChefDelete(curChef);
    }
    CF_DB_END
    ChefWindows.clear();
}

/**
 * ChefCreate.  Creates a Chef and inserts it into our vector.
 *
 * @param m_hWnd            Window Handle
 * @param m_WindowName        Window Name
 * @param cCallbackAlias    Alias to call back to from handler
 * @param hwResizeTarget    HWND to resize the browser too, this allows us to either default to the entire parent window, or use a target
 * @return lpChef            Pointer to the new Chef
 */
lpChef ChefCreate(const HWND m_hWnd, const char *m_WindowName, char *cCallbackAlias, HWND hwResizeTarget)
{
    CF_DB_START

    lpChef curChef = (lpChef)ChefMalloc(sizeof(Chef));
    memset(curChef,0,sizeof(Chef));

    curChef->mBrowser = NULL;
    curChef->flags = NULL;
    curChef->mark = NULL;

    curChef->m_WindowName = ChefStrDup(m_WindowName);
    curChef->m_hWnd = m_hWnd;
    curChef->m_BrowserHwnd = NULL;
    curChef->m_ResizeTarget = hwResizeTarget;

    RECT m_Rect;
    GetClientRect(curChef->m_hWnd, &m_Rect);
    
    if (hwResizeTarget) {
        GetClientRect(curChef->m_ResizeTarget, &m_Rect);    
        curChef->wpResizeTarget = (WNDPROC)SetWindowLongPtr(curChef->m_ResizeTarget, GWL_WNDPROC, (LONG_PTR)ResizeWndProc);
    }

    curChef->OldProc = (WNDPROC)SetWindowLongPtr(curChef->m_hWnd, GWL_WNDPROC, (LONG_PTR)SubWndProc);
    curChef->callbackAlias = ChefStrDup(cCallbackAlias);

    // Initialize the mWindowInfo Structure with the basic params
    curChef->mWindowInfo = (cef_window_info_t *)ChefMalloc(sizeof(cef_window_info_t));
    memset(curChef->mWindowInfo,0,sizeof(cef_window_info_t));
    curChef->mWindowInfo->m_dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
    curChef->mWindowInfo->m_hWndParent = m_hWnd;
    curChef->mWindowInfo->m_windowName = CEF_String(m_WindowName).ToCefString();
    curChef->mWindowInfo->m_x =
    curChef->mWindowInfo->m_y = 0;
    curChef->mWindowInfo->m_nWidth = m_Rect.right;
    curChef->mWindowInfo->m_nHeight = m_Rect.bottom;
    curChef->bCanGoBack = curChef->bCanGoForward = curChef->bIsLoading = false;

    ChefInsert(curChef);
    g_iChefCount++;

    CF_DB_END
    return curChef;
}
/**
* ChefErrorLookup.    Reverse looksup an error code, should probably be put under the ChefMisc.cpp
*
* @param error                Error Code
* 
* @return char*                Error Code String Representation
*/
char *ChefErrorLookup(cef_handler_errorcode_t error)
{
    switch (error)
    {
    case ERR_ABORTED:
        return "Operation Aborted";
    case ERR_ACCESS_DENIED:
        return "Access Denied";
    case ERR_ADDRESS_INVALID:
        return "Address Invalid";
    case ERR_ADDRESS_UNREACHABLE:
        return "Address Unreachable";
    case ERR_CACHE_MISS:
        return "Cache Miss";
    case ERR_CERT_AUTHORITY_INVALID:
        return "Certificate Authority is Invalid";
    case ERR_CERT_COMMON_NAME_INVALID:
        return "Certificate Common Name is Invalid";
    case ERR_CERT_CONTAINS_ERRORS:
        return "Certificate Contains Errors";
    case ERR_CERT_DATE_INVALID:
        return "Certificate Date is Invalid or Expired";
    case ERR_CERT_END:
        return "Certificate End";
    case ERR_CERT_INVALID:
        return "Certificate is Invalid";
    case ERR_CERT_NO_REVOCATION_MECHANISM:
        return "Certificate has no Revocation Mechanism";
    case ERR_CERT_REVOKED:
        return "Certificate is Revoked";
    case ERR_CERT_UNABLE_TO_CHECK_REVOCATION:
        return "Unable to check Certificate Revocation Information";
    case ERR_CONNECTION_ABORTED:
        return "Connection Aborted";
    case ERR_CONNECTION_CLOSED:
        return "Connection Closed";
    case ERR_CONNECTION_FAILED:
        return "Connection Failed";
    case ERR_CONNECTION_REFUSED:
        return "Connection Refused";
    case ERR_CONNECTION_RESET:
        return "Connection Reset";
    case ERR_DISALLOWED_URL_SCHEME:
        return "Disallowed URL Scheme";
    case ERR_EMPTY_RESPONSE:
        return "Empty Response from Server";
    case ERR_FAILED:
        return "Generic Failure Occurred";
    case ERR_FILE_NOT_FOUND:
        return "File Not Found";
    case ERR_FILE_TOO_BIG:
        return "File was too big";
    case ERR_INSECURE_RESPONSE:
        return "Insecure Response from Server";
    case ERR_INTERNET_DISCONNECTED:
        return "Internet was Disconnected";
    case ERR_INVALID_ARGUMENT:
        return "Invalid Argument";
    case ERR_INVALID_CHUNKED_ENCODING:
        return "Invalid Chunked Encoding";
    case ERR_INVALID_HANDLE:
        return "Invalid Handle";
    case ERR_INVALID_RESPONSE:
        return "Invalid Response From Server";
    case ERR_INVALID_URL:
        return "Invalid URL";
    case ERR_METHOD_NOT_SUPPORTED:
        return "Method Not Supported";
    case ERR_NAME_NOT_RESOLVED:
        return "DNS Name Not Resolved";
    case ERR_NO_SSL_VERSIONS_ENABLED:
        return "No SSL Enabled";
    case ERR_NOT_IMPLEMENTED:
        return "Not Implemented";
    case ERR_RESPONSE_HEADERS_TOO_BIG:
        return "Response Header is Too Big";
    case ERR_SSL_CLIENT_AUTH_CERT_NEEDED:
        return "SSL Client Auth Certificate is Needed";
    case ERR_SSL_PROTOCOL_ERROR:
        return "SSL Protocol Error";
    case ERR_SSL_RENEGOTIATION_REQUESTED:
        return "SSL Renegotiation Error";
    case ERR_SSL_VERSION_OR_CIPHER_MISMATCH:
        return "SSL Version or Cipher Mismatch";
    case ERR_TIMED_OUT:
        return "Operation Timed Out";
    case ERR_TOO_MANY_REDIRECTS:
        return "Too Many Redirects Occurred";
    case ERR_TUNNEL_CONNECTION_FAILED:
        return "Tunnel Connection Failed";
    case ERR_UNEXPECTED:
        return "An Unexpected Error Occurred";
    case ERR_UNEXPECTED_PROXY_AUTH:
        return "An Unexpected Error Occurred With the Proxy Authenication Request";
    case ERR_UNKNOWN_URL_SCHEME:
        return "There is an Unknown URL Scheme";
    case ERR_UNSAFE_PORT:
        return "An Unsafe Port was Used";
    case ERR_UNSAFE_REDIRECT:
        return "An Unsafe Redirect was Attempted";
    default:
        return "Unknown Error Occured";
    }
}


/**
* ChefNavTypeLookup. Reverse looksup a nav type, should probably be moved to ChefMisc.cpp
*
* @param nav                Nav Code
* 
* @return char*                Nav Code String Representation
*/
char *ChefNavTypeLookup(cef_handler_navtype_t nav)
{
    switch (nav)
    {
    case NAVTYPE_BACKFORWARD:
        return "BackForward";
    case NAVTYPE_FORMRESUBMITTED:
        return "FormResubmit";
    case NAVTYPE_FORMSUBMITTED:
        return "FormSubmit";
    case NAVTYPE_LINKCLICKED:
        return "LinkClicked";
    case NAVTYPE_OTHER:
        return "Other";
    case NAVTYPE_RELOAD:
        return "Reload";
    default:
        return "Unknown";
    }
}

/**
 * ChefInitialize. Initializes mChef.
 *
 * @param memCache      Set to true if you want to cache in memory.
 */
bool ChefInitialize(bool memCache)
{
    CF_DB_START
    
    CF_DB(fprintf(stderr, "Initializing Chef.\n"));
    
    if (!g_lpCachePath && !memCache)
        g_lpCachePath = wBuildPath(g_lpCachePath, L"Cache");

    if (cef_initialize(true, memCache ? NULL : g_lpCachePath)) {
        CF_DB(fwprintf(stderr, L"CefInitialize: Ok\n\nCache Directory: %s\n", memCache ? L"Memory" : g_lpCachePath));
        g_Signal = new ChefSignals(g_mIRC_hWnd, L"mIRC3");
        g_bCefInit = true;
    }
    else {
        CF_DB(fprintf(stderr, "CefInitialization failed.\n"));
    }

    g_bCacheInMemory = memCache;

    CF_DB_END
    return g_bCefInit;
}
/* EOF */

