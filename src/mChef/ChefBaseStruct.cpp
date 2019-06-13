/* mChef - ChefBaseStruct.cpp
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

/* Vector for our lpBases */
static std::vector<lpBases> vBases;

/**
 * Since we are going to be checking the size of the bases quite a bit,
 *   I think it only makes sense that we store them instead of checking them
 *  every time that we need to find out what's what. 
 */
static size_t SizeOfCefBase     = sizeof(cef_base_t);
static size_t SizeOfCefBrowser  = sizeof(cef_browser_t);
static size_t SizeOfCefHandler  = sizeof(cef_handler_t);
static size_t SizeOfV8Handler   = sizeof(cef_v8handler_t);
static size_t SizeOfV8Value     = sizeof(cef_v8value_t);

/**
 * BaseIdentify.    Identifies a base by size.
 *
 * @param lpbase    Base that we want to identify.
 */
BaseType BaseIdentify(lpBases lpbase)
{

    if (!lpbase || !lpbase->base)
        return BaseUnknown;
    else if (lpbase->base->size == SizeOfCefBase) 
        return BaseCef;
    else if (lpbase->base->size == SizeOfCefBrowser)
        return BaseCefBrowser;
    else if (lpbase->base->size == SizeOfCefHandler)
        return BaseCefHandler;
    else if (lpbase->base->size == SizeOfV8Handler)
        return BaseV8Handler;
    else if (lpbase->base->size == SizeOfV8Value)
        return BaseV8Value;

    /* This should never be reached. */
    return BaseUnknown;
}

/**
 * BaseIdentifyString.    Identifies a base by size.
 *
 * @param lpbase       Base that we want to identify.
 *
 * @return String 
 */
const char *BaseIdentifyString(lpBases lpbase)
{
    if (!lpbase || !lpbase->base)
        return "UnknownItsOverNineThousand";
    else if (lpbase->base->size == SizeOfCefBase) 
        return "BaseCef";
    else if (lpbase->base->size == SizeOfCefBrowser)
        return "BaseCefBrowser";
    else if (lpbase->base->size == SizeOfCefHandler)
        return "BaseCefHandler";
    else if (lpbase->base->size == SizeOfV8Handler)
        return "BaseV8Handler";
    else if (lpbase->base->size == SizeOfV8Value)
        return "BaseV8Value";

    /* This should never be reached. */
    return "BaseUnknown";
}

/**
 * add_ref.  Increases the reference count to an object
 *
 * @param t        base object
 *
 * @return reference count
 */
int CEF_CALLBACK add_ref(struct _cef_base_t *t)
{
    lpBases base = BaseFind(t);

    if (base) 
        return InterlockedIncrement(&base->iReferenceCount);
    
    return 1;    // safety net but this is a nasty error to have occur
}

/**
 * get_refct.  Returns the reference count to an object
 *
 * @param t        base object
 *
 * @return reference count
 */
int CEF_CALLBACK get_refct(struct _cef_base_t *t)
{
    CF_DB_START
    lpBases base = BaseFind(t);

    if (base) 
        return base->iReferenceCount;
    
    CF_DB_END
    return 1;    // safety net but this is a nasty error to have occur
}

/**
 * release.  Decreases the reference count to an object
 *
 * @param t        base object
 *
 * @return reference count
 */
int CEF_CALLBACK release(struct _cef_base_t *t)
{
    lpBases base = BaseFind(t);

    if (base) {
        if (base->iReferenceCount-1 == 0) {
            CF_DB(printf("[%s] Auto-BasePoping, references == 0\n", 
                    BaseIdentifyString(base)));
            BasePop(base->base);
            return 0;
        }
        return InterlockedDecrement(&base->iReferenceCount);
    }

    return 1;    // safety net but this is a nasty error to have occur
}

/**
 * InitBaseStruct.  Initializes a base struct, NEVER SET THE SIZE BEFORE CALLING THIS, it will zero it out!!!!
 *
 * @param base        Structure to initialize
 *
 * @return pointer to structure
 */
cef_base_t *InitBaseStruct(cef_base_t *base)
{
    base->add_ref       = add_ref;
    base->get_refct     = get_refct;
    base->release       = release;
    base->size          = 0;

    lpBases lpBase = (lpBases)ChefMalloc(sizeof(Bases));

    lpBase->base = base;
    lpBase->iReferenceCount = 0;
    vBases.push_back(lpBase);

    return base;
}

/**
 * BaseFree - deletes a struct based on the sizeof its base struct member
 *
 * @param base        the base to delete
 */
void BaseFree(lpBases lpbase)
{
    BaseType curBaseType = BaseIdentify(lpbase);

    CF_DB(printf("Freeing %s\n", BaseIdentifyString(lpbase)));

    /* for right now the only thing we have using our custom logic is the handler, 
     * ultimately the v8 stuff will need it as well */
    if (curBaseType == BaseCef)  {
        Free((cef_base_t *)lpbase->base);
    }
    else if (curBaseType == BaseCefHandler) {
        Free((cef_handler_t *)lpbase->base);
    }
    else if (curBaseType == BaseCefBrowser) {
        Free((cef_browser_t *)lpbase->base);
    }
    else if (curBaseType == BaseV8Handler) {
        Free((cef_v8handler_t *)lpbase->base);
    }
    else if (curBaseType == BaseV8Value) {
        Free((cef_v8value_t *)lpbase->base);
    }
    else {
        CF_DB(printf("UH OH UH OH UH OH UH OH!! BaseFree called for unknown base with size: %u\n",
            lpbase->base->size));
    }
    Free(lpbase);
}

/**
 * BaseFind.  Returns the reference count to an object
 *
 * @param base        base pointer
 *
 * @return reference to a lpBases Struct containing your base
 */
lpBases BaseFind(cef_base_t *base)
{
    for (std::vector<lpBases>::iterator it = vBases.begin(); it != vBases.end(); it++ )
        if ((*it)->base == base)
            return (*it);

        return NULL;
}

/**
 * BasePop  Removes all bases and frees memory
 */
void BasePopThemAll()
{
    CF_DB_START
    lpBases curBase = NULL;

    while (!vBases.empty()) {
        curBase = vBases.front();
        BaseFree(curBase);
    }

    vBases.clear();   // were not responsible for our memory of the bases so all we have to do is free each struct and erase away
    CF_DB_END
}

/**
 * RemoveBase  Removes a single base and frees memory
 *
 * @param base      Base Structure to remove
 *
 * @return true
 * @return false
 */
bool BasePop(cef_base_t *base)
{
    CF_DB_START

    for (std::vector<lpBases>::iterator it = vBases.begin(); it != vBases.end(); it++ ) {
        if ((*it)->base == base) {
            BaseFree(*it);
            vBases.erase(it);
            return true;
        }
    }

    CF_DB(printf("%s(Line %i): %s failed. Remove Base Structure Failed\n", __FILE__, __LINE__, __FUNCTION__));
    CF_DB_END
    return false;
}

/* EOF */

