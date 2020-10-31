/*!
 * \file GLDImageObject.cpp
 *
 * \author xieg-a
 * \date 九月 2014
 *
 *
 */
#include "GLDImageObject.h"

GLDImageObject::GLDImageObject()
    : m_ulRefCnt(0)
    , m_bRelease(FALSE)
    , m_pStorage(NULL)
    , m_pOleObject(NULL)
{
}

GLDImageObject::~GLDImageObject()
{
    unsigned long cTestCount = 0;

    if (NULL != m_pOleObject)
    {
        cTestCount = m_pOleObject->Release();
    }

    if (NULL != m_pStorage)
    {
        cTestCount = m_pStorage->Release();
    }

    if (TRUE == m_bRelease)
    {
        ::ReleaseStgMedium(&m_stgmed);
    }
}

//#import "ImageOle.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search 
//static CLSID const gif_clsid = { 0x6ADA938, 0xFB0, 0x4BC0, { 0xB1, 0x9B, 0xA, 0x38, 0xAB, 0x17, 0xF1, 0x82 } };

GLDImageObject * GLDImageObject::insertBitmap(IRichEditOle* pRichEditOle, HBITMAP hBitmap, DWORD dwUser, int w, int h)
{
    SCODE sc;

    // Get the image data object
    //
    GLDImageObject *pods = new GLDImageObject;
    LPDATAOBJECT lpDataObject = NULL; //! 就是pods

    pods->QueryInterface(IID_IDataObject, (void **)&lpDataObject);
    pods->setBitmap(hBitmap);
    // Get the RichEdit container site
    //
    IOleClientSite *pOleClientSite = NULL;
    pRichEditOle->GetClientSite(&pOleClientSite);
    // Initialize a Storage Object
    //
    LPLOCKBYTES lpLockBytes = NULL;
    sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);

    IStorage *pStorage = NULL;
    sc = ::StgCreateDocfileOnILockBytes(
                lpLockBytes,
                STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE,
                0,
                &pStorage
                );

    // The final ole object which will be inserted in the richedit control
    //
    IOleObject *pOleObject = NULL;
    pOleObject = pods->getOleObject(pOleClientSite, pStorage);

    // all items are "contained" -- this makes our reference to this object
    //  weak -- which is needed for links to embedding silent update.
    OleSetContainedObject(pOleObject, TRUE);

    // Now Add the object to the RichEdit
    //
    REOBJECT reobject;
    ZeroMemory(&reobject, sizeof(REOBJECT));
    reobject.cbStruct = sizeof(REOBJECT);

    CLSID clsid;
    sc = pOleObject->GetUserClassID(&clsid);

    reobject.clsid = clsid;
    reobject.cp = REO_CP_SELECTION;
    reobject.dvaspect = DVASPECT_CONTENT;
    reobject.poleobj = pOleObject;
    reobject.polesite = pOleClientSite;
    reobject.pstg = pStorage;
    reobject.dwUser = dwUser;
    SIZEL sizel = { 0 }; // 初始化大小
    sizel.cx = w;
    sizel.cy = h;
    reobject.sizel = sizel;

    // Insert the bitmap at the current location in the richedit control
    //
    pRichEditOle->InsertObject(&reobject);
    // Release all unnecessary interfaces
    //
    unsigned long cTestCount = 0;
    cTestCount = pOleObject->Release();
    cTestCount = pStorage->Release();
    cTestCount = lpLockBytes->Release();
    cTestCount = pOleClientSite->Release();

    pods->m_pStorage = pStorage;
    pods->m_pOleObject = pOleObject;
    //lpDataObject->Release();

    return pods;
}

//
//void GLDImageObject::InsertGif(IRichEditOle *lpRichEditOle, const char *gif_file, DWORD dwUser)
//{
//    LPOLEOBJECT  lpObject = NULL;
//    LPSTORAGE       lpStorage = NULL;
//    LPOLECLIENTSITE lpClientSite = NULL;
//    LPLOCKBYTES  lpLockBytes = NULL;
//
//    HRESULT hr = S_OK;
//    CLSID clsid = CLSID_NULL;
//    do{
//        hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
//        if (hr != S_OK || lpLockBytes == NULL)
//            break;
//
//        hr = ::StgCreateDocfileOnILockBytes(lpLockBytes, STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &lpStorage);
//        if (hr != S_OK || lpStorage == NULL)
//            break;
//
//        hr = lpRichEditOle->GetClientSite(&lpClientSite);
//        if (hr != S_OK || lpClientSite == NULL)
//            break;
//
//        try
//        {
//            HRESULT hr;
//            IGifAnimator *pGif = NULL;
//            hr = CoCreateInstance(gif_clsid, NULL, CLSCTX_INPROC_SERVER, IID_IGifAnimator, (void **)&pGif);
//            if (FAILED(hr) || pGif == NULL)
//                break;
//
//            _bstr_t bstrPath(gif_file);
//            pGif->LoadFromFile(bstrPath);
//            if (FAILED(hr))
//            {
//                pGif->Release();
//                break;
//            }
//
//            hr = pGif->QueryInterface(IID_IOleObject, (void**)&lpObject);
//            if (FAILED(hr) || lpObject == NULL)
//            {
//                pGif->Release();
//                break;
//            }
//
//            hr = OleSetContainedObject(lpObject, TRUE);
//            if (FAILED(hr))
//            {
//                pGif->Release();
//                break;
//            }
//            hr = lpObject->GetUserClassID(&clsid);
//            if (FAILED(hr))
//            {
//                pGif->Release();
//                break;
//            }
//
//            REOBJECT reobject;
//            ZeroMemory(&reobject, sizeof(REOBJECT));
//            reobject.cbStruct = sizeof(REOBJECT);
//            reobject.clsid = clsid;
//            reobject.cp = REO_CP_SELECTION;
//            reobject.dvaspect = DVASPECT_CONTENT;
//            reobject.dwFlags = REO_BELOWBASELINE;
//            reobject.dwUser = dwUser;
//            reobject.poleobj = lpObject;
//            reobject.polesite = lpClientSite;
//            reobject.pstg = lpStorage;
//            SIZEL sizel = { 0, 0 };
//            reobject.sizel = sizel;
//            hr = lpRichEditOle->InsertObject(&reobject);
//        }
//        catch (_com_error &e)
//        {
//        }
//
//    } while (FALSE);
//    if (lpLockBytes) lpObject->Release();
//    if (lpLockBytes) lpLockBytes->Release();
//    if (lpClientSite) lpClientSite->Release();
//    if (lpRichEditOle) lpRichEditOle->Release();
//}

////////////////////////////////////////////////////////////////////// 
// Construction/Destruction 
////////////////////////////////////////////////////////////////////// 

void GLDImageObject::setBitmap(HBITMAP hBitmap)
{
    STGMEDIUM stgm;
    stgm.tymed = TYMED_GDI;                    // Storage medium = HBITMAP handle
    stgm.hBitmap = hBitmap;
    stgm.pUnkForRelease = NULL;                // Use ReleaseStgMedium

    FORMATETC fm;
    fm.cfFormat = CF_BITMAP;                // Clipboard format = CF_BITMAP
    fm.ptd = NULL;                            // Target Device = Screen
    fm.dwAspect = DVASPECT_CONTENT;            // Level of detail = Full content
    fm.lindex = -1;                            // Index = Not applicaple
    fm.tymed = TYMED_GDI;                    // Storage medium = HBITMAP handle

    this->SetData(&fm, &stgm, TRUE);
}

IOleObject *GLDImageObject::getOleObject(IOleClientSite *pOleClientSite, IStorage *pStorage)
{
    /*
    OleCreateStaticFromData（）执行失败，返回结果为返回值为-2147024882
    STGMEDIUM stgm;
    stgm.tymed = TYMED_GDI;    // Storage medium = HBITMAP handle
    stgm.hBitmap = hBitmap;
    stgm.pUnkForRelease = NULL; // Use ReleaseStgMedium
    FORMATETC fm;
    fm.cfFormat = CF_BITMAP; // Clipboard format = CF_BITMAP
    确定你的hBitmap不是DIB而是in memory bitmap, 否则就会导致oom
    */
    SCODE sc;
    IOleObject *pOleObject;
    sc = ::OleCreateStaticFromData(
                this,
                IID_IOleObject,
                OLERENDER_FORMAT,
                &m_fromat,
                pOleClientSite,
                pStorage,
                (void **)&pOleObject
                );
    return pOleObject;
}
