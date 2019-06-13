/* mChef - ChefPrinter.h
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

#ifndef __CHEFPRINTER_H__
#   define __CHEFPRINTER_H__

/**
 * ChefPrinter Class.  This class will handle the "printing" of Chef windows to images.
 */
class ChefPrinter
{
private:
    ImageType m_ImageOutputFormat;      /* Image Output format */
    char *m_SavePath;                   /* Path that we should save the image to */
    char m_ImageName[MAX_PATH];                  /* The name of the image. */
    char m_FullImagePath[MAX_PATH];     /* Full path to the image (filename included) */
    lpChef m_curChef;                   /* We will only keep this as a reference. */
    HDC   m_ChefDC;                     /* Device Context */
    HBITMAP hBitmap;                    /* Bitmap */
    BITMAPINFO b_Info;                  /* Bitmap Info */
    BITMAPFILEHEADER b_Header;          /* Bitmap Header */
    LPVOID pBuf;                        /* Don't worry about it */
    bool bFullView;                     /* Are we printing the full page or just what we see? */
    
public:
    /**
     * ChefPrinter Constructor #1.
     *
     * @param curChef       Chef window that we are going to print.
     * @param ImageType     What type of format should we print to?
     */
    ChefPrinter(lpChef curChef, ImageType iType, bool bView);

    /**
     * ~ChefPrinter Deconstructor.
     */
    ~ChefPrinter();

    /**
     * Release. THIS MUST BE CALLED BEFORE DELETING THE CLASS.
     */
    void Release();

    /**
     * BuildImageName. Will fill m_ImageName accordingly.
     *
     * @return true     If we could build an image name.
     * @return false    If we failed.
     */
    bool BuildImageName();

    /**
     * BuildBitmapHeaders. Builds the Bitmap headers.
     */
    void BuildBitmapHeaders();

    /**
     * BuildBitmap. This function will copy the context of the window and 
     *      build a bitmap accordingly.
     */
    void BuildBitmap();

    /**
     * SaveBitmap. Saves the image as a BMP.
     */
    void SaveBitmap();

    /**
     * SavePng. Saves the image as PNG.
     */
    void SavePng();

    /**
     * CheckPath. Checks to see if m_SavePath is a valid path.
     *
     * @return false
     * @return true
     */
    bool CheckPath();

    /**
     * Print. Prints the Chef.
     */
    void Print();
};
#endif

/* EOF */

