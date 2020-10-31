#ifndef GLDRICHTEXTEDITOLECALLBACK_H
#define GLDRICHTEXTEDITOLECALLBACK_H

#include <Qt>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <Richedit.h>
#include <RichOle.h>

class GLDRichTextEditOleCallback : public IRichEditOleCallback
{
public:
    GLDRichTextEditOleCallback(LPRICHEDITOLE lpRichEditOle);
    ~GLDRichTextEditOleCallback();

    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)(); 
    STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj); 

    
    STDMETHOD(GetNewStorage) (LPSTORAGE*);
    STDMETHOD(GetInPlaceContext) (LPOLEINPLACEFRAME*, LPOLEINPLACEUIWINDOW*, LPOLEINPLACEFRAMEINFO);
    STDMETHOD(ShowContainerUI) (BOOL);
    STDMETHOD(QueryInsertObject) (LPCLSID, LPSTORAGE, LONG);
    STDMETHOD(DeleteObject) (LPOLEOBJECT);
    STDMETHOD(QueryAcceptData) (LPDATAOBJECT, CLIPFORMAT*, DWORD, BOOL, HGLOBAL);
    STDMETHOD(ContextSensitiveHelp) (BOOL);
    STDMETHOD(GetClipboardData) (CHARRANGE*, DWORD, LPDATAOBJECT*);
    STDMETHOD(GetDragDropEffect) (BOOL, DWORD, LPDWORD);
    STDMETHOD(GetContextMenu) (WORD, LPOLEOBJECT, CHARRANGE*, HMENU*);

private:
    long          m_dwRef;

    LPRICHEDITOLE m_lpRichEditOle;
    int           m_iNumStorages;
    LPSTORAGE     m_lpStorage;  // provides storage for m_lpObject
    
};

#endif // Q_OS_WIN

#endif // GLDRICHTEXTEDITOLECALLBACK_H
