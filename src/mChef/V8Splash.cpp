/* mChef - V8Splash.cpp
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
#include "Chef.h"

// Used to handle executions of the mIRCLink object
CEF_V8CALL execute(struct _cef_v8handler_t* v8handler, const wchar_t* name, struct _cef_v8value_t* object, size_t numargs,
                        struct _cef_v8value_t*const* args, struct _cef_v8value_t** retval, cef_string_t* exception)
{
    bool bHandled = false;
    if (!wcscmp(name,L"evaluate"))
    {
        // Do Eval Stuff
        if (numargs != 1)
            *exception = CEF_String("Invalid Parameters: Evaluate(\"mIRC Script\");").ToCefString();
        else if (!g_Signal->JSSignal(CEF_String((wchar_t*)args[0]->get_string_value(args[0])).ToMultiByte(),true))
            *exception = CEF_String("mIRC Communication Failed").ToCefString();
        else
            *retval = cef_v8value_create_string(CEF_String(g_Signal->GetEvaluatedData()).ToWideChar());
        bHandled = true;
    }
    else if (!wcscmp(name,L"command"))
    {
        // Do Command Stuff
        if (numargs != 1)
            *exception = CEF_String("Invalid Parameters: Command(\"mIRC Script\");").ToCefString();
        else if (!g_Signal->JSSignal(CEF_String((wchar_t*)args[0]->get_string_value(args[0])).ToMultiByte(),false))
            *exception = CEF_String("mIRC Communication Failed").ToCefString();
        bHandled = true;
    }

    for (int i = 0; i < (int)numargs; i++)
        BaseRelease(args[i]);
    BaseRelease(object);
    return bHandled ? 1 : 0;
}

// Internally initializes a v8 handler for the mIRC object
cef_v8handler_t *InitV8Handler(void)
{
    cef_v8handler_t *handler = (cef_v8handler_t *)ChefMalloc(sizeof(cef_v8handler_t));
    memset(handler,0,sizeof(cef_v8handler_t));
    InitBaseStruct(&handler->base);
    handler->base.size = sizeof(cef_v8handler_t);
    handler->execute = execute;
    return handler;
}

/*
    InitializeMIRCJSLink.  Name says what it does duh...
    Notes: Only call during a JSBinding or Execute event of a v8handler, this function relies on cef functions that are only valid during these times

    @return cef_v8value_t containing a built mIRCLink object
*/
cef_v8value_t *InitializeMIRCJSLink(lpChef chef)
{
    cef_base_t *base = (cef_base_t *)ChefMalloc(sizeof(cef_base_t));
    InitBaseStruct(base);
    cef_v8value_t *mCHEFObject = cef_v8value_create_object(base);    // top level mCHEF object
    
    mCHEFObject->set_value_bykey(mCHEFObject,L"version",cef_v8value_create_string(MCHEF_VERSION_STRWC));    // mCHEF.version
    mCHEFObject->set_value_bykey(mCHEFObject,L"windowName",cef_v8value_create_string(CEF_String(chef->m_WindowName).ToWideChar()));    // mCHEF.windowName
    mCHEFObject->set_value_bykey(mCHEFObject,L"browserHWND",cef_v8value_create_string(CEF_String((long)chef->m_BrowserHwnd).ToWideChar()));    // mCHEF.browserHWND
    mCHEFObject->set_value_bykey(mCHEFObject,L"windowHWND",cef_v8value_create_string(CEF_String((long)chef->m_hWnd).ToWideChar()));    // mCHEF.windowHWND

    base = (cef_base_t *)ChefMalloc(sizeof(cef_base_t));
    InitBaseStruct(base);
    cef_v8value_t *mIRCObject = cef_v8value_create_object(base);    // Child mCHEF.mIRC object

    cef_v8handler_t *mIRCHandler = InitV8Handler();                    // one handler to rule them all
    const int iFunctionsInObjects = 2;        // Five total functions use this handler
    for (int i = 0; i < iFunctionsInObjects; i++)
        BaseAddRef(mIRCHandler);            // must add a reference for each function we pass this biatch off too

    // Now we add any functions we want to make native to the object here, to add more functions duplicate the stuff below, change the name, then add a definition for it in execute
    mIRCObject->set_value_bykey(mIRCObject,L"evaluate",cef_v8value_create_function(L"evaluate",mIRCHandler));    // Evaluate
    mIRCObject->set_value_bykey(mIRCObject,L"command",cef_v8value_create_function(L"command",mIRCHandler));        // Command
    mCHEFObject->set_value_bykey(mCHEFObject,L"mIRC",mIRCObject);

    return mCHEFObject;
}

