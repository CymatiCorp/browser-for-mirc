/* mChef - ChefExtern.h
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
#ifndef __CHEFEXTERN_H_
#   define __CHEFEXTERN_H_

/* The following are external scope declarations. */

extern inline void ChefInsert(lpChef curChef);

extern HWND g_mIRC_hWnd;

extern void BasePopThemAll();
extern void *ChefCalloc(size_t numOfElements, size_t Size);
extern void ChefDelete(lpChef curChef);
extern void ChefPanic();
extern void ChefPop(lpChef curChef);
extern void ChefPopThemAll();
extern void ChefSend(lpChef curChef, const char *curSignal, char *fmt, ...);
extern void *ChefMalloc(size_t size);
extern void ChefPrint(lpChef curChef, ImageType m_Type, bool bFullView);
extern void CreateBrowser(HWND hwParent, char *name, char *alias, char *url = NULL, long flags = NULL, HWND hwResizeTarget = NULL);
extern void SmartCreateBrowser(HWND hwParent, char *name, char *alias, char *url, long flags, HWND hwResizeTarget);

extern lpChef ChefCreate(const HWND m_hWnd, const char *m_WindowName, char *cCallbackAlias, HWND hwResizeTarget = NULL);
extern lpChef ChefFind(const HWND m_hWnd);
extern lpChef ChefFind(const char *m_WindowName);
extern lpChef ChefFindEx(char *ptr);
extern lpChef ChefFindBrowser(const HWND m_BrowserHwnd);
extern lpChef ChefFindCount(const int iCount);
extern lpChef ChefFindByResize(const HWND m_hWnd);

extern FlagsTable g_curFlags[];
extern char *ChefErrorLookup(cef_handler_errorcode_t error);
extern char *ChefNavTypeLookup(cef_handler_navtype_t nav);
extern char *ChefStrDup(const char *_string);
extern char *BuildPath(char *Ptr, const char *Dir);

extern lpBases BaseFind(cef_base_t *);

extern ChefSignals *g_Signal;

extern cef_handler_t *CreateMyHandler();
extern cef_base_t *InitBaseStruct(cef_base_t *base);

extern bool g_bCefInit, g_bClearCacheOnExit, g_bCacheInMemory;
extern bool BasePop(cef_base_t *base);
extern bool IsAllNum(char *String);
extern bool ChefInitialize(bool memCache);
extern bool ChefSetFlags(lpChef curChef, const char *lpFlag, bool set = true);

extern int CEF_CALLBACK add_ref(struct _cef_base_t *t);
extern int CEF_CALLBACK get_refct(struct _cef_base_t *t);
extern int CEF_CALLBACK release(struct _cef_base_t *t);
extern int __stdcall UnloadDll(int mTimeout);

extern LRESULT CALLBACK SubWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
extern LRESULT CALLBACK ResizeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

extern cef_v8value_t *InitializeMIRCJSLink(lpChef);

extern wchar_t *g_lpCachePath;
extern wchar_t *wBuildPath(wchar_t *wPtr, const wchar_t *wDir);

extern unsigned int g_iChefCount;

#ifdef CHEF_DEBUG
extern void ChefGetLastError();
#endif

#endif

/* EOF */

