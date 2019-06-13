/* mChef - ChefSignals.cpp
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

ChefSignals::ChefSignals(HWND whWnd, LPWSTR wMapName)
    : m_hWnd(whWnd), m_MapName(wMapName)
{
    SetMooHandle(m_hWnd);
    OpenMap(m_MapName);
}

ChefSignals::~ChefSignals()
{
}

char *ChefSignals::GetEvaluatedData()
{
    return m_MsgBuffer;
}

char *ChefSignals::GetEvaluatedDataCopy()
{
    char *data = ChefStrDup(m_MsgBuffer);

    return data;
}

bool ChefSignals::CustomNotify(const lpChef chef, const char *signal, const char *data = NULL)
{
    return Cmd("/%s %s %s %s", chef->callbackAlias, chef->m_WindowName, signal, data ? data : "") == 1 ? true : false;
}

bool ChefSignals::CustomEvaluate(const lpChef chef, const char *signal, const char *data = NULL)
{
    if (Eval(m_MsgBuffer, MaxReturnBuff, "$%s(%s, %s, %s)", 
        chef->callbackAlias,  chef->m_WindowName, signal, data ? data : "") != 1)
        return false;

    if (m_MsgBuffer[0] != 0)
        return true;

    return false;
}

/**
* JSSignal. The name says it all.
*
* @param script            Script to send to MIRC
* @param bEvaluate        True to evaluate, False To Notify
*
* @return true     If we succeeded.
* @return false    If we failed.
*/
bool ChefSignals::JSSignal(char *script, bool bEvaluate)
{
    bool bRet = true;
    if (bEvaluate) {
        if (Eval(m_MsgBuffer, MaxReturnBuff, "%s", script) != 1 || m_MsgBuffer[0] == '\0')
            bRet = false;
    }
    else 
        bRet = Cmd("%s", script) == 1 ? true : false;

    return bRet;
}

/**
 * TitleBarChanged. Signal for when the title bar has changed.
 *
 * @param chef      Pointer to the Chef structure whose title has changed.
 * @param title     The new title.
 *
 * @return true     If CustomEvaluate succeeded.
 * @return false    If CustomEvaluate failed.
 */
bool ChefSignals::TitleBarChanged(const lpChef chef,const char *title)
{ 
    CF_DB(fprintf(stderr, "%s:%s\n", __FUNCTION__, title));
    return CustomEvaluate(chef, SIGNAL_TITLECHANGED, title);
}

/**
 * NavigateBegin. Signal for when navigation has begun
 *
 * @param chef        Pointer to the Chef structure whose title has changed.
 * @param navtype    The type of navigation about to begin
 * @param isRedirect If the navigation is a redirection
 * @param url        The URL being navigated
 *
 * @return true        If CustomEvaluate succeeded.
 * @return false        If CustomEvaluate failed.
 */
bool ChefSignals::NavigateBegin(const lpChef chef, const cef_handler_navtype_t navtype, const int isRedirect, const char *url)
{ 
    char cData[1024];

    ZeroMemory(cData, 1024);
    sprintf(cData, "%s, %i, %s", ChefNavTypeLookup(navtype),isRedirect, url);

    return CustomEvaluate(chef, SIGNAL_NAVBEGIN, cData);
}
/**
 * JSBind. Signal to notify JS is binding, if C_CANCEL is returned it will not bind the javascript object
 *
 * @param chef        Pointer to the Chef structure whose title has changed.
 * @param url        URL of page
 *
 * @return true        If CustomEvaluate succeeded.
 * @return false        If CustomEvaluate failed.
 */
bool ChefSignals::JSBind(const lpChef chef, const char *url) {
    return CustomEvaluate(chef, SIGNAL_JSBIND, url);
}

/**
 * LoadError.  Signal for when there was an error during a "load."
 *
 * @param chef          Chef structure.
 * @param error         CEF error code.
 * @param failedURL     URL that we failed on.
 *
 * @return true         If CustomEvaluate succeeded.
 * @return false        If CustomEvaluate failed.
 */
bool ChefSignals::LoadError(const lpChef chef,const cef_handler_errorcode_t error, const char *failedURL)
{ 
    char cErrorString[512];

    ZeroMemory(cErrorString, 512);
    sprintf(cErrorString,"%i, %s, %s",(int)error,ChefErrorLookup(error),failedURL);

    return CustomEvaluate(chef,SIGNAL_LOADERROR,cErrorString);
}

/**
 * LoadStart. Signal called when we start to a page.
 *
 * @param chef      Chef structure.
 *
 * @return true     If CustomEvaluate succeeded.
 * @return false    If CustomEvaluate failed.
 */
bool ChefSignals::LoadStart(const lpChef chef)
{
    return CustomNotify(chef, SIGNAL_STARTLOAD);
}


/**
 * LoadEnd. Signal called when we finished loading a page.
 *
 * @param chef      Chef structure.
 *
 * @return true     If CustomEvaluate succeeded.
 * @return false    If CustomEvaluate failed.
 */
bool ChefSignals::LoadEnd(const lpChef chef)
{
    return CustomNotify(chef, SIGNAL_ENDLOAD);
}

/**
 * AddressBarChanged. Signal called when the address bar (or the address in general) has changed.
 *
 * @param chef      Chef structure.
 * @param address   The new address.
 *
 * @return true     If CustomEvaluate succeeded.
 * @return false    If CustomEvaluate failed.
 */
bool ChefSignals::AddressBarChanged(const lpChef chef, const char *address)
{
    CF_DB(fprintf(stderr, "%s:%s\n", __FUNCTION__, address));

    return CustomNotify(chef, SIGNAL_ADDRESSCHANGED, address);
}

/**
 * PrinterSend. Signal called when ever we have finished printing a Chef window.
 *
 * @param curChef   Chef Structure.
 * @param sFile     Filename of the image.
 * @param sPath     Full path to the file.
 * @param bSuccess  Was it written successfully?
 *
 * @return true
 * @return false
 */
bool ChefSignals::PrinterSend(const lpChef curChef, const char *sFile, 
                              const char *sPath, bool bSuccess)
{
    char pBuf[1024];

    ZeroMemory(pBuf, 1024);

    sprintf_s(pBuf, 1024, "%s %s %s", sFile,  bSuccess ? "Done" : "Fail", sPath);

    return CustomNotify(curChef, SIGNAL_CHEFPRINTER, pBuf);
}

/* EOF */

