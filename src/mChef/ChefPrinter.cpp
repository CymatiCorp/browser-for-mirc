/* mChef - ChefPrinter.cpp
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
#include <time.h>
#include <shlwapi.h>
#include "h.h"
#include "BMGLibPNG.h"
#include "ChefPrinter.h"

ChefPrinter::ChefPrinter(lpChef curChef, ImageType iType, bool bView)
    : m_curChef(curChef), m_ImageOutputFormat(iType), bFullView(bView),
     m_SavePath(BuildPath(m_SavePath, "Screens\\")), pBuf(NULL)
{
    CF_DB_START
    ZeroMemory(m_FullImagePath, MAX_PATH);
    ZeroMemory(m_ImageName, MAX_PATH);

    CF_DB_END
}

ChefPrinter::~ChefPrinter()
{
    CF_DB_START
    
    /* Nothing to do here.  Use CefPrinter::Release() before calling delete */

    CF_DB_END
}

void ChefPrinter::Release()
{
    CF_DB_START

    MyFreeEx(m_SavePath);
    Free(pBuf);

    DeleteObject(hBitmap);

    CF_DB_END
}

bool ChefPrinter::BuildImageName()
{
    CF_DB_START

    char ext[4];

    ZeroMemory(ext, 4);

    if (m_ImageOutputFormat == IMAGE_PNG) 
        strncpy(ext, "png", 3);
    else
        strncpy(ext, "bmp", 3);

    bool bGoodToGo = false;
    for (int i = 0; i < 250; i++) {
        sprintf_s(m_ImageName, MAX_PATH, "Chef%i.%s", i, ext);
        sprintf_s(m_FullImagePath, MAX_PATH, "%s%s", m_SavePath, m_ImageName);
        if (!PathFileExistsA(m_FullImagePath)) {
            bGoodToGo = true;
            break;
        }
    }
    CF_DB_END
    return bGoodToGo;
}

void ChefPrinter::BuildBitmap()
{
    CF_DB_START

    HDC ChefDC;
    RECT ChefRT;
    int ChefWidth=0, ChefHeight=0;
    HWND m_hWnd = NULL;

    //Print the WebViewHost if we can find it.
    if (!(m_hWnd = FindWindowEx(m_curChef->m_BrowserHwnd, NULL, L"WebViewHost", NULL)))
        m_hWnd = m_curChef->m_BrowserHwnd;

    CF_DB(printf("Building Bitmap from %08X (%s)\n", m_hWnd, m_curChef->m_WindowName));

    GetWindowRect(m_hWnd, &ChefRT);
    ChefDC = GetDC(m_hWnd);
    m_ChefDC = CreateCompatibleDC(ChefDC);
    ChefWidth = ((ChefRT.right) - ChefRT.left);
    ChefHeight = (ChefRT.bottom-ChefRT.top);
    hBitmap = CreateCompatibleBitmap(ChefDC, ChefWidth, ChefHeight);
    SelectObject(m_ChefDC, hBitmap);
    BitBlt(m_ChefDC, 0, 0, ChefWidth, ChefHeight, ChefDC, 0, 0, SRCCOPY);

    ReleaseDC(m_hWnd, ChefDC);
    ReleaseDC(m_hWnd, m_ChefDC);

    CF_DB_END
}

void ChefPrinter::BuildBitmapHeaders()
{
    CF_DB_START
    HDC hDC;

    hDC = GetDC(NULL);
    memset(&b_Info, 0, sizeof(BITMAPINFO));
    b_Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    GetDIBits(hDC, hBitmap, 0, 0, NULL, &b_Info, DIB_RGB_COLORS);

    if (b_Info.bmiHeader.biSizeImage <= 0)
        b_Info.bmiHeader.biSizeImage = (b_Info.bmiHeader.biWidth * 
        abs(b_Info.bmiHeader.biHeight) * (b_Info.bmiHeader.biBitCount+7)/8);

    pBuf = ChefMalloc(b_Info.bmiHeader.biSizeImage);
    b_Info.bmiHeader.biCompression=BI_RGB;
    GetDIBits(hDC, hBitmap, 0, b_Info.bmiHeader.biHeight, pBuf, &b_Info, DIB_RGB_COLORS);
    b_Header.bfType = 0x4D42;
    b_Header.bfReserved1 = b_Header.bfReserved2 = 0;
    b_Header.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + b_Info.bmiHeader.biSizeImage);
    b_Header.bfOffBits = (DWORD) (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

    CF_DB(printf("%s: %dx%d (%dbpp)\n", __FUNCTION__, 
        b_Info.bmiHeader.biWidth, b_Info.bmiHeader.biHeight,
        b_Info.bmiHeader.biBitCount));

    if (hDC)
        ReleaseDC(NULL, hDC);

    CF_DB_END
}

void ChefPrinter::SaveBitmap()
{
    CF_DB_START
    FILE *fp;
    bool bStatus = false;

    do {
        if ((fp = fopen(m_FullImagePath, "wb"))==NULL)
            break;

        fwrite(&b_Header, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite(&b_Info.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
        fwrite(pBuf, b_Info.bmiHeader.biSizeImage, 1, fp);

        bStatus = true;
    } while (0);

    if (fp)
        fclose(fp);

    g_Signal->PrinterSend(m_curChef, m_ImageName, m_FullImagePath, bStatus);

    CF_DB_END
}

void ChefPrinter::SavePng()
{
    CF_DB_START

   bool bStatus = false;

    if (SaveBitmapToPNGFile(hBitmap, m_FullImagePath) == BMG_OK)
        bStatus = true;

    g_Signal->PrinterSend(m_curChef, m_ImageName, m_FullImagePath, bStatus);

    CF_DB_END
}

bool ChefPrinter::CheckPath()
{
    CF_DB_START

    if (!PathIsDirectoryA(m_SavePath)) {
        if (CreateDirectoryA(m_SavePath,NULL))
            return true;

        return false;
    }

    CF_DB_END
    return true;
}

void ChefPrinter::Print()
{
    CF_DB_START

    CF_DB(fprintf(stderr, "Beginning print for %s\n", m_curChef->m_WindowName));
    
    if (CheckPath()) {
        if (!BuildImageName()) {
            //FIXME: Send a signal here indicating that the "SavePath" does NOT exist.
        }
        BuildBitmap();
        BuildBitmapHeaders();

        if (m_ImageOutputFormat == IMAGE_BMP)
            SaveBitmap();
        else
            SavePng();
    } else {
        /* FIXME: Send a signal here indicating that the "SavePath" does NOT exist. */
    }

    CF_DB_END
}

/**
 * ChefPrintThread. Prints a Chef.
 *
 * @param curChef       Chef
 */
void WINAPI ChefPrintThread(LPPrinterThread curPrint)
{
    ChefPrinter *pChef = new ChefPrinter(curPrint->curChef, 
        curPrint->m_Type, curPrint->bFullView);
    
    pChef->Print();
    pChef->Release();

    delete pChef;

    //dereference pointer so we don't delete it.
    curPrint->curChef = NULL;
    Free(curPrint);
}

/**
 * ChefPrint. Threads ChefPrintThread.
 */
void ChefPrint(lpChef curChef, ImageType m_Type, bool bFullView)
{
    LPPrinterThread curPrint = (LPPrinterThread)ChefMalloc(sizeof(PrinterThread));

    curPrint->curChef = curChef;
    curPrint->m_Type = m_Type;
    curPrint->bFullView = bFullView;

    SmartCreateThread<LPPrinterThread>(&ChefPrintThread, curPrint);
}

/* EOF */

