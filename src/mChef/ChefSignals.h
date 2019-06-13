/* mChef - ChefSignals.h
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
#ifndef __CHEF_SIGNALS_H__
#   define __CHEF_SIGNALS_H__

#define MaxReturnBuff        512

/**
 * ChefSignals.
 * Used to marshall data communication
 * B/T The dll and MIRC
 */
class ChefSignals : public Gen::CGenMap
{
private:
    HWND m_hWnd;                      // mIRC's handle.
    LPWSTR m_MapName;                 // Map Name;
    char m_MsgBuffer[MaxReturnBuff];  // Storage for user return information.


public:
    /**
     * ChefSignals. Constructor.
     *
     * @param mWnd      Handle to the window.
     */
    ChefSignals(HWND hWnd, LPWSTR wMapName);

    /**
     * ~CefSignals. Destructor.
     */
    ~ChefSignals();

    /**
     * GetEvaluatedData. Returns the evaluated data.
     *
     * @return _mReturn
     */
    char *GetEvaluatedData();

    /**
     * GetEvaluatedDataCopy. Returns the evaluated data.
     *  This function will allocate memory for a new pointer,
     *  copy the evaluated data to that pointer, and returns 
     *  the new pointer.
     *
     * @return data
     */
    char *GetEvaluatedDataCopy(); 

    /**
     * CustomNotify. The name says it all.
     *
     * @param chef      Pointer to the Chef structure.
     * @param signal    The signal that we are sending.
     * @param data      The data that we are sending along with the signal.
     *
     * @return true     If we succeeded.
     * @return false    If we failed.
     */
    bool CustomNotify(const lpChef ,const  char *,const  char *);

    /**
     * CustomEvaluate. The name says it all.
     *
     * @param chef      Pointer to the Chef structure.
     * @param signal    Signal that we are sending.
     * @param data      Data that we are sending along.
     *
     * @return true     If we succeeded.
     * @return false    If we failed.
     */
    bool CustomEvaluate(const lpChef ,const  char *,const  char *);

    // JS Functions
    bool JSSignal(char *script, bool bEvaluate);

    // Evaluated Functions
    bool TitleBarChanged(const lpChef ,const char*);
    bool LoadError(const lpChef,const cef_handler_errorcode_t , const char*);
    bool NavigateBegin(const lpChef, const cef_handler_navtype_t, const int, const char *);
    bool JSBind(const lpChef, const char*); 

    // Notify Functions
    bool LoadStart(const lpChef);
    bool LoadEnd(const lpChef);
    bool AddressBarChanged(const lpChef,const char*);
    bool PrinterSend(const lpChef curChef, const char *sFile, const char *sPath, bool bSuccess);
};

#endif

/* EOF */

