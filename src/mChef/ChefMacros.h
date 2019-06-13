/* mChef - ChefMacros.h
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

#ifndef __CHEFMACROS_H__
#define __CHEFMACROS_H__

/**
 * Sanity Check.
 */
#ifndef __cplusplus
#   error "Why are you compiling mChef and not using a C++ compiler?"
#endif

/**
 * General Purpose Chef Macros.
 */
#define CHEF_MAXPARAMS      10
 
/**
 * mIRC Signal's Defined
 */ 
#define SIGNAL_CHEFATTACHED         "ChefAttached"             // Houston, we have attached.
#define SIGNAL_CHEFCLOSED           "ChefClosed"               // Houston, we have closed/detached.
#define SIGNAL_ADDRESSCHANGED       "AddressBarChanged"        // address is given 
#define SIGNAL_NAVBEGIN             "NavigateBegin"            // params: navtype isredirect url, return: C_CANCEL to halt, or new url to chagne request, nothing causes it to continue navigation
#define SIGNAL_TITLECHANGED         "TitleChanged"             // title is evaluated, return new title or current one will be used
#define SIGNAL_STARTLOAD            "StartLoad"                // No Params Given
#define SIGNAL_ENDLOAD              "EndLoad"                  // No params given
#define SIGNAL_LOADERROR            "LoadError"                // error code failedurl, return custom message or default will be used
#define SIGNAL_JSBIND               "JSBind"                   // javascript is binding, if C_CANCEL dont provide the mCHEF object
#define SIGNAL_CHEFPRINTER          "ChefPrinter"              // Chef Printer.

#if defined(_DEBUG)
#   define _CHEFSSERT(x)        _ASSERT((x))
#else
#   define _CHEFSSERT
#endif

/**
 * General Purpose Debugging Macros.
 */
#ifdef CHEF_DEBUG
#   define CF_DB(x)     x
#   define CF_DB_START                                              \
        clock_t m_tStart = clock();
#   define CF_DB_END                                                \
        printf("[Speed Demon] %s:%s executed in %f\n", __FILE__, __FUNCTION__,    \
                (double) (clock() - m_tStart)/CLOCKS_PER_SEC);
#else
#   define CF_DB(x)
#   define CF_DB_START
#   define CF_DB_END
#endif

/**
 * MyFree
 *  Use this to free pointers created via the "new" keyword.
 */
#define MyFree(x) \
    do {          \
    if ((x))      \
    delete ((x)); \
    } while(0);

/**
 * MyFreeEx
 *  Use this to free pointers created via the "new []" keyword.
 */
#define MyFreeEx(x) \
    do {            \
    if ((x))        \
    delete [] ((x));\
    } while(0);

/**
* Free
*  Use this to free pointers created via the "new []" keyword.
*/
#define Free(x)     \
    do {            \
    if ((x))        \
    free((x));      \
    } while(0);

/**
 * ParseParameters
 *  Use this at the very start of a function that you need to parse mIRC's data.
 */
#define ParseParameters(x)\
    char *parv[CHEF_MAXPARAMS];\
    int parc = 0;\
    Parse((x), parv, &parc);

/**
 * ParseParametersEx
 *  Same as above, but it also declares the hWnd handle from parv[0]
 */
#define ParseParametersEx(x)\
    char *parv[CHEF_MAXPARAMS];\
    int parc = 0;\
    Parse((x), parv, &parc);\
    HWND hWnd = parv[0] ? (HWND) atol(parv[0]) : NULL;

/**
 * CheckWindowByHandle
 *  Checks to see if a window exists by the given handle.
 *  Note that this only checks to see if its really a window, 
 *  and it does not check if we are attached to that window handle.
 */
#define CheckWindowByHandle(x)\
    do {\
        if (IsWindow((x))==false) {\
            sprintf(data,"C_ERR Invalid Window Handle");\
            return 3;\
        }\
    } while(0);

/**
 * CheckWindowByName
 *  Checks to see if we are attached to a window, by the given window name.
 */
#define CheckWindowByName(x)\
    do {\
        if (ChefFind((x))) {\
            sprintf(data, "C_ERR Window Already Exists");\
            return 3;\
        }\
    } while (0);
   
/**
 * SanityCheck
 *  Checks to see if we have initialized. If we haven't, we attempt to.
 */
#define SanityCheck()\
    do {\
        if ((!g_bCefInit) && !ChefInitialize(false)) {\
            sprintf(data, "C_ERR Initialization of CEF failed");\
            return 3;\
        }\
    } while(0);

#endif

/**
 * Macros for general CEF callback stuff.
 */
#define CEF_CALL    cef_retval_t CEF_CALLBACK
#define CEF_V8CALL    int    CEF_CALLBACK

/**
 * Macros to handle cef_base_t references.
 */
#define BaseAddRef(x) if (x) ((cef_base_t*)x)->add_ref((cef_base_t*)x);
#define BaseRelease(x) if (x) ((cef_base_t*)x)->release((cef_base_t*)x);

/**
 * Window Flag Types
 */
#define FLAG_PICWIN             0x000001
#define FLAG_DIALOG             0x000002
#define FLAG_LISTBOX            0x000004
#define FLAG_POPUP              0x000008
#define FLAG_CANFOCUS           0x000010
#define FLAG_CANFOCUSWIDGET     0x000020
#define FLAG_TREEBAR            0x000040
#define FLAG_TOOLBAR            0x000080
#define FLAG_ADDRESSBAR			0x000200

/**
 * Macros for verifying flags.
 */
#define IsPicWin(x)         ((x)->flags & FLAG_PICWIN)
#define IsDialog(x)         ((x)->flags & FLAG_DIALOG)
#define IsListBox(x)        ((x)->flags & FLAG_LISTBOX)
#define IsPopUp(x)          ((x)->flags & FLAG_POPUP)
#define CanFocus(x)         ((x)->flags & FLAG_CANFOCUS)
#define CanFocusWidget(x)   ((x)->flags & FLAG_CANFOCUSWIDGET)
#define IsTreeBar(x)        ((x)->flags & FLAG_TREEBAR)
#define IsToolBar(x)        ((x)->flags & FLAG_TOOLBAR)
#define HasAddressBar(x)	((x)->flags & FLAG_ADDRESSBAR)

/**
 * Macros for setting flags.
 */
#define SetPicWin(x)        ((x)->flags |= FLAG_PICWIN)
#define SetDialog(x)        ((x)->flags |= FLAG_DIALOG)
#define SetListBox(x)       ((x)->flags |= FLAG_LISTBOX)
#define SetPopUp(x)         ((x)->flags |= FLAG_POPUP)
#define SetFocus(x)         ((x)->flags |= FLAG_CANFOCUS)
#define SetFocusWidget(x)   ((x)->flags |= FLAG_CANFOCUSWIDGET)
#define SetTreeBar(x)       ((x)->flags |= FLAG_TREEBAR)
#define SetToolBar(x)       ((x)->flags |= FLAG_TOOLBAR)
#define SetAddressBar(x)	((x)->flags |= FLAG_ADDRESSBAR)

/**
 * Macros for clearing flags.
 */
#define ClearPicWin(x)      ((x)->flags &= ~FLAG_PICWIN)
#define ClearDialog(x)      ((x)->flags &= ~FLAG_DIALOG)
#define ClearListBox(x)     ((x)->flags &= ~FLAG_LISTBOX)
#define ClearPopUp(x)       ((x)->flags &= ~FLAG_POPUP)  
#define ClearFocus(x)       ((x)->flags &= ~FLAG_CANFOCUS)
#define ClearFocusWidget(x) ((x)->flags &= ~FLAG_CANFOCUSWIDGET)
#define ClearTreeBar(x)     ((x)->flags &= ~FLAG_TREEBAR)
#define ClearToolBar(x)     ((x)->flags &= ~FLAG_TOOLBAR)
#define ClearAddressBar(x)	((x)->flags &= ~FLAG_ADDRESSBAR)


/**
 * Toolbar Defines
 *
 */
#define MC_TOOLBAR_BACK		301
#define MC_TOOLBAR_FORWARD	302
#define MC_TOOLBAR_REFRESH	303
#define MC_TOOLBAR_GO		304
#define MC_TOOLBAR_EDIT		305

/* EOF */

