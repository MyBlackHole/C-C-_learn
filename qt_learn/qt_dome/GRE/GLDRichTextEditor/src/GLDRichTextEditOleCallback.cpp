#include "GLDRichTextEditOleCallback.h"
#include "GLDRichTextEditGlobal.h"

// we use IID_IUnknown because richedit doesn't define an IID

GLDRichTextEditOleCallback::GLDRichTextEditOleCallback(LPRICHEDITOLE lpRichEditOle)
    : m_dwRef(0)
    , m_lpRichEditOle(lpRichEditOle)
    , m_iNumStorages(0)
    , m_lpStorage(NULL)
{
    HRESULT hr = ::StgCreateDocfile(
                NULL,
                STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */ | STGM_CREATE,
                0, &m_lpStorage);
    if (FAILED(hr))
    {
        GLD_RICH_ERROR_POS("StgCreateDocfile fail.");
    }
}

GLDRichTextEditOleCallback::~GLDRichTextEditOleCallback()
{
    if (NULL != m_lpStorage)
    {
        m_lpStorage->Release();
    }
}

STDMETHODIMP_(ULONG) GLDRichTextEditOleCallback::AddRef()
{
    return InterlockedIncrement(&m_dwRef);
}

STDMETHODIMP_(ULONG) GLDRichTextEditOleCallback::Release()
{
    if (m_dwRef == 0)
        return 0;

    LONG lResult = InterlockedDecrement(&m_dwRef);
    if (lResult == 0)
    {
        delete this;
    }
    return lResult;
}

STDMETHODIMP GLDRichTextEditOleCallback::QueryInterface(REFIID iid, LPVOID* ppvObj)
{

    HRESULT hr = S_OK;
    *ppvObj = NULL;

    if (iid == IID_IUnknown || iid == IID_IRichEditOleCallback)
    {
        *ppvObj = this;
        AddRef();
        hr = S_OK;
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

STDMETHODIMP GLDRichTextEditOleCallback::GetNewStorage(LPSTORAGE* ppstg)
{
    //通过该方法存储从粘贴板或超文本流(RTF)中读取的新对象

    HRESULT hRes = E_OUTOFMEMORY;
    ++m_iNumStorages;
    WCHAR tName[50];
    swprintf(tName, L"REOLEStorage%d", m_iNumStorages);
    hRes = m_lpStorage->CreateStorage(
                tName,
                STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
                0,
                0,
                ppstg
                );
    return hRes;
}

STDMETHODIMP GLDRichTextEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME* lplpFrame, LPOLEINPLACEUIWINDOW* lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    // 通过该方法提供了应用程序级和文档级接口，以及必要的支持In-place激活的信息
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::ShowContainerUI(THIS_ BOOL fShow)
{
    //通过该方法告知应用程序是否显示自己的操作界面
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::QueryInsertObject(THIS_ LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp) 
{
    //通过该方法向应用程序询问某个对象是否可以被插入
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::DeleteObject(THIS_ LPOLEOBJECT lpoleobj)
{
    //发出通知：一个对象即将从RichEdit控件中删除
    lpoleobj->Close(OLECLOSE_NOSAVE);
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::QueryAcceptData(THIS_ LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
    //通过该方法决定在粘贴操作或拖放操作中引入的数据是否可以被接受。
    
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::ContextSensitiveHelp(THIS_ BOOL fEnterMode)
{
    //通过该方法通知应用程序它将以上下文关联方式调度帮助。
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::GetClipboardData(THIS_ CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
{
    //通过该方法允许RichEdit的客户端（调用程序）提供自己的粘贴对象
    LPDATAOBJECT lpOrigDataObject = NULL;

    // get richedit's data object
    if (FAILED(m_lpRichEditOle->GetClipboardData(lpchrg, reco, &lpOrigDataObject)))
    {
        return E_NOTIMPL;
    }

    // allow changes
    HRESULT hRes = E_NOTIMPL;// pThis->GetClipboardData(lpchrg, reco, lpOrigDataObject, lplpdataobj);

    // if changed then free original object
    if (SUCCEEDED(hRes))
    {
        if (lpOrigDataObject != NULL)
            lpOrigDataObject->Release();
        return hRes;
    }
    else
    {
        // use richedit's data object
        *lplpdataobj = lpOrigDataObject;
        return S_OK;
    }
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::GetDragDropEffect(THIS_ BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect) 
{
    //通过该方法允许RichEdit的客户端（调用程序）设置拖动操作的效果
    return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GLDRichTextEditOleCallback::GetContextMenu(THIS_ WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu) 
{
    //通过该方法向应用程序提出通过鼠标右键事件来获取上下文菜单的请求
    return S_OK;
}
