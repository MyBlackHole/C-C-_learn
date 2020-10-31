/*!
 * \file GLDImageObject.h
 *
 * \author xieg-a
 * \date ¾ÅÔÂ 2014
 *
 *  HBITMAP bmp = (HBITMAP)::LoadImageW(NULL, (LPCWSTR)image.data(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
 */
#ifndef __GLDImageObject_H__
#define __GLDImageObject_H__

#include <Windows.h>
#include <Richedit.h>
#include <ObjIdl.h>
#include <RichOle.h>

class GLDImageObject : public IDataObject
{
public:
    static GLDImageObject * insertBitmap(IRichEditOle * ipRichEditOle, HBITMAP hBitmap, DWORD dwUser, int w, int h);

public:
    GLDImageObject();
    ~GLDImageObject();

    STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject)
    {
        if (iid == IID_IUnknown || iid == IID_IDataObject)
        {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }
        
        return E_NOINTERFACE;
    }

    STDMETHOD_(ULONG, AddRef)(void)
    {
        ++m_ulRefCnt;
        return m_ulRefCnt;
    }

    STDMETHOD_(ULONG, Release)(void)
    {
        if (--m_ulRefCnt == 0)
        {
            delete this;
        }

        return m_ulRefCnt;
    }

    // Methods of the IDataObject Interface 
    STDMETHOD(GetData)(FORMATETC *pformatetcIn, STGMEDIUM *pmedium) {
        HANDLE hDst;
        hDst = ::OleDuplicateData(m_stgmed.hBitmap, CF_BITMAP, NULL);
        if (hDst == NULL)
        {
            return E_HANDLE;
        }

        pmedium->tymed = TYMED_GDI;
        pmedium->hBitmap = (HBITMAP)hDst;
        pmedium->pUnkForRelease = NULL;

        return S_OK;
    }

    STDMETHOD(GetDataHere)(FORMATETC* pformatetc, STGMEDIUM*  pmedium)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(QueryGetData)(FORMATETC*  pformatetc)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(GetCanonicalFormatEtc)(FORMATETC*  pformatectIn, FORMATETC* pformatetcOut)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(SetData)(FORMATETC* pformatetc, STGMEDIUM*  pmedium, BOOL  fRelease)
    {
        m_fromat = *pformatetc;
        m_stgmed = *pmedium;

        return S_OK;
    }

    STDMETHOD(EnumFormatEtc)(DWORD  dwDirection, IEnumFORMATETC**  ppenumFormatEtc)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(DAdvise)(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
        DWORD *pdwConnection)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(DUnadvise)(DWORD dwConnection)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(EnumDAdvise)(IEnumSTATDATA **ppenumAdvise)
    {
        return E_NOTIMPL;
    }

    // Some Other helper functions 
    void setBitmap(HBITMAP hBitmap);
    IOleObject *getOleObject(IOleClientSite *pOleClientSite, IStorage *pStorage);

    void setRelease(bool release){
        m_bRelease = release;
    }

private:
    ULONG      m_ulRefCnt;
    BOOL       m_bRelease;
    STGMEDIUM  m_stgmed;
    FORMATETC  m_fromat;

    IStorage       *m_pStorage;
    IOleObject     *m_pOleObject;
};

#endif/*__GLDImageObject_H__*/
