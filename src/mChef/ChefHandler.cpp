/* mChef - ChefHandler.cpp
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
 * handle_before_created. Called when a new window is created.
 *
 * @param handler       Handler
 * @param parentBrowser Parent browser window
 * @param windowInfo    Self explanatory
 * @param popup         If the new window is a popup, this will be "true"
 * @param newHandler    The handler; usually the same as our parent window.
 *
 * @return RV_CONTINUE
 */
CEF_CALL handle_before_created(struct _cef_handler_t* handler, cef_browser_t* parentBrowser,
                                cef_window_info_t* windowInfo, int popup,
                                struct _cef_handler_t** newHandler, cef_string_t* url)
{
    CF_DB_START

    BaseRelease(parentBrowser);

    CF_DB_END
    return RV_CONTINUE;
}

                                
/**
 * handle_after_created.  Called after the window has been created.
 * The return value is ignored by CEF.
 *
 * @param handler   Handler
 * @param browser   Pointer to the browser
 *
 * @return RV_CONTINUE
 */                               
CEF_CALL handle_after_created(struct _cef_handler_t* handler, cef_browser_t* browse)
{
    CF_DB_START

    HWND m_BrowserHwnd = browse->get_window_handle(browse);
    HWND m_hWnd = NULL; /* Safe Practice */

    if (m_BrowserHwnd) {
        if ((m_hWnd = GetParent(m_BrowserHwnd))) {
            lpChef curChef = ChefFind(m_hWnd);
            if (curChef) {
                CF_DB(printf("Browser: %d, Parent: %d\n", m_BrowserHwnd, m_hWnd));
                curChef->m_BrowserHwnd = m_BrowserHwnd;
                curChef->mBrowser = browse;
                BaseAddRef(curChef->mBrowser);
                g_Signal->CustomNotify(curChef, SIGNAL_CHEFATTACHED, NULL);
            } else {
                CF_DB(printf("Could not find Chef for %d\n", m_hWnd));
            }
        } else {
            CF_DB(printf("Could not find Parent for %d, bastard child?\n", m_BrowserHwnd));
        }
    } else {
        CF_DB(printf("%s:%i:This should not happen!!\n", __FUNCTION__, __LINE__));
    }
    
    BaseRelease(browse);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_address_change. Called when the address bar is changed.
 * The return value is ignored by CEF.
 *
 * @param handler   Handler
 * @param browser   Pointer to the browser
 * @param frame     Not sure yet
 * @param url       The address that it was changed to
 *
 * @return RV_CONTINUE
 */

CEF_CALL handle_address_change(struct _cef_handler_t* handler, cef_browser_t* browser,
                               cef_frame_t* frame, const wchar_t* url)
{
    CF_DB_START
    
    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));

    if (curChef) 
        g_Signal->AddressBarChanged(curChef,CEF_String(url).ToMultiByte());
    else {
        CF_DB(fprintf(stderr, "\t%s called on non-mChef window. Handle: %d, Popup: %i.\n",
            __FUNCTION__, browser->get_window_handle(browser), browser->is_popup(browser)));
    }

    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_title_change.  Called when the page title changes.
 * The return value is ignored by CEF.
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser
 * @param title         New title
 *
 * @return RV_HANDLED
 */
CEF_CALL handle_title_change(struct _cef_handler_t* handler, cef_browser_t* browser, 
                             const wchar_t* title)
{
    CF_DB_START

    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));

    if (curChef && g_Signal->TitleBarChanged(curChef,CEF_String(title).ToMultiByte()))
            SetWindowTextA(curChef->m_hWnd,g_Signal->GetEvaluatedData());    // ease of use force ansi string one so we dont have to convert back to multi char
    else {
        CF_DB(fprintf(stderr, "\t%s called on non-mChef window. Handle: %d, Popup: %i.\n",
            __FUNCTION__, browser->get_window_handle(browser), browser->is_popup(browser)));
    }

    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_before_browse. Called before browser navigation.
 * If we want to change/modify the request object, this is where we will do so.
 *
 * @param handler           Handler
 * @param browser           Pointer to the browser
 * @param frame             ?
 * @param request           Pointer to the request
 * @param navType           Navigation type
 * @param isRedirect        Boolean: redirected?
 *
 * @return RV_CONTINUE      Continue navigation
 * @return RV_HANDLED       Cancel navigation
 */
CEF_CALL handle_before_browse(struct _cef_handler_t* handler, cef_browser_t* browser,
                              cef_frame_t* frame, struct _cef_request_t* request, 
                              cef_handler_navtype_t navType, int isRedirect)
{
    CF_DB_START

    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));
    if (curChef)
    {
        bool bSuccess = g_Signal->NavigateBegin(curChef, navType,isRedirect,CEF_String(request->get_url(request)).ToMultiByte());
        if (bSuccess && !_strcmpi(g_Signal->GetEvaluatedData(),"C_Cancel"))
        {
            BaseRelease(frame);
            BaseRelease(request);
            BaseRelease(browser);
            return RV_HANDLED;
        }
        else if (bSuccess)
            request->set_url(request, CEF_String(g_Signal->GetEvaluatedData()).ToWideChar());
    }
    else {
        CF_DB(fprintf(stderr, "\t%s called on non-mChef window. Handle: %d, Popup: %i.\n",
            __FUNCTION__, browser->get_window_handle(browser), browser->is_popup(browser)));
    }
    
    BaseRelease(frame);
    BaseRelease(request);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_load_start. Called when we begin to load a page.
 * The return value is ignored by CEF.
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser
 * @param frame         ?
 *
 * @return RV_CONTINUE
 */
CEF_CALL handle_load_start(struct _cef_handler_t* handler, cef_browser_t* browser, cef_frame_t* frame)
{
    CF_DB_START

    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));

    if (curChef) {
        if (frame) {
            curChef->bIsLoading = true;
            curChef->bCanGoBack = curChef->bCanGoForward = false;
        }
        g_Signal->LoadStart(curChef);
    }
    else {
        CF_DB(fprintf(stderr, "\t%s called on non-mChef window. Handle: %d, Popup: %i.\n",
            __FUNCTION__, browser->get_window_handle(browser), browser->is_popup(browser)));
    }

    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_load_end. Called when we finished loading a page REGARDLESS if it was successfully or not.
 * The return value is ignored by CEF.
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser
 * @param frame         ?
 *
 * @return RV_CONTINUE
 */
CEF_CALL handle_load_end(struct _cef_handler_t* handler, cef_browser_t* browser, cef_frame_t* frame)
{
    CF_DB_START

    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));

    if (curChef) {
        if (frame) {
            curChef->bIsLoading = false;
            curChef->bCanGoForward = browser->can_go_forward(browser) ? true : false;
            curChef->bCanGoBack = browser->can_go_back(browser) ? true : false;
        }
        g_Signal->LoadEnd(curChef);
    }
    else {
        CF_DB(fprintf(stderr, "\t%s called on non-mChef window. Handle: %d, Popup: %i.\n",
            __FUNCTION__, browser->get_window_handle(browser), browser->is_popup(browser)));
    }
    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_load_error. Called when we fail to load a resource.
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser
 * @param frame         ?
 * @param errorCode     Error code number
 * @param failedUrl     URL that failed to load
 * @param errorText     Our own custom error text (If NULL we must return RV_CONTINUE; else RV_HANDLED)
 *
 * @return RV_CONTINUE  Return the default error text
 * @return RV_HANDLED   Return our own custom error text
 */
CEF_CALL handle_load_error(struct _cef_handler_t* handler, cef_browser_t* browser, cef_frame_t* frame,
                           cef_handler_errorcode_t errorCode, const wchar_t* failedUrl,
                           cef_string_t* errorText)
{
    CF_DB_START
    
    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));
    if (curChef && g_Signal->LoadError(curChef,errorCode,CEF_String(failedUrl).ToMultiByte())) {
        cef_string_t eText = CEF_String(g_Signal->GetEvaluatedData()).ToCefString();
        errorText = &eText;

        BaseRelease(frame);
        BaseRelease(browser);
        CF_DB_END
        return RV_HANDLED;
    }
    else {
        CF_DB(fprintf(stderr, "\t%s called on non-mChef window. Handle: %d, Popup: %i.\n",
            __FUNCTION__, browser->get_window_handle(browser), browser->is_popup(browser)));
    }
        
    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_before_resource_load. Called before we load a resource.
 *
 * @param handler           Handler
 * @param browser           Pointer to the browser
 * @param request           Pointer to the request
 * @param redirectUrl       If we want to redirect the resource, we populate this and return RV_CONTINUE
 * @param resourceStream    If we want to specify data for the resource, we must create a CefStream object
 * @param mimeType          If we populate resourceStream we must set this mime to the same type as the resource's stream.
 * @param loadFlags         Load Flags...NFC
 *
 * @return RV_CONTINUE      To continue
 * @return RV_HANDLED       To cancel
 */
CEF_CALL handle_before_resource_load(struct _cef_handler_t* handler, cef_browser_t* browser,
                                     struct _cef_request_t* request, cef_string_t* redirectUrl,
                                     struct _cef_stream_reader_t** resourceStream, cef_string_t* mimeType,
                                     int loadFlags)
{
    BaseRelease(request);
    BaseRelease(browser);

    return RV_CONTINUE;
}

/**
 * handle_before_menu. Called before our menu is displayed.
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser
 * @param menuInfo      Menu info
 *
 * @return RV_CONTINUE  Continue loading the default menu
 * @return RV_HANDLED   Cancel the default menu
 */
CEF_CALL handle_before_menu(struct _cef_handler_t* handler, cef_browser_t* browser,
                            const cef_handler_menuinfo_t* menuInfo)
{
    CF_DB_START

    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_get_menu_label. Called if we want to override the default text for the menu items.
 * The return value is ignored by CEF
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser.
 * @param menuId        Menu ID
 * @param label         Default text.  We can change this if we want.
 *
 * @return RV_CONTINUE
 */
CEF_CALL handle_get_menu_label(struct _cef_handler_t* handler, cef_browser_t* browser,
                               cef_handler_menuid_t menuId, cef_string_t* label)
{
    CF_DB_START
    
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_menu_action.  Called when an option is selected from our menu.
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser
 * @param menuId        Menu ID
 *
 * @return RV_CONTINUE  Let CEF handle it
 * @return REF_HANDLED  We've handled it
 */
CEF_CALL handle_menu_action(struct _cef_handler_t* handler, cef_browser_t* browser,
                            cef_handler_menuid_t menuId)
{
    CF_DB_START
    
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_print_header_footer. Called when we want to format the headers and footers of the print context.
 *
 * @param handler           Handler
 * @param browser           Pointer to the browser
 * @param frame             ?
 * @param printInfo         Information about the printer context.
 * @param url               The page being currently printed
 * @param title             The title of the page
 * @param currentPage       Current page number
 * @param maxPages          Total number of pages
 * @param topLeft           Header Location: Top Left
 * @param topCenter         Header Location: Top Center
 * @param topRight          Header Location: Top Right
 * @param bottomLeft        Header Location: Bottom Left
 * @param bottomCenter      Header Location: Bottom Center
 * @param bottomRight       Header Location: Bottom Right
 *
 * @return RV_CONTINUE      Let CEF handle this.
 * @return RV_HANDLED       Ok we printed our own header & footer.
 */
CEF_CALL handle_print_header_footer(struct _cef_handler_t* handler, cef_browser_t* browser, cef_frame_t* frame,
                                    cef_print_info_t* printInfo, const wchar_t* url, const wchar_t* title,
                                    int currentPage, int maxPages, cef_string_t* topLeft,
                                    cef_string_t* topCenter, cef_string_t* topRight,
                                    cef_string_t* bottomLeft, cef_string_t* bottomCenter,
                                    cef_string_t* bottomRight)
{
    CF_DB_START
    
    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_jsalert.  Called when there is a JS alert message.
 *
 * @param handler           Handler
 * @param browser           Pointer to the browser
 * @param frame             ?
 * @param message           Our own custom message
 *
 * @return RV_CONTINUE      Display default alert text
 * @return RV_HANDLED       Display our custom alert text
 */
CEF_CALL handle_jsalert(struct _cef_handler_t* handler, cef_browser_t* browser,
                        cef_frame_t* frame, const wchar_t* message)
{
    CF_DB_START
    
    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_jsconfirm.     Called when there is a JS Confirm request.
 *
 * @param handler           Handler
 * @param browser           Pointer to the browser
 * @param frame             ?
 * @param message           Our own custom message
 * @param retval            Has the user accepted the confirmation?
 * 
 * @return RV_CONTINUE      Display default text
 * @return RV_HANDLED       Display our custom text
 */
CEF_CALL handle_jsconfirm(struct _cef_handler_t* handler, cef_browser_t* browser,
                          cef_frame_t* frame, const wchar_t* message, int* retval)
{
    CF_DB_START
    
    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_jsprompt.  Called when there is a JS Prompt.
 *
 * @param handler           Handler
 * @param browser           Pointer to the browser
 * @param frame             ?
 * @param message           The message of the prompt
 * @param defaultValue      The default text
 * @param retval            Has the user accepted the request?
 * @param result            The resulting value
 *
 * @return RV_CONTINUE      Display default prompt
 * @return RV_HANDLED       Display our custom prompt
 */
CEF_CALL handle_jsprompt(struct _cef_handler_t* handler, cef_browser_t* browser, cef_frame_t* frame,
                         const wchar_t* message, const wchar_t* defaultValue, int* retval,
                         cef_string_t* resul)
{
    CF_DB_START
    
    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_before_window_close.  Called before a window is closed.
 * The return value is ignored by CEF.
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser
 *
 * @return RV_CONTINUE
 */
CEF_CALL handle_before_window_close(struct _cef_handler_t* handler, cef_browser_t* browser)
{
    CF_DB_START

    HWND m_hWnd = browser->get_window_handle(browser);
    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));

    /* The following is a small little hack. It appears that the Window is not
     * completely destroyed by CEF, and if we attempt to destroy it ourselfs,
     * well, let's just say that you will create an abyss. */
    ShowWindow(m_hWnd, SW_HIDE);

    if (curChef) {
        CF_DB(printf("Chef %s [%08X] was closed.\n", curChef->m_WindowName,
            curChef->m_hWnd));

        // If the browser window is still loading, we tell it to stop, and basically
        // run this handler again.  So in turn, this is now a recursive function.
        // http://sourceforge.net/support/tracker.php?aid=2830503
        if (curChef->bIsLoading) {
            curChef->bIsLoading = false;
            browser->stop_load(browser);
            BaseRelease(handler);
            BaseRelease(browser);

            return handler->handle_before_window_close(handler, browser);
        }

        g_Signal->Cmd("/%s %s %s", curChef->callbackAlias, curChef->m_WindowName, SIGNAL_CHEFCLOSED);
        ChefPop(curChef);
    }
    CF_DB(printf("[%08X] closed.\n", m_hWnd));
    BaseRelease(handler);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_take_focus.  Called when a component is just about to loose focus 
 *  (this includes TAB'ing thru HTML elements) The return value is ignored by CEF.
 *
 * @param handler       Handler
 * @param browser       Pointer to the browser
 * @param reverse       NFC
 *
 * @return RV_CONTINUE
 */
CEF_CALL handle_take_focus(struct _cef_handler_t* handler, cef_browser_t* browser, int reverse)
{
    CF_DB_START
    
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * handle_set_focus.  Called when a browser or a web widget are about to set the focus.
 *
 * @param handler       Handler
 * @param browser       Browser
 * @param isWidget        1 if its the widget, 0 if its the browser
 * 
 * @return RV_CONTINUE  To let the browser set focus.
 * @return RV_HANDLED   Stops the browser from stealing focus.
 */
CEF_CALL handle_set_focus(struct _cef_handler_t* handler, cef_browser_t* browser, int isWidget)
{
    CF_DB_START

    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));
    BaseRelease(browser);

    if (curChef) {
        if (isWidget) {
            if (CanFocusWidget(curChef)) {
                CF_DB_END
                return RV_CONTINUE;
            }
        } else if (CanFocus(curChef)) {
            CF_DB_END
            return RV_CONTINUE;
        }
    }

    CF_DB_END
    return RV_HANDLED;
}

/**
 * handle_jsbinding. Called for adding values to a frame's JavaScript 'window' object.
 *  Return value is ignored by CEF.
 *
 * @param handler       Handler
 * @param browser       Browser
 * @param frame         ?
 * @param object        Object
 *
 * @return RV_CONTINUE
 */
CEF_CALL handle_jsbinding(struct _cef_handler_t* handler, cef_browser_t* browser,
                          cef_frame_t* frame, struct _cef_v8value_t* object)
{
    CF_DB_START
    
    lpChef curChef = ChefFindBrowser(browser->get_window_handle(browser));

    if (curChef) {
        // basically if they dont return C_CANCEL and they do handle the JSBind event
        if (frame->get_url(frame) && g_Signal->JSBind(curChef,CEF_String(frame->get_url(frame)).ToMultiByte()) && 
            _strcmpi(g_Signal->GetEvaluatedData(),"C_CANCEL"))
            object->set_value_bykey(object,L"mCHEF",InitializeMIRCJSLink(curChef));
    }
    else{
        CF_DB(fprintf(stderr, "\t%s called on non-mChef window. Handle: %d, Popup: %i.\n",
            __FUNCTION__, browser->get_window_handle(browser), browser->is_popup(browser)));
    }


    BaseRelease(object);
    BaseRelease(frame);
    BaseRelease(browser);

    CF_DB_END
    return RV_CONTINUE;
}

/**
 * CreateMyHandler.  Creates a new instance of a cef_handler_t.
 *
 * @return cef_handler_t*
 */
cef_handler_t *CreateMyHandler()
{
    CF_DB_START
    cef_handler_t *m = (cef_handler_t *)ChefMalloc(sizeof(cef_handler_t));

    // Initializes the base_t struct's parameters
    InitBaseStruct(&m->base);
    m->base.size                    = sizeof(cef_handler_t);        //set to the size of of the struct were working on so it knows since its all polymorphic
    m->handle_before_created        = handle_before_created;
    m->handle_after_created         = handle_after_created;
    m->handle_address_change        = handle_address_change;
    m->handle_title_change          = handle_title_change;
    m->handle_before_browse         = handle_before_browse;
    m->handle_load_start            = handle_load_start;
    m->handle_load_end              = handle_load_end;
    m->handle_load_error            = handle_load_error;
    m->handle_before_resource_load  = handle_before_resource_load;
    m->handle_before_menu           = handle_before_menu;
    m->handle_get_menu_label        = handle_get_menu_label;
    m->handle_menu_action           = handle_menu_action;
    m->handle_print_header_footer   = handle_print_header_footer;
    m->handle_jsalert               = handle_jsalert;
    m->handle_jsconfirm             = handle_jsconfirm;
    m->handle_jsprompt              = handle_jsprompt;
    m->handle_before_window_close   = handle_before_window_close;
    m->handle_take_focus            = handle_take_focus;
    m->handle_jsbinding             = handle_jsbinding;
    m->handle_set_focus             = handle_set_focus;

    BaseAddRef(m); //Increase reference before passing it so it does not get killed on the spot.

    CF_DB_END
    return m;
}

/* EOF */

