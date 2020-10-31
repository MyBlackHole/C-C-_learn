/*!
 * \file GLDRichTextEdit.cpp
 *
 * \author xieg-a
 * \date 八月 2014
 *
 *
 */
#include "GLDRichTextEdit.h"

#include <Windows.h> // 此头文件必须放在前面，否则引起编译错误
#include <RichEdit.h>
#include <RichOle.h>
#include <TOM.h>

#include <QApplication>
#include <QAtomicInt>
#include <QAxWidget>
#include <QAxObject>
#include <QBuffer>
#include <QClipboard>
#include <QCloseEvent>
#include <QCursor>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QMutex>
#include <QMutexLocker>
#include <QResizeEvent>
#include <QUuid>
#include <QLayout>

#include "GLDRichTextEditGlobal.h"
#include "GLDRichTextEditOleCallback.h"
#include "GLDRichTextEditNativeEventFilter.h"
#include "GLDImageObject.h"

#define DEFINE_GUIDXXX(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID CDECL name \
    = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

DEFINE_GUIDXXX(IID_ITextDocument, 0x8CC497C0, 0xA1DF, 0x11CE, 0x80, 0x98,
               0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D);
DEFINE_GUIDXXX(IID_ITextDocument2, 0xC241F5E0, 0x7206, 0x11D8, 0xA2, 0xC7,
               0x00, 0xA0, 0xD1, 0xD6, 0xC6, 0xB3);
DEFINE_GUIDXXX(IID_ITextRow, 0xC241F5EF, 0x7206, 0x11D8, 0xA2, 0xC7,
               0x00, 0xA0, 0xD1, 0xD6, 0xC6, 0xB3);
DEFINE_GUIDXXX(IID_ITextPara, 0x8CC497C4, 0xA1DF, 0x11ce, 0x80, 0x98,
               0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D);

DEFINE_GUIDXXX(IID_ITextRange2, 0xC241F5E2, 0x7206, 0x11D8, 0xA2, 0xC7,
               0x00, 0xA0, 0xD1, 0xD6, 0xC6, 0xB3);

class GLDRichTextEditPrivate;

static QAtomicInt s_lFilterNum = 0;
static GLDRichTextEditNativeEventFilter *s_enentFilter = NULL;

//放大缩小最小最大比例
const double c_minZoom        = 0.1;
const double c_maxZoom        = 5.0;

// 直线的斜率和b值(由阴影区域为0--1/6--1/3（对应比例为5--1--0.1）分段计算初出)
const double c_kBigThanOne    = -1.0 / 12;
const double c_bBigThanOne    = 3.0  / 12;

const double c_kSmallThanOne  = -5.0 / 18;
const double c_bSmallThanOne  = 4.0  / 9;
// 字体和背景缩放之比
const double c_FontAndBckZoom = 1.3;
// 用到的资源图片路径
const QString c_cutIcon       = ":/smallcut.png";
const QString c_copyIcon      = ":/smallcopy.png";
const QString c_pasteIcon     = ":/smallpaste.png";

class GLDRichTextEditPrivate
{
public:
    enum RichEditVer
    {
        Msftedit,
        Richedit20,
        Richedit32
    };

    class GArchive
    {
    public:
        enum Mode
        {
            store = 0,
            load  = 1
        };

    public:
        GArchive(QIODevice *file, Mode mode)
            : m_file(file)
            , m_mode(mode)
            , m_pDocument(NULL)
        {}

        bool isLoad() const
        {
            return (load == m_mode);
        }

    public:
        QIODevice              *m_file;
        Mode                    m_mode;
        GLDRichTextEditPrivate *m_pDocument;
    };

public:
    GLDRichTextEditPrivate(GLDRichTextEdit *parent);
    ~GLDRichTextEditPrivate();

    int contentHeight();
    int contentWidth();
    void cut();
    void copy();
    void del();
    void past();
    void undo();
    void setUndoCount(int undoCount);
    void redo();

    bool canUndo() const;
    bool canRedo() const;

    void insertText(const QString &text);
    BOOL insertImage(const QString &image, int w, int h, bool bMonochrome);

    void setDefaultFont(const QFont &fnt, int size, const QColor &clr);
    void setSelectionCharFont(const QFont &font);
    void setSelectionCharBackgroundColor(const QColor &color);
    void setSelectionCharSize(int size);
    void setSelectionCharColor(const QColor &clr);
    void setSelectionCharFormat(DWORD dwMask, DWORD dwEffects);
    void setSelectionCharFormat(DWORD dwMask, DWORD dwEffects, bool enable);

    void setParaFormat(WORD align);

    void setReadOnly(bool value);

    void setZoomFactor(const double factor);

    bool findText(const QString &find, bool bCase, bool bWord, bool bNext);

    inline int minInGRE(int a, int b)
    {
        return a < b ? a : b;
    }
    inline int maxInGRE(int a, int b)
    {
        return a > b ? a : b;
    }

    /*!
     * @brief    查找替换
     * @param    [i]find 查找的字符串
     * @param    [i]bCase 是否大写
     * @param    [i]bWord 整个单词
     * @param    [i]bNext 向下
     * @param    [i]replace 替换文本
     * @param    [i]findAndImReplace 找到是否马上替换
     * @return   替换成功返回true
     */
    bool replaceSelectText(const QString &find, bool bCase, bool bWord, bool bNext,
                           const QString &replace, bool findAndImReplace);
    bool replaceTextToImage(const QString &find, bool bCase, bool bWord, bool bNext,
                            const QString &image, bool findAndImReplace, int w, int h, bool bMonochrome);
    void replaceAllText(const QString &find, bool bCase, bool bWord, const QString &replace);

    void setTextCursorToHead()
    {
        setSel(0, 0);
    }
    void setTextCursorToTail()
    {
        setSel(-1, -1);
    }
    void selectAll()
    {
        setSel(0, -1);
    }

    QString selectText() const;
    QString allText() const;

    QFont selectionCharFont(bool bMousePress) const;
    QColor selectionCharFontColor() const;
    QColor selectionCharBackgroundColor() const;

    void insertTitleHead(const QString &title, int Num, GLDRichTextEdit::Align align);
    void insertTable(int rows, int columns);

    //得到控件中包含文本的行数 向控件发送消息EM_GETLINECOUNT可得到它包含的文本的总行数，但要注意当文本为空的时候，它还是返回1，换句话说，它返回的总行数永远不会小于1。所以要真正知道现在有多少行，必须做些小处理：
    int richEditLineCount() const;

    QString text(int row, int cell) const;
    void setText(int row, int cell, const QString &text);

    bool hasCell(int row) const;
    int cellCount(int row) const;

    bool seriesTable(int &tableRowStart, int &tableRowEnd, int rowIndex = 0) const;

    QString textOnMsftedit(int row, int cell) const;
    void setTextOnMsftedit(int row, int cell, const QString &text);
    bool hasCellOnMsftedit(int row) const;
    int cellCountOnMsftedit(int row) const;
    bool seriesTableOnMsftedit(int &tableRowStart, int &tableRowEnd, int rowIndex = 0) const;

    void editBegin();
    void editEnd();

    void sel(_Out_ CHARRANGE &cr) const
    {
        ::SendMessageW(m_hEdit, EM_EXGETSEL, 0, (LPARAM)&cr);
    }
    void setSel(long lStartChar, long lEndChar);
    int currentRow() const;
    void moveTextCursorNext();

    bool selectRegionText(const QString &beginText, const QString &endText, bool bCase);

    void test();

    static DWORD CALLBACK editStreamCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
    void setCursorFocus();

public:
    QWidget                 *m_parent;
    RichEditVer              m_richEditVer;
    HMODULE                  m_hnsEdit;
    HWND                     m_hEdit;
    LPRICHEDITOLE            m_lpRichEditOle;
    GLDRichTextEditOleCallback  *m_richEditOleCallback;
    ITextDocument           *m_itextDoc;

    QString                  m_fileName;

    GLDRichTextEdit         *m_p;

    QList<GLDImageObject *>  m_lstImgs;

    void loadRichEdit(QWidget *parent);

private:
    void init(QWidget *parent);

    void hideSelection(_In_ BOOL bHide, _In_ BOOL bPerm)
    {
        ::SendMessageW(m_hEdit, EM_HIDESELECTION, bHide, bPerm);
    }

    bool replaceSel(_In_z_ LPCWSTR lpszNewText, _In_ BOOL bCanUndo = FALSE)
    {
        return (FALSE == (BOOL)::SendMessageW(m_hEdit, EM_REPLACESEL, (WPARAM)bCanUndo,
                                              (LPARAM)lpszNewText) ? false : true);
    }

    // 取最末尾字符的位置
    int richEditTail() const;

    int lineLength(int nLine /* = -1 */) const
    {
        return (int)::SendMessageW(m_hEdit, EM_LINELENGTH, nLine, 0);
    }

    int lineIndex(int nLine /* = -1 */) const
    {
        return (int)::SendMessageW(m_hEdit, EM_LINEINDEX, nLine, 0);
    }

    int lineFromChar(int nIndex) const
    {
        return (int)::SendMessageW(m_hEdit, EM_LINEFROMCHAR, nIndex, 0);
    }

    int line(_In_ int nIndex, _Out_ LPCWSTR lpszBuffer) const
    {
        return (int)::SendMessageW(m_hEdit, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
    }

    int line(_In_ int nIndex, LPCWSTR lpszBuffer, _In_ int nMaxLength) const
    {
        *(LPWORD)lpszBuffer = (WORD)nMaxLength;
        return (int)::SendMessageW(m_hEdit, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
    }

    int line(_In_ int nIndex, LPCSTR lpszBuffer, _In_ int nMaxLength) const
    {
        *(LPWORD)lpszBuffer = (WORD)nMaxLength;
        return (int)::SendMessageA(m_hEdit, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
    }

    QStringList paser(const QString &text, bool &bHasCell) const;
    QStringList paser2(const QString &text, bool &bHasCell) const;

    int checkIsTable(const QString &text, bool checkCellCount) const;
    int checkIsTable2(const QString &text, bool checkCellCount) const;
    QString compose(const QStringList &list) const;

private:
    int m_zoomFactorA; // 缩放因子A(接口对小数貌似不支持, 因此需要设置此两个变量进行转换)
    int m_zoomFactorB; // 缩放因子B(接口对小数貌似不支持, 因此需要设置此两个变量进行转换)
};

GLDRichTextEditPrivate::GLDRichTextEditPrivate(GLDRichTextEdit *parent)
    : m_parent(parent)
    , m_richEditVer(Msftedit)
    , m_hnsEdit(NULL)
    , m_hEdit(NULL)
    , m_lpRichEditOle(NULL)
    , m_richEditOleCallback(NULL)
    , m_itextDoc(NULL)
    , m_fileName()
    , m_p(parent)
    , m_zoomFactorA(100)
    , m_zoomFactorB(100)
{
    Q_ASSERT(NULL != parent);

    init(parent);

    if (0 == s_lFilterNum.fetchAndAddAcquire(1))
    {
        if (NULL == s_enentFilter)
        {
            s_enentFilter = new GLDRichTextEditNativeEventFilter;
        }
    }

    Q_ASSERT(NULL != s_enentFilter);
    s_enentFilter->registerEdit(m_hEdit, parent);
}

GLDRichTextEditPrivate::~GLDRichTextEditPrivate()
{
    for (int i = 0; i < m_lstImgs.size(); ++i)
    {
        GLDImageObject *pObj = m_lstImgs.at(i);
        pObj->setRelease(true);
        pObj->Release();
    }

    if (NULL != m_lpRichEditOle)
    {
        m_lpRichEditOle->Release();
        m_lpRichEditOle = NULL;
    }

    if (NULL != m_richEditOleCallback)
    {
        if (0 != m_richEditOleCallback->Release())
        {
            GLD_RICH_ERROR_POS("no release");
        }

        m_richEditOleCallback = NULL;

        if (NULL != m_itextDoc)
        {
            m_itextDoc->Release();
            m_itextDoc = NULL;
        }
    }

    Q_ASSERT(NULL != s_enentFilter);
    s_enentFilter->removeEdit(m_hEdit);

    if (1 == s_lFilterNum.fetchAndAddAcquire(-1))
    {
        delete s_enentFilter;
        s_enentFilter = NULL;
    }
}

void GLDRichTextEditPrivate::loadRichEdit(QWidget *parent)
{
    do
    {
        HWND hwndParent = (HWND)parent->winId();

        int nWidth = parent->width();
        int nHeight = parent->height();

        DWORD dStyle =
            WS_VISIBLE
            | WS_CHILD
            | ES_MULTILINE
            | ES_WANTRETURN
            | ES_AUTOVSCROLL
            | ES_NOHIDESEL
            | ES_SAVESEL
            | WS_VSCROLL
            | ES_SELECTIONBAR
            ;

        m_hnsEdit = ::LoadLibraryW(L"Msftedit.dll");

        if (NULL != m_hnsEdit)
        {
            m_richEditVer = Msftedit;
            m_hEdit = ::CreateWindowExW(0, MSFTEDIT_CLASS, L"", dStyle,
                                        0, 0, nWidth, nHeight, hwndParent,
                                        NULL, m_hnsEdit, NULL);
            break;
        }

        m_hnsEdit = ::LoadLibraryW(L"Riched20.dll");

        if (NULL != m_hnsEdit)
        {
            m_richEditVer = Richedit20;
            m_hEdit = ::CreateWindowExW(0, RICHEDIT_CLASSW, L"", dStyle,
                                        0, 0, nWidth, nHeight, hwndParent,
                                        NULL, m_hnsEdit, NULL);
            break;
        }

        m_hnsEdit = ::LoadLibraryW(L"Riched32.dll");

        if (NULL != m_hnsEdit)
        {
            m_richEditVer = Richedit32;
            m_hEdit = ::CreateWindowExW(0, RICHEDIT_CLASSW, L"", dStyle,
                                        0, 0, nWidth, nHeight, hwndParent,
                                        NULL, m_hnsEdit, NULL);
            break;
        }
    } while (false);
}

int GLDRichTextEditPrivate::contentHeight()
{
    int nMin;
    int nMax;
    GetScrollRange(m_hEdit, SB_VERT, &nMin, &nMax);
    return nMax;
}

int GLDRichTextEditPrivate::contentWidth()
{
    RECT clientRect;
    GetClientRect(m_hEdit, &clientRect);
    return clientRect.right - clientRect.left;
}

void GLDRichTextEditPrivate::init(QWidget *parent)
{
    Q_ASSERT(NULL == m_hnsEdit);
    Q_ASSERT(NULL == m_hEdit);
    Q_ASSERT(NULL == m_lpRichEditOle);
    Q_ASSERT(NULL == m_richEditOleCallback);

    loadRichEdit(parent);

    Q_ASSERT(NULL != m_hEdit);

    //   已经使用 GLDRichTextEditNativeEventFilter 截取 m_hEdit 的消息 不需要向父窗口发送消息
    long lEventMask = (long)::SendMessage(m_hEdit, EM_GETEVENTMASK, 0, 0L) | (ENM_SCROLL);

    ::SendMessageW(m_hEdit, EM_SETEVENTMASK, 0, lEventMask);

    ::SendMessageW(m_hEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&m_lpRichEditOle);

    Q_ASSERT(NULL != m_lpRichEditOle);
    m_lpRichEditOle->QueryInterface(IID_ITextDocument, (void **)&m_itextDoc);

    Q_ASSERT(NULL != m_itextDoc);
    m_itextDoc->AddRef();

    if (NULL == m_richEditOleCallback)
    {
        m_richEditOleCallback = new GLDRichTextEditOleCallback(m_lpRichEditOle);

        if (FALSE == (BOOL)::SendMessageW(m_hEdit, EM_SETOLECALLBACK, 0, (LPARAM)m_richEditOleCallback))
        {
            GLD_RICH_ERROR_POS("set ole call back fail.");
        }
    }
}

void GLDRichTextEditPrivate::setSel(long lStartChar, long lEndChar)
{
    CHARRANGE cCharrange;
    cCharrange.cpMin = lStartChar;
    cCharrange.cpMax = lEndChar;
    ::SendMessageW(m_hEdit, EM_EXSETSEL, 0, (LPARAM)&cCharrange);
}

int GLDRichTextEditPrivate::richEditLineCount() const
{
    int result = ::SendMessageW(m_hEdit, EM_GETLINECOUNT, 0, 0);
    int nFirstCharPosOfLastLine = (int)::SendMessageW(m_hEdit, EM_LINEINDEX, result - 1, 0);

    if (0 != ::SendMessageW(m_hEdit, EM_LINELENGTH, nFirstCharPosOfLastLine, 0))
    {
        --result;
    }

    return result;
}

int GLDRichTextEditPrivate::richEditTail() const
{
    int nLines = richEditLineCount();

    int result = (int)::SendMessageW(m_hEdit, EM_LINEINDEX, nLines - 1, 0);

    result += (int)::SendMessageW(m_hEdit, EM_LINELENGTH, result, 0);

    return result;
}

void GLDRichTextEditPrivate::cut()
{
    ::SendMessageW(m_hEdit, WM_CUT, 0, 0);
}

void GLDRichTextEditPrivate::copy()
{
    ::SendMessageW(m_hEdit, WM_COPY, 0, 0);
}

void GLDRichTextEditPrivate::past()
{
    ::SendMessageW(m_hEdit, WM_PASTE, 0, 0);
}

void GLDRichTextEditPrivate::undo()
{
    (BOOL)::SendMessageW(m_hEdit, EM_UNDO, 0, 0);
}

void GLDRichTextEditPrivate::setUndoCount(int undoCount)
{
    ::SendMessage(m_hEdit, EM_SETUNDOLIMIT, undoCount, 0);
}

void GLDRichTextEditPrivate::redo()
{
    (BOOL)::SendMessageW(m_hEdit, EM_REDO, 0, 0);
}

bool GLDRichTextEditPrivate::canUndo() const
{
    if (FALSE == (BOOL)::SendMessageW(m_hEdit, EM_CANUNDO, 0, 0))
    {
        return false;
    }

    return true;
}

bool GLDRichTextEditPrivate::canRedo() const
{
    if (FALSE == (BOOL)::SendMessageW(m_hEdit, EM_CANREDO, 0, 0))
    {
        return false;
    }

    return true;
}

void GLDRichTextEditPrivate::insertText(const QString &text)
{
    replaceSel(reinterpret_cast<LPCWSTR>(text.data()), true);
}

BOOL GLDRichTextEditPrivate::insertImage(const QString &image, int w, int h, bool bMonochrome)
{
    if (image.isEmpty())
    {
        return FALSE;
    }

    if (NULL == m_lpRichEditOle)
    {
        return FALSE;
    }

    HBITMAP hBitmap = loadImageFromFile(image, bMonochrome);

    if (NULL != hBitmap)
    {
        GLDImageObject *pObj = GLDImageObject::insertBitmap(m_lpRichEditOle, hBitmap, 0, w, h);

        if (NULL != pObj)
        {
            m_lstImgs.append(pObj);
        }

        DeleteObject(hBitmap);
        return TRUE;
    }

    return FALSE;
}

void GLDRichTextEditPrivate::setDefaultFont(const QFont &fnt, int size, const QColor &clr)
{
    QString strDefFont = fnt.family();

    CHARFORMAT2 cf;

    cf.cbSize = sizeof(CHARFORMAT2);
    cf.dwMask = CFM_BOLD |
                CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_SIZE | CFM_WEIGHT |
                CFM_COLOR | CFM_OFFSET | CFM_PROTECTED;
    cf.dwEffects = NULL;// CFE_AUTOCOLOR;//自动颜色
    cf.yHeight = size * 20; // 240; //12pt
    cf.yOffset = 0;
    cf.crTextColor = RGB(clr.red(), clr.green(), clr.blue()); //字体颜色
    cf.bCharSet = 0;
    cf.bPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

    Q_ASSERT(strDefFont.length() < LF_FACESIZE);

    lstrcpyn(cf.szFaceName, (LPCWSTR)strDefFont.utf16(), LF_FACESIZE);

    cf.dwMask |= CFM_FACE;

    ::SendMessage(m_hEdit, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
}

void GLDRichTextEditPrivate::setSelectionCharFont(const QFont &font)
{
    CHARFORMAT2W cf;
    cf.cbSize = sizeof(CHARFORMAT2W);
    cf.szFaceName[0] = NULL;
    cf.dwMask = CFM_FACE | CFM_CHARSET;
    std::wstring wstrFamily = font.family().toStdWString();

    lstrcpyn(cf.szFaceName, wstrFamily.c_str(), LF_FACESIZE);

    cf.bPitchAndFamily = font.fixedPitch() ? FIXED_PITCH : DEFAULT_PITCH;
    cf.bCharSet = DEFAULT_CHARSET;
    ::SendMessageW(m_hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

void GLDRichTextEditPrivate::setSelectionCharBackgroundColor(const QColor &color)
{
    if (!color.isValid())
    {
        return;
    }

    CHARFORMAT2W cf;
    cf.cbSize = sizeof(CHARFORMAT2W);
    ::SendMessageW(m_hEdit, EM_GETCHARFORMAT, 1, (LPARAM)&cf);

    cf.dwMask = CFM_BACKCOLOR;
    cf.dwEffects = NULL;
    cf.crBackColor = RGB(color.red(), color.green(), color.blue());
    ::SendMessageW(m_hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

void GLDRichTextEditPrivate::setSelectionCharSize(int size)
{
    if (0 > size || 32760 < size)
    {
        return;
    }
    size *= 20;

    CHARFORMAT2W cFormat2;
    cFormat2.cbSize = sizeof(CHARFORMAT2W);
    cFormat2.dwMask |= CFM_SIZE;
    cFormat2.yHeight = size;
    ::SendMessageW(m_hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cFormat2);
}

void GLDRichTextEditPrivate::setSelectionCharColor(const QColor &clr)
{
    if (!clr.isValid())
    {
        return;
    }

    CHARFORMAT2W cf;
    cf.cbSize = sizeof(CHARFORMAT2W);
    ::SendMessageW(m_hEdit, EM_GETCHARFORMAT, 1, (LPARAM)&cf);

    cf.dwMask = CFM_COLOR;
    cf.dwEffects = NULL;
    cf.crTextColor = RGB(clr.red(), clr.green(), clr.blue());
    ::SendMessageW(m_hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

void GLDRichTextEditPrivate::setSelectionCharFormat(DWORD dwMask, DWORD dwEffects)
{
    CHARFORMAT2W cf;
    cf.cbSize = sizeof(CHARFORMAT2W);
    ::SendMessageW(m_hEdit, EM_GETCHARFORMAT, 1, (LPARAM)&cf);

    if (0 != (dwMask & cf.dwMask))
    {
        cf.dwEffects ^= dwEffects;
    }
    else
    {
        cf.dwEffects |= dwEffects;
    }

    cf.dwMask = dwMask;

    ::SendMessageW(m_hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

void GLDRichTextEditPrivate::setSelectionCharFormat(DWORD dwMask, DWORD dwEffects, bool enable)
{
    CHARFORMAT2W cf;
    cf.cbSize = sizeof(CHARFORMAT2W);
    ::SendMessageW(m_hEdit, EM_GETCHARFORMAT, 1, (LPARAM)&cf);

    if (enable)
    {
        cf.dwEffects |= dwEffects;
    }
    else
    {
        if (0 != (cf.dwEffects & dwEffects))
        {
            cf.dwEffects ^= dwEffects;
        }
    }

    cf.dwMask = dwMask;

    ::SendMessageW(m_hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

void GLDRichTextEditPrivate::setParaFormat(WORD align)
{
    PARAFORMAT2 pf;
    pf.cbSize = sizeof(PARAFORMAT2);
    pf.dwMask = PFM_ALIGNMENT;
    pf.wAlignment = align;

    ::SendMessageW(m_hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
}

void GLDRichTextEditPrivate::setReadOnly(bool value)
{
    ::SendMessage(m_hEdit, EM_SETREADONLY, (WPARAM)(BOOL)value, 0);
}

void GLDRichTextEditPrivate::setZoomFactor(const double factor)
{
    // 富文本只支持10% ~ 63之间的缩放
    if (factor < 0.1 || factor > 63.0)
    {
        return;
    }

    // 主要用于解决浮点精度丢失问题
    if (factor >= 1)
    {
        m_zoomFactorA = (int)(factor * 100);
        m_zoomFactorB = 100;
    }
    else if (factor < 1)
    {
        m_zoomFactorA = factor * 100;
        m_zoomFactorB = 100;
    }

    ::SendMessageW(m_hEdit, EM_SETZOOM, m_zoomFactorA, m_zoomFactorB);
}

bool GLDRichTextEditPrivate::findText(const QString &find, bool bCase, bool bWord, bool bNext)
{
    if (find.isEmpty())
    {
        return false;
    }

    FINDTEXTEXW ft;
    ::SendMessageW(m_hEdit, EM_EXGETSEL, 0, (LPARAM)&ft.chrg);

    ft.lpstrText = reinterpret_cast<LPCWSTR>(find.data()); // QString 采用unicode 内存模型 short

    DWORD dwFlags = bCase ? FR_MATCHCASE : 0;
    dwFlags |= bWord ? FR_WHOLEWORD : 0;

    if (ft.chrg.cpMin != ft.chrg.cpMax)  // i.e. there is a selection
    {
        if (bNext)
        {
            // won't wraparound backwards
            long length = (long)::SendMessageW(m_hEdit, WM_GETTEXTLENGTH, NULL, NULL);
            ft.chrg.cpMin = minInGRE(ft.chrg.cpMin + 1, length);
        }
        else
        {
            // won't wraparound backwards
            ft.chrg.cpMin = maxInGRE(ft.chrg.cpMin - 1, 0);
        }
    }

    if (bNext)
    {
        ft.chrg.cpMax = (long)::SendMessageW(m_hEdit, WM_GETTEXTLENGTH, NULL, NULL);
        dwFlags |= FR_DOWN;
        Q_ASSERT(ft.chrg.cpMax >= ft.chrg.cpMin);
    }
    else
    {
        ft.chrg.cpMax = 0;
        dwFlags &= ~FR_DOWN;
        Q_ASSERT(ft.chrg.cpMax <= ft.chrg.cpMin);
    }

    long lIndex = (long)::SendMessageW(m_hEdit, EM_FINDTEXTEX, dwFlags, (LPARAM)&ft);

    if (-1 != lIndex)
    {
        ::SendMessageW(m_hEdit, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
        return true;
    }

    return false;
}

bool GLDRichTextEditPrivate::replaceSelectText(const QString &find, bool bCase, bool bWord,
                                               bool bNext, const QString &replace, bool findAndImReplace)
{
    if (find.isEmpty())
    {
        return false;
    }

    Qt::CaseSensitivity eCase = Qt::CaseInsensitive;

    if (bCase)
    {
        eCase = Qt::CaseSensitive;
    }

    if (0 != find.compare(selectText(), eCase))
    {
        if (findText(find, bCase, bWord, bNext))
        {
            if (findAndImReplace)
            {
                return replaceSel(reinterpret_cast<LPCWSTR>(replace.data()));
            }
        }

        // print not find text
        return false;
    }

    bool bRet = false;
    bRet = replaceSel(reinterpret_cast<LPCWSTR>(replace.data()));

    if (!findAndImReplace)
    {
        if (!findText(find, bCase, bWord, bNext))
        {
            // print not find text
        }
    }

    return bRet;
}

bool GLDRichTextEditPrivate::replaceTextToImage(const QString &find, bool bCase, bool bWord,
                                                bool bNext, const QString &image,
                                                bool findAndImReplace, int w, int h, bool bMonochrome)
{
    if (find.isEmpty())
    {
        return false;
    }

    Qt::CaseSensitivity eCase = Qt::CaseInsensitive;

    if (bCase)
    {
        eCase = Qt::CaseSensitive;
    }

    if (0 != find.compare(selectText(), eCase))
    {
        if (findText(find, bCase, bWord, bNext))
        {
            if (findAndImReplace)
            {
                return ((FALSE == insertImage(image, w, h, bMonochrome)) ? false : true);
            }
        }

        // print not find text
        return false;
    }

    // replaceSel(reinterpret_cast<LPCWSTR>(replace.data()));
    bool bRet = (FALSE == insertImage(image, w, h, bMonochrome)) ? false : true;

    if (!findAndImReplace)
    {
        if (!findText(find, bCase, bWord, bNext))
        {
            // print not find text
        }
    }

    return bRet;
}

void GLDRichTextEditPrivate::replaceAllText(const QString &find, bool bCase, bool bWord, const QString &replace)
{
    if (find.isEmpty())
    {
        return;
    }

    FINDTEXTEXW ft;
    sel(ft.chrg);
    long lInitialCursorPosition = ft.chrg.cpMin;

    hideSelection(TRUE, FALSE);
    setSel(0, 0);

    if (findText(find, bCase, bWord, true))
    {
        do
        {
            sel(ft.chrg);
            replaceSel(reinterpret_cast<LPCWSTR>(replace.data()));

            if (ft.chrg.cpMin < lInitialCursorPosition)
            {
                lInitialCursorPosition += replace.length() - find.length();
            }
        } while (findText(find, bCase, bWord, true));
    }

    setSel(lInitialCursorPosition, lInitialCursorPosition);
    hideSelection(FALSE, FALSE);
}

QString GLDRichTextEditPrivate::selectText() const
{
    CHARRANGE cr;
    cr.cpMin = 0;
    cr.cpMax = 0;
    ::SendMessageW(m_hEdit, EM_EXGETSEL, 0, (LPARAM)&cr);

    QString text(cr.cpMax - cr.cpMin, '\0');
    ::SendMessageW(m_hEdit, EM_GETSELTEXT, 0, (LPARAM)(text.data())); // QString 采用unicode 内存模型 short

    return text;
}

QString GLDRichTextEditPrivate::allText() const
{
    QString text;

    CHARRANGE cr;
    ::SendMessageW(m_hEdit, EM_EXGETSEL, 0, (LPARAM)&cr);

    CHARRANGE crTemp;
    crTemp.cpMin = 0;
    crTemp.cpMax = -1;
    ::SendMessageW(m_hEdit, EM_EXSETSEL, 0, (LPARAM)&crTemp);

    crTemp.cpMin = 0;
    crTemp.cpMax = 0;
    ::SendMessageW(m_hEdit, EM_EXGETSEL, 0, (LPARAM)&crTemp);

    text = QString(crTemp.cpMax - crTemp.cpMin, '\0');
    ::SendMessageW(m_hEdit, EM_GETSELTEXT, 0, (LPARAM)(text.data())); // QString 采用unicode 内存模型 short

    ::SendMessageW(m_hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);

    return text;
}

QFont GLDRichTextEditPrivate::selectionCharFont(bool bMousePress) const
{
    CHARRANGE cr;
    sel(cr);
    ITextRange *ipTextRange = NULL;

    if (bMousePress)
    {
        QPoint cusorPos = QCursor::pos();
        m_itextDoc->RangeFromPoint(cusorPos.x(), cusorPos.y(), &ipTextRange);
    }
    else
    {
        m_itextDoc->Range(cr.cpMin, cr.cpMax, &ipTextRange);
    }

    if (NULL == ipTextRange)
    {
        return QFont();
    }

    ITextFont *ipTextFont = NULL;
    ipTextRange->GetFont(&ipTextFont);

    if (NULL == ipTextFont)
    {
        return QFont();
    }

    QFont fnt;
    {
        BSTR bstrFontName;
        ipTextFont->GetName(&bstrFontName);
        QString sFontName = QString::fromStdWString(bstrFontName);
        fnt = QFont(sFontName);
    }

    float fsize = 12;
    ipTextFont->GetSize(&fsize);
    fnt.setPointSize(fsize);

    return fnt;
}

QColor GLDRichTextEditPrivate::selectionCharFontColor() const
{
    CHARFORMAT2W cf;
    cf.cbSize = sizeof(CHARFORMAT2W);
    ::SendMessageW(m_hEdit, EM_GETCHARFORMAT, 1, (LPARAM)&cf);

    BYTE red = cf.crTextColor;
    BYTE green = cf.crTextColor >> 8;
    BYTE blue = cf.crTextColor >> 16;

    return QColor(red, green, blue);
}

QColor GLDRichTextEditPrivate::selectionCharBackgroundColor() const
{
    CHARFORMAT2W cf;
    cf.cbSize = sizeof(CHARFORMAT2W);
    ::SendMessageW(m_hEdit, EM_GETCHARFORMAT, 1, (LPARAM)&cf);

    BYTE red = cf.crBackColor;
    BYTE green = cf.crBackColor >> 8;
    BYTE blue = cf.crBackColor >> 16;

    return QColor(red, green, blue);
}

void GLDRichTextEditPrivate::insertTable(int rows, int columns)
{
    if (1 > rows || 1 > columns)
    {
        return;
    }

    std::string table;
    {
        QString sTable;
        QString sTableHead = "{\\rtf1";
        QString sRowHead = "\\trowd\\trgaph108\\trleft-108\\trbrdrl\\brdrs\\brdrw10 \
                          \\trbrdrt\\brdrs\\brdrw10 \\trbrdrr\\brdrs\\brdrw10 \\trbrdrb \
                          \\brdrs\\brdrw10\\trpaddl108\\trpaddr108\\trpaddfl3\\trpaddfr3";

        QString sColumnHead = " \\clbrdrl\\brdrw10\\brdrs\\clbrdrt\\brdrw10\\brdrs\
                \\clbrdrr\\brdrw10\\brdrs\\clbrdrb\\brdrw10\\brdrs ";

        QString sCellX = "\\cellx%1\\clbrdrl\\brdrw10\\brdrs\\clbrdrt\\brdrw10\\brdrs\
                \\clbrdrr\\brdrw10\\brdrs\\clbrdrb\\brdrw10\\brdrs ";

        QString sColumnTail = "\\pard\\intbl\\nowidctlpar\\qj";
        QString sCellTail = "\\cell";
        QString sRowTail = "\\row";
        QString sTableTail = "}";
        int nWidth = 8748 / columns;

        int nRow = 0;
        int nColumn = 0;
        sTable += sTableHead;
        QString sRow;
        QString sCellTails;

        for (nRow = 0; nRow < rows; ++nRow)
        {
            nColumn = 0;
            sRow.clear();
            sCellTails.clear();

            sRow = sRowHead + sColumnHead;

            for (nColumn = 0; nColumn < columns; ++nColumn)
            {
                if (nColumn == columns - 1)
                {
                    sRow += QString("\\cellx%1").arg(nWidth * (nColumn + 1));
                }
                else
                {
                    sRow += sCellX.arg(nWidth * (nColumn + 1));
                }

                sCellTails.append(sCellTail);
            }

            sRow += sColumnTail;
            sRow += sCellTails;
            sRow += sRowTail;
            sTable += sRow;
        }

        sTable += sTableTail;
        table = sTable.toStdString();
    }

    SETTEXTEX st;
    st.codepage = 1200;
    st.flags = ST_SELECTION | ST_KEEPUNDO;
    ::SendMessageW(m_hEdit, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)table.data());
}

void GLDRichTextEditPrivate::del()
{
    CHARRANGE range;
    sel(range);

    ITextRange *ipRange = NULL;
    HRESULT hr = m_itextDoc->Range(range.cpMin, range.cpMax, &ipRange);

    if (NULL != ipRange)
    {
        hr = ipRange->Delete(tomText, 1, NULL);
    }
}

QString GLDRichTextEditPrivate::text(int row, int cell) const
{
    QString rStr;

    do
    {
        --row;
        --cell;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > row || row > nRowCount)
        {
            break;
        }

        if (NULL == m_itextDoc)
        {
            break;
        }

        int nStart = lineIndex(row);
        int nEnd = lineIndex(row + 1) - 1;

        if (0 > nEnd)
        {
            nEnd = richEditTail();
        }

        if (nStart >= nEnd)
        {
            break;
        }

        ITextRange *ipRange = NULL;

        if (NULL == ipRange)
        {
            break;
        }

        BSTR bstr;
        ipRange->GetText(&bstr);
        // 解析
        bool bHasCell = false;
        QStringList lst = paser(QString::fromStdWString(bstr), bHasCell);

        if (1 == lst.size())
        {
            rStr = QString::fromStdWString(bstr);

            if (rStr.endsWith('\t'))
            {
                rStr.chop(1);
            }

        }
        else
        {
            if (0 <= cell && cell < lst.size())
            {
                rStr = lst.at(cell);
            }
        }

    } while (false);

    return rStr;
}

QString GLDRichTextEditPrivate::textOnMsftedit(int row, int cell) const
{
    QString rStr;

    do
    {
        --row;
        --cell;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > row || row > nRowCount)
        {
            break;
        }

        if (NULL == m_itextDoc)
        {
            break;
        }

        int nStart = lineIndex(row);
        int nEnd = lineIndex(row + 1) - 1;

        if (0 > nEnd)
        {
            nEnd = richEditTail();
        }

        if (nStart >= nEnd)
        {
            break;
        }

        ITextRange *ipRange = NULL;

        if (NULL == ipRange)
        {
            break;
        }

        BSTR bstr;
        ipRange->GetText(&bstr);
        // 解析
        bool bHasCell = false;
        QStringList lst = paser2(QString::fromStdWString(bstr), bHasCell);

        if (1 == lst.size())
        {
            rStr = QString::fromStdWString(bstr);

            if (bHasCell)
            {
                rStr.remove(0, 2);
                rStr.chop(1);
            }
        }
        else
        {
            Q_ASSERT(bHasCell); //断言不是表格
            lst.takeFirst(); // begin
            lst.takeLast();  // end

            if (0 <= cell && cell < lst.size())
            {
                rStr = lst.at(cell);
            }
        }

    } while (false);

    return rStr;
}

void GLDRichTextEditPrivate::setText(int row, int cell, const QString &text)
{
    do
    {
        --row;
        --cell;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > row || row > nRowCount)
        {
            break;
        }

        if (NULL == m_itextDoc)
        {
            break;
        }

        int nStart = lineIndex(row);
        int nEnd = lineIndex(row + 1) - 1;

        if (0 > nEnd)
        {
            nEnd = richEditTail();
        }

        if (nStart >= nEnd)
        {
            break;
        }

        ITextRange *ipRange = NULL;

        if (NULL == ipRange)
        {
            break;
        }

        QStringList lst;
        bool bHasCell = false;
        {
            BSTR bstr;
            ipRange->GetText(&bstr);
            // 解析
            lst = paser(QString::fromStdWString(bstr), bHasCell);
        }

        if (1 == lst.size())
        {
            // 没有单元格 或仅一个单元格子
            BSTR bstr = ::SysAllocString((OLECHAR *)text.utf16());

            if (bHasCell)
            {
                long lEnd = 0;
                ipRange->GetEnd(&lEnd);
                --lEnd;
                ipRange->SetEnd(lEnd);

                if (0 == cell)
                {
                    ipRange->SetText(bstr);
                }
            }
            else
            {
                ipRange->SetText(bstr);
            }

            ::SysFreeString(bstr);
        }
        else
        {
            if (0 <= cell && cell < lst.size())
            {
                long lReStart = nStart;

                for (int i = 0; i < cell; ++i)
                {
                    lReStart += lst.at(i).size() + 1; // '\t'
                }

                ipRange->SetStart(lReStart);

                if (cell != lst.size() - 1)
                {
                    long lReEnd = lReStart;
                    lReEnd = lReStart + lst.at(cell).size();
                    ipRange->SetEnd(lReEnd);
                }

                BSTR bstr = ::SysAllocString((OLECHAR *)text.utf16());
                ipRange->SetText(bstr);

                ::SysFreeString(bstr);
            }
        }
    } while (false);
}

void GLDRichTextEditPrivate::setTextOnMsftedit(int row, int cell, const QString &text)
{
    do
    {
        --row;
        --cell;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > row || row > nRowCount)
        {
            break;
        }

        if (NULL == m_itextDoc)
        {
            break;
        }

        int nStart = lineIndex(row);
        int nEnd = lineIndex(row + 1) - 1;

        if (0 > nEnd)
        {
            nEnd = richEditTail();
        }

        if (nStart >= nEnd)
        {
            break;
        }

        ITextRange *ipRange = NULL;

        if (NULL == ipRange)
        {
            break;
        }

        QStringList lst;
        bool bHasCell = false;
        {
            BSTR bstr;
            ipRange->GetText(&bstr);
            // 解析
            lst = paser2(QString::fromStdWString(bstr), bHasCell);
        }

        if (bHasCell)
        {
            lst.takeFirst();
            lst.takeLast();
        }

        if (1 == lst.size())
        {
            // 没有单元格 或仅一个单元格子
            BSTR bstr = ::SysAllocString((OLECHAR *)text.utf16());

            if (bHasCell)
            {
                if (0 == cell)
                {
                    long lCbegin = 0;
                    ipRange->GetStart(&lCbegin);
                    ++lCbegin;
                    ++lCbegin;
                    ipRange->SetStart(lCbegin);

                    long lCend = 0;
                    ipRange->GetEnd(&lCend);
                    --lCend;
                    --lCend;
                    ipRange->SetEnd(lCend);
                    ipRange->SetText(bstr);
                }
            }
            else
            {
                ipRange->SetText(bstr);
            }

            ::SysFreeString(bstr);
        }
        else
        {
            if (0 <= cell && cell < lst.size())
            {
                long lCbegin = 0;
                ipRange->GetStart(&lCbegin);
                ++lCbegin;
                ++lCbegin;

                long lReStart = lCbegin;

                for (int i = 0; i < cell; ++i)
                {
                    lReStart += lst.at(i).size() + 1; // '\a'
                }

                ipRange->SetStart(lReStart);

                if (cell != lst.size() - 1)
                {
                    long lReEnd = lReStart;
                    lReEnd = lReStart + lst.at(cell).size();
                    ipRange->SetEnd(lReEnd);
                }
                else if (cell == lst.size() - 1)
                {
                    long lCend = 0;
                    ipRange->GetEnd(&lCend);
                    --lCend;
                    --lCend;
                    ipRange->SetEnd(lCend);
                }

                BSTR bstr = ::SysAllocString((OLECHAR *)text.utf16());
                ipRange->SetText(bstr);

                ::SysFreeString(bstr);
            }
        }
    } while (false);
}

bool GLDRichTextEditPrivate::hasCell(int row) const
{
    bool bRet = false;

    do
    {
        --row;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > row || row > nRowCount)
        {
            break;
        }

        if (NULL == m_itextDoc)
        {
            break;
        }

        int nStart = lineIndex(row);
        int nEnd = lineIndex(row + 1) - 1;

        if (nStart >= nEnd)
        {
            break;
        }

        ITextRange *ipRange = NULL;

        if (NULL == ipRange)
        {
            break;
        }

        BSTR bstr;
        ipRange->GetText(&bstr);

        // 解析
        if (0 < checkIsTable(QString::fromStdWString(bstr), false))
        {
            bRet = true;
        }

    } while (false);

    return bRet;
}

bool GLDRichTextEditPrivate::hasCellOnMsftedit(int row) const
{
    bool bRet = false;

    do
    {
        --row;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > row || row > nRowCount)
        {
            break;
        }

        if (NULL == m_itextDoc)
        {
            break;
        }

        int nStart = lineIndex(row);
        int nEnd = lineIndex(row + 1) - 1;

        if (nStart >= nEnd)
        {
            break;
        }

        ITextRange *ipRange = NULL;

        if (NULL == ipRange)
        {
            break;
        }

        BSTR bstr;
        ipRange->GetText(&bstr);

        // 解析
        if (0 < checkIsTable2(QString::fromStdWString(bstr), false))
        {
            bRet = true;
        }

    } while (false);

    return bRet;
}

int GLDRichTextEditPrivate::cellCount(int row) const
{
    int nRet = 0;

    do
    {
        --row;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > row || row > nRowCount)
        {
            break;
        }

        if (NULL == m_itextDoc)
        {
            break;
        }

        int nStart = lineIndex(row);
        int nEnd = lineIndex(row + 1) - 1;

        if (nStart >= nEnd)
        {
            break;
        }

        ITextRange *ipRange = NULL;

        if (NULL == ipRange)
        {
            break;
        }

        BSTR bstr;
        ipRange->GetText(&bstr);
        nRet = checkIsTable(QString::fromStdWString(bstr), true);

    } while (false);

    return nRet;
}

int GLDRichTextEditPrivate::cellCountOnMsftedit(int row) const
{
    int nRet = 0;

    do
    {
        --row;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > row || row > nRowCount)
        {
            break;
        }

        if (NULL == m_itextDoc)
        {
            break;
        }

        int nStart = lineIndex(row);
        int nEnd = lineIndex(row + 1) - 1;

        if (nStart >= nEnd)
        {
            break;
        }

        ITextRange *ipRange = NULL;

        if (NULL == ipRange)
        {
            break;
        }

        BSTR bstr;
        ipRange->GetText(&bstr);
        nRet = checkIsTable2(QString::fromStdWString(bstr), true);

    } while (false);

    return nRet;
}

bool GLDRichTextEditPrivate::seriesTable(int &tableRowStart, int &tableRowEnd, int rowIndex) const
{
    bool bRet = false;

    do
    {
        if (NULL == m_itextDoc)
        {
            break;
        }

        if (1 > rowIndex)
        {
            rowIndex = 1;
        }

        --rowIndex;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > rowIndex || rowIndex > nRowCount)
        {
            break;
        }

        bool bFirstFindTableRow = false;

        for (int i = rowIndex; i < nRowCount; ++i)
        {
            int nStart = lineIndex(i);
            int nEnd = lineIndex(i + 1) - 1;

            if (0 > nEnd)
            {
                nEnd = richEditTail();
            }

            if (nStart == nEnd)
            {
                if (i > nRowCount)
                {
                    tableRowEnd = i + 1;
                    break;
                }

                continue;
            }
            else if (nStart > nEnd)
            {
                tableRowEnd = i + 1;
                break;
            }

            ITextRange *ipRange = NULL;

            if (NULL == ipRange)
            {
                if (bFirstFindTableRow)
                {
                    tableRowEnd = i + 1;
                }

                break;
            }

            BSTR bstr;
            ipRange->GetText(&bstr);

            // 解析
            if (0 < checkIsTable(QString::fromStdWString(bstr), false))
            {
                if (!bFirstFindTableRow)
                {
                    bRet = true;
                    tableRowStart = i + 1;
                    bFirstFindTableRow = true;
                }

                if ((i + 1) == nRowCount)
                {
                    tableRowEnd = i + 2;
                }
            }
            else
            {
                if (bFirstFindTableRow)
                {
                    tableRowEnd = i + 1;
                    break;
                }
            }
        }
    } while (false);

    return bRet;
}

bool GLDRichTextEditPrivate::seriesTableOnMsftedit(int &tableRowStart, int &tableRowEnd, int rowIndex) const
{
    bool bRet = false;

    do
    {
        if (NULL == m_itextDoc)
        {
            break;
        }

        if (1 > rowIndex)
        {
            rowIndex = 1;
        }

        --rowIndex;
        int nRowCount = richEditLineCount();

        if (0 == nRowCount)
        {
            break;
        }

        if (0 > rowIndex || rowIndex > nRowCount)
        {
            break;
        }

        bool bFirstFindTableRow = false;

        for (int i = rowIndex; i < nRowCount; ++i)
        {
            int nStart = lineIndex(i);
            int nEnd = lineIndex(i + 1) - 1;

            if (0 > nEnd)
            {
                nEnd = richEditTail();
            }

            if (nStart == nEnd)
            {
                if (i > nRowCount)
                {
                    tableRowEnd = i + 1;
                    break;
                }

                continue;
            }
            else if (nStart > nEnd)
            {
                tableRowEnd = i + 1;
                break;
            }

            ITextRange *ipRange = NULL;

            if (NULL == ipRange)
            {
                if (bFirstFindTableRow)
                {
                    tableRowEnd = i + 1;
                }

                break;
            }

            BSTR bstr;
            ipRange->GetText(&bstr);

            // 解析
            if (0 < checkIsTable2(QString::fromStdWString(bstr), false))
            {
                if (!bFirstFindTableRow)
                {
                    bRet = true;
                    tableRowStart = i + 1;
                    bFirstFindTableRow = true;
                }

                if ((i + 1) == nRowCount)
                {
                    tableRowEnd = i + 2;
                }
            }
            else
            {
                if (bFirstFindTableRow)
                {
                    tableRowEnd = i + 1;
                    break;
                }
            }
        }
    } while (false);

    return bRet;
}

QStringList GLDRichTextEditPrivate::paser(const QString &text, bool &bHasCell) const
{
    QStringList lst;

    QString temp;

    for (int i = 0; i < text.size(); ++i)
    {
        if ('\t' == text.at(i))
        {
            lst.append(temp);
            temp.clear();
            bHasCell = true;
        }
        else
        {
            temp.append(text.at(i));
        }
    }

    // 没有单元格
    if (!temp.isEmpty())
    {
        lst.append(temp);
    }

    return lst;
}

QStringList GLDRichTextEditPrivate::paser2(const QString &text, bool &bHasCell) const
{
    QStringList lst;

    // 表格的样式 65529 \r    65531
    if (3 > text.size())
    {
        lst.append(text);
        return lst;
    }

    QChar beginC(65529);
    QChar beginC2('\r');
    QChar endC(65531);

    if (text.at(0) != beginC || text.at(1) != beginC2 || text.at(text.length() - 1) != endC)
    {
        lst.append(text);
        return lst;
    }

    QString temp;

    temp.append(beginC);
    temp.append(beginC2);
    lst.append(temp);
    temp.clear();

    for (int i = 2; i < text.size(); ++i)
    {
        if ('\a' == text.at(i))
        {
            lst.append(temp);
            temp.clear();
            bHasCell = true;
        }
        else if (endC == text.at(i))
        {
            temp.clear();
            lst.append(endC);
            break;
        }
        else
        {
            temp.append(text.at(i));
        }
    }

    // 没有单元格
    if (!temp.isEmpty())
    {
        lst.append(temp);
    }

    return lst;
}

int GLDRichTextEditPrivate::checkIsTable2(const QString &text, bool checkCellCount) const
{
    int nRet = 0;
    QChar beginC(65529);
    QChar beginC2('\r');
    QChar endC(65531);

    if (text.at(0) != beginC || text.at(1) != beginC2 || text.at(text.length() - 1) != endC)
    {
        return nRet;
    }

    if (checkCellCount)
    {
        bool bFindT = false;

        for (int i = 2; i < text.size(); ++i)
        {
            if ('\a' == text.at(i))
            {
                ++nRet;
                bFindT = true;
            }
            else if (endC == text.at(i))
            {
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < text.size(); ++i)
        {
            if ('\a' == text.at(i))
            {
                nRet = 1;
                break;
            }
        }
    }

    return nRet;
}

int GLDRichTextEditPrivate::checkIsTable(const QString &text, bool checkCellCount) const
{
    int nRet = 0;

    if (checkCellCount)
    {
        bool bFindT = false;

        for (int i = 0; i < text.size(); ++i)
        {
            if ('\t' == text.at(i))
            {
                ++nRet;
                bFindT = true;
            }
        }

        if (bFindT)
        {
            if ('\t' != text.at(text.size() - 1))
            {
                ++nRet;
            }
        }
    }
    else
    {
        for (int i = 0; i < text.size(); ++i)
        {
            if ('\t' == text.at(i))
            {
                nRet = 1;
                break;
            }
        }
    }

    return nRet;
}

void GLDRichTextEditPrivate::editBegin()
{
    m_itextDoc->BeginEditCollection();
}

void GLDRichTextEditPrivate::editEnd()
{
    m_itextDoc->EndEditCollection();
}

void GLDRichTextEditPrivate::insertTitleHead(const QString &title, int Num, GLDRichTextEdit::Align align)
{
    if (title.isEmpty() || 1 > Num || 9 < Num)
    {
        return;
    }

    std::string sWw;
    {
        QByteArray by64;
        {
            char cArr[10];
            cArr[0] = char(0xb7);
            cArr[1] = char(0xef);
            cArr[2] = char(0xbb);
            cArr[3] = char(0xcb);
            cArr[4] = '\0';
        }

        QString strHTitle;

        QString strQ;

        switch (align)
        {
            case GLDRichTextEdit::Left:
                strQ = "ql";
                break;

            case GLDRichTextEdit::Center:
                strQ = "qc";
                break;

            case GLDRichTextEdit::Right:
                strQ = "qr";
                break;

            case GLDRichTextEdit::Justify:
                strQ = "qj";
                break;

            default:
                break;
        }

        QString sHead = "{\\rtf1";
        QString sh =
            "{\\stylesheet{ Normal;}{\\s1 heading 1;}{\\s2 heading 2;}{\\s3 heading 3;}\
        {\\s4 heading 4;}{\\s5 heading 5;}{\\s6 heading 6;}{\\s7 heading 7;}{\\s8 heading 8;}{\\s9 heading 9;}}"
            ;
        QString sTail = "}";

        strHTitle += sHead;
        strHTitle += sh;

        switch (Num)
        {
            case 1:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s1\\sb340\\sa330\
                                     \\sl576\\slmult1\\%1\\kerning44\\b\\fs44 ").arg(strQ);
                break;

            case 2:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s2\\sb260\\sa260\
                                     \\sl408\\slmult1\\%1\\kerning2\\f1\\fs32 ").arg(strQ);
                break;

            case 3:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s3\\sb260\\sa260\
                                     \\sl408\\slmult1\\%1\\kerning2\\f0\\fs32 ").arg(strQ);
                break;

            case 4:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s4\\sb280\\sa290\
                                     \\sl372\\slmult1\\%1\\f1\\fs28 ").arg(strQ);
                break;

            case 5:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s5\\sb280\\sa290\
                                     \\sl372\\slmult1\\%1\\f0\\fs28 ").arg(strQ);
                break;

            case 6:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s6\\sb240\\sa64\
                                     \\sl312\\slmult1\\%1\\f1\\fs24 ").arg(strQ);
                break;

            case 7:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s7\\sb240\\sa64\
                                     \\sl312\\slmult1\\%1\\f0\\fs24 ").arg(strQ);
                break;

            case 8:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s8\\sb240\\sa64\
                                     \\sl312\\slmult1\\%1\\b0\\f1 ").arg(strQ);
                break;

            case 9:
                strHTitle += QString("\\pard\\keep\\keepn\\nowidctlpar\\s9\\sb240\\sa64\
                                     \\sl312\\slmult1\\%1\\fs21 ").arg(strQ);
                break;

            default:
                break;
        }

        strHTitle += "d";
        strHTitle += "\\par";
        strHTitle += sTail;
        sWw = strHTitle.toStdString();
    }

    CHARRANGE range;
    sel(range);
    int nCurrentRow = ::SendMessageW(m_hEdit, EM_LINEFROMCHAR, range.cpMin, 0); // 获取当前行
    {
        wchar_t buf[10];
        long lBufSize = 10;
        int nCount = line(nCurrentRow, buf, lBufSize) - 1; // 当前行字符数

        if (0 < nCount)
        {
            int nRowIndex = lineIndex(nCurrentRow);
            setSel(nRowIndex, nRowIndex); //
        }
    }

    SETTEXTEX st;
    st.codepage = 1200;
    st.flags = ST_SELECTION | ST_KEEPUNDO | ST_NEWCHARS;
    ::SendMessageW(m_hEdit, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)sWw.c_str());

    replaceSelectText("d", true, true, false, title, true);
    // 设置光标到下一行
    sel(range);
    nCurrentRow = (int)::SendMessageW(m_hEdit, EM_LINEFROMCHAR, range.cpMin, 0);
    int nextRowIndex = lineIndex(nCurrentRow + 1);
    setSel(nextRowIndex, nextRowIndex);
}

int GLDRichTextEditPrivate::currentRow() const
{
    CHARRANGE range;
    ::SendMessageW(m_hEdit, EM_EXGETSEL, 0, (LPARAM)&range);
    return (int)::SendMessageW(m_hEdit, EM_LINEFROMCHAR, range.cpMin, 0); // 获取当前行
}

void GLDRichTextEditPrivate::moveTextCursorNext()
{
    CHARRANGE range;
    ::SendMessageW(m_hEdit, EM_EXGETSEL, 0, (LPARAM)&range);
    int  nCurrentRow = (int)::SendMessageW(m_hEdit, EM_LINEFROMCHAR, range.cpMin, 0); // 获取当前行
    int nNextRowIndex = lineIndex(nCurrentRow + 1);
    setSel(nNextRowIndex, nNextRowIndex);
}

bool GLDRichTextEditPrivate::selectRegionText(const QString &beginText, const QString &endText, bool bCase)
{
    bool bOk = false;
    bOk = findText(beginText, bCase, true, true);

    if (!bOk)
    {
        return false;
    }

    CHARRANGE cr;
    sel(cr);
    LONG cpMin = cr.cpMin;

    bOk = findText(endText, bCase, true, true);

    if (!bOk)
    {
        return false;
    }

    sel(cr);
    setSel(cpMin, cr.cpMax);
    return true;
}

void GLDRichTextEditPrivate::test()
{
    std::string sWw;
    {
        QString strHTitle;


        QString sHead = "{\\rtf1";
        QString sh =
            "{\\stylesheet{ Normal;}{\\s1 heading 1;}{\\s2 heading 2;}{\\s3 heading 3;}\
        {\\s4 heading 4;}{\\s5 heading 5;}{\\s6 heading 6;}{\\s7 heading 7;}{\\s8 heading 8;}{\\s9 heading 9;}}"
            ;
        QString sTail = "}";

        strHTitle += sHead;
        strHTitle += sh;

        strHTitle += "\\pard\\keep\\keepn\\nowidctlpar\\s1\\sb340\\sa330\\sl576\\slmult1\\qj\\kerning44\\b\\fs44";

        strHTitle += "\\par";
        strHTitle += sTail;
        sWw = strHTitle.toStdString();
    }

    SETTEXTEX st;
    st.codepage = 1200;
    st.flags = ST_SELECTION | ST_KEEPUNDO | ST_NEWCHARS;
    ::SendMessageW(m_hEdit, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)sWw.c_str());
}

// return 0 for no error, otherwise return error code
DWORD CALLBACK GLDRichTextEditPrivate::editStreamCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    DWORD ret = 0;

    do
    {
        GLDRichTextEditPrivate::GArchive *pArchive = (GLDRichTextEditPrivate::GArchive *)dwCookie;

        if (pArchive->isLoad())
        {
            *pcb = pArchive->m_file->read((char *)pbBuff, cb);

        }
        else
        {
            pArchive->m_file->write((char *)pbBuff, cb);
        }

    }
    while (false);

    return ret;
}

void GLDRichTextEditPrivate::setCursorFocus()
{
    ::SetFocus(m_hEdit);
}

//////////////////////////////////////////////////////////////////////////

GLDRichTextEdit::TextRowIterator::TextRowIterator()
    : m_edit(NULL)
    , m_row(-1)
{

}

GLDRichTextEdit::TextRowIterator::TextRowIterator(GLDRichTextEdit *edit, int row)
    : m_edit(edit)
    , m_row(row)
{
    Q_ASSERT(NULL != edit);
}

GLDRichTextEdit::TextRowIterator::~TextRowIterator()
{

}

bool GLDRichTextEdit::TextRowIterator::isVaild() const
{
    if (NULL != m_edit)
    {
        if (0 < m_row && m_row < m_edit->m_d->richEditLineCount())
        {
            return true;
        }
    }

    return false;
}

QString GLDRichTextEdit::TextRowIterator::getText(int cell) const
{
    QString str;

    if (NULL != m_edit)
    {
        switch (m_edit->m_d->m_richEditVer)
        {
            case GLDRichTextEditPrivate::Msftedit:
                str = m_edit->m_d->textOnMsftedit(m_row, cell);
                break;

            default:
                str = m_edit->m_d->text(m_row, cell);
                break;
        }
    }

    return str;
}

void GLDRichTextEdit::TextRowIterator::setText(int cell, const QString &text)
{
    if (NULL != m_edit)
    {
        switch (m_edit->m_d->m_richEditVer)
        {
            case GLDRichTextEditPrivate::Msftedit:
                m_edit->m_d->setTextOnMsftedit(m_row, cell, text);
                break;

            default:
                m_edit->m_d->setText(m_row, cell, text);
                break;
        }
    }
}

bool GLDRichTextEdit::TextRowIterator::hasCell() const
{
    if (NULL != m_edit)
    {
        switch (m_edit->m_d->m_richEditVer)
        {
            case GLDRichTextEditPrivate::Msftedit:
                return m_edit->m_d->hasCellOnMsftedit(m_row);
                break;

            default:
                return m_edit->m_d->hasCell(m_row);
                break;
        }
    }

    return false;
}

int GLDRichTextEdit::TextRowIterator::cellCount() const
{
    if (NULL != m_edit)
    {
        switch (m_edit->m_d->m_richEditVer)
        {
            case GLDRichTextEditPrivate::Msftedit:
                return m_edit->m_d->cellCountOnMsftedit(m_row);
                break;

            default:
                return m_edit->m_d->cellCount(m_row);
                break;
        }
    }

    return 0;
}

GLDRichTextEdit::TextRowIterator GLDRichTextEdit::TextRowIterator::next() const
{
    return TextRowIterator(m_edit, m_row + 1);
}

//////////////////////////////////////////////////////////////////////////

GLDRichTextEdit::GLDRichTextEdit(QWidget *parent)
    : QWidget(parent)
    , m_d(NULL)
{
    m_d = new GLDRichTextEditPrivate(this);
    Q_ASSERT(NULL != m_d);

    setDefaultFont(QFont(QString::fromStdWString(L"新宋体")), 12, Qt::black);

    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Dark);
    m_dZoomFactor = 1;
    initRBtnMenu();

}

GLDRichTextEdit::~GLDRichTextEdit()
{
    delete m_d;
}

QPoint GLDRichTextEdit::getTextCursor() const
{
    QPoint pt;
    CHARRANGE cr;
    m_d->sel(cr);
    pt = QPoint(cr.cpMin, cr.cpMax);
    return pt;
}

void GLDRichTextEdit::setTextCursor(const QPoint &pos)
{
    m_d->setSel(pos.x(), pos.y());
}

int GLDRichTextEdit::getCurrentRow() const
{
    return m_d->currentRow();
}

void GLDRichTextEdit::moveTextCursorNext()
{
    m_d->moveTextCursorNext();
}

void GLDRichTextEdit::open()
{
    open(getOpenFileName());
}

void GLDRichTextEdit::open(const QString &fileName)
{
    do
    {
        // 保存原来的
        if (!m_d->m_fileName.isEmpty())
        {
            if (isModify())
            {
                save();
            }
        }

        // 打开新的
        if (fileName.isEmpty())
        {
            break;
        }

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly))
        {
            break;
        }

        GLDRichTextEditPrivate::GArchive loadArchive(&file, GLDRichTextEditPrivate::GArchive::load);
        loadArchive.m_pDocument = m_d;

        EDITSTREAM es = { 0, 0, GLDRichTextEditPrivate::editStreamCallBack };
        es.dwCookie = (DWORD_PTR)&loadArchive;
        DWORD rt = 0;
        rt = (DWORD)::SendMessageW(m_d->m_hEdit, EM_STREAMIN, SF_RTF, (LPARAM)&es);

        file.close();

        m_d->m_fileName = fileName;
        setModify(false);

        setWindowTitle(QFileInfo(m_d->m_fileName).baseName());

    }
    while (false);
}

int GLDRichTextEdit::save()
{
    int nRet = 0;

    do
    {
        if (!isModify())
        {
            break;
        }

        if (m_d->m_fileName.isEmpty())
        {
            m_d->m_fileName = getSaveFileName(GLD_REICH_TEXT("新建文件.rtf"));

            if (m_d->m_fileName.isEmpty())
            {
                nRet = -1;
                break;
            }
        }

        QFile file(m_d->m_fileName);

        if (!file.open(QIODevice::WriteOnly))
        {
            nRet = -2;
            break;
        }

        GLDRichTextEditPrivate::GArchive storeArchive(&file, GLDRichTextEditPrivate::GArchive::store);
        storeArchive.m_pDocument = m_d;

        EDITSTREAM es = { 0, 0, GLDRichTextEditPrivate::editStreamCallBack };
        es.dwCookie = (DWORD_PTR)&storeArchive;
        DWORD rt = 0;
        rt = (DWORD)::SendMessageW(m_d->m_hEdit, EM_STREAMOUT, SF_RTF, (LPARAM)&es);

        file.close();
        setModify(false);
    }
    while (false);

    return nRet;
}

void GLDRichTextEdit::onSave()
{
    save();

    m_d->setCursorFocus();
}

void GLDRichTextEdit::saveAs()
{
    saveAs(getSaveFileName(GLD_REICH_TEXT("未命名.rtf")), true);

    m_d->setCursorFocus();
}

void GLDRichTextEdit::saveAs(const QString &fileName, bool userSaveAsFileName)
{
    do
    {
        if (fileName.isEmpty())
        {
            break;
        }

        if (!m_d->m_fileName.isEmpty())
        {
            save();
        }

        if (fileName == m_d->m_fileName)
        {
            break;
        }

        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly))
        {
            break;
        }

        GLDRichTextEditPrivate::GArchive storeArchive(&file, GLDRichTextEditPrivate::GArchive::store);
        storeArchive.m_pDocument = m_d;

        EDITSTREAM es = { 0, 0, GLDRichTextEditPrivate::editStreamCallBack };
        es.dwCookie = (DWORD_PTR)&storeArchive;
        ::SendMessageW(m_d->m_hEdit, EM_STREAMOUT, SF_RTF, (LPARAM)&es);

        if (userSaveAsFileName)
        {
            m_d->m_fileName = fileName;
        }

        setModify(false);
        file.close();

        setWindowTitle(QFileInfo(m_d->m_fileName).baseName());

    }
    while (false);
}

int GLDRichTextEdit::saveToBuf(QByteArray &byteArray)
{
    int nRet = 0;

    do
    {
        QBuffer buf(&byteArray);

        if (!buf.open(QIODevice::WriteOnly))
        {
            nRet = -2;
            break;
        }

        GLDRichTextEditPrivate::GArchive storeArchive(&buf, GLDRichTextEditPrivate::GArchive::store);
        storeArchive.m_pDocument = m_d;

        EDITSTREAM es = { 0, 0, GLDRichTextEditPrivate::editStreamCallBack };
        es.dwCookie = (DWORD_PTR)&storeArchive;
        DWORD rt = 0;
        rt = (DWORD)::SendMessageW(m_d->m_hEdit, EM_STREAMOUT, SF_RTF, (LPARAM)&es);

        buf.close();
    }
    while (false);

    return nRet;
}

int GLDRichTextEdit::readFromBuf(QByteArray &byteArray)
{
    do
    {
        QBuffer buf(&byteArray);

        if (!buf.open(QIODevice::ReadOnly))
        {
            break;
        }

        GLDRichTextEditPrivate::GArchive loadArchive(&buf, GLDRichTextEditPrivate::GArchive::load);
        loadArchive.m_pDocument = m_d;

        EDITSTREAM es = { 0, 0, GLDRichTextEditPrivate::editStreamCallBack };
        es.dwCookie = (DWORD_PTR)&loadArchive;
        DWORD rt = 0;
        rt = (DWORD)::SendMessageW(m_d->m_hEdit, EM_STREAMIN, SF_RTF, (LPARAM)&es);
        buf.close();
    }
    while (false);

    return 0;
}

void GLDRichTextEdit::cut()
{
    m_d->cut();
}

void GLDRichTextEdit::catchWheelup()
{
    ::SetFocus((HWND)this->winId());
    if (g_bResize)
    {
        emit updateScroll();
        g_bResize = false;
    }
}

void GLDRichTextEdit::copy()
{
    m_d->copy();
}

void GLDRichTextEdit::paste()
{
    m_d->past();
}

void GLDRichTextEdit::del()
{
    m_d->del();
}

void GLDRichTextEdit::undo()
{
    m_d->undo();
    emit updateScroll();
}

void GLDRichTextEdit::redo()
{
    m_d->redo();
    emit updateScroll();
}

void GLDRichTextEdit::setUndoCount(int undoCount)
{
    m_d->setUndoCount(undoCount);
}

void GLDRichTextEdit::selectAll()
{
    m_d->selectAll();
}

void GLDRichTextEdit::insertImage(const QString &image, int w, int h, bool bMonochrome)
{
    m_d->insertImage(image, w, h, bMonochrome);

    m_d->setCursorFocus();
}

void GLDRichTextEdit::insertText(const QString &text)
{
    m_d->insertText(text);
    updateUI(QEvent::KeyPress, Qt::NoButton, Qt::Key_No);
}

void GLDRichTextEdit::insertTable(int rows, int columns)
{
    m_d->insertTable(rows, columns);

    m_d->setCursorFocus();
}

bool GLDRichTextEdit::isModify() const
{
    if (FALSE == (BOOL)::SendMessageW(m_d->m_hEdit, EM_GETMODIFY, 0, 0))
    {
        return false;
    }

    return true;
}

void GLDRichTextEdit::setModify(bool modified)
{
    ::SendMessageW(m_d->m_hEdit, EM_SETMODIFY, modified ? TRUE : FALSE, 0);
}

void GLDRichTextEdit::setDefaultFont(const QFont &fnt, int size, const QColor &clr)
{
    m_d->setDefaultFont(fnt, size, clr);
}

QString GLDRichTextEdit::getSelectText() const
{
    return m_d->selectText();
}

QString GLDRichTextEdit::getAllText() const
{
    return m_d->allText();
}

const QString &GLDRichTextEdit::getFileName() const
{
    return m_d->m_fileName;
}

QFont GLDRichTextEdit::selectionCharFont(bool bMousePress) const
{
    return m_d->selectionCharFont(bMousePress);
}

QColor GLDRichTextEdit::selectionCharFontColor() const
{
    return m_d->selectionCharFontColor();
}

QColor GLDRichTextEdit::selectionCharBackgroundColor() const
{
    return m_d->selectionCharBackgroundColor();
}

int GLDRichTextEdit::getLineCount() const
{
    return m_d->richEditLineCount();
}

GLDRichTextEdit::TextRowIterator GLDRichTextEdit::getRowIterator(int row)
{
    return TextRowIterator(this, row);
}

bool GLDRichTextEdit::getSeriesTableIterator(TextRowIterator &start, TextRowIterator &end, int rowIndex)
{
    int nRowStart = 0;
    int nRowEnd = 0;

    switch (m_d->m_richEditVer)
    {
        case GLDRichTextEditPrivate::Msftedit:
            if (m_d->seriesTableOnMsftedit(nRowStart, nRowEnd, rowIndex))
            {
                start = TextRowIterator(this, nRowStart);
                end = TextRowIterator(this, nRowEnd);;
                return true;
            }
            else
            {
                start.m_edit = NULL;
                end.m_edit = NULL;
            }

            break;

        default:
            if (m_d->seriesTable(nRowStart, nRowEnd, rowIndex))
            {
                start = TextRowIterator(this, nRowStart);
                end = TextRowIterator(this, nRowEnd);;
                return true;
            }
            else
            {
                start.m_edit = NULL;
                end.m_edit = NULL;
            }

            break;
    }

    return false;
}

QString GLDRichTextEdit::getOpenFileName() const
{
    QFileInfo flIn(QFileDialog::getOpenFileName(NULL, GLD_REICH_TEXT("选择文件"), QString(), GLD_REICH_TEXT("*.rtf")));

    if (!flIn.isFile() || 0 != flIn.suffix().compare("rtf", Qt::CaseInsensitive))
    {
        return QString();
    }

    return flIn.absoluteFilePath();
}

QString GLDRichTextEdit::getSaveFileName(const QString &defaultName) const
{
    QFileInfo flIn(QFileDialog::getSaveFileName(NULL, GLD_REICH_TEXT("保存%1文件").arg(windowTitle()),
                                                defaultName, GLD_REICH_TEXT("文件(*.rtf)")));

    QString sFile = flIn.absoluteFilePath();

    if (!sFile.endsWith(".rtf", Qt::CaseInsensitive))
    {
        sFile += ".rtf";
    }

    return sFile;
}

void GLDRichTextEdit::bold()
{
    m_d->setSelectionCharFormat(CFM_BOLD, CFE_BOLD);
}

void GLDRichTextEdit::setBold(bool enable)
{
    m_d->setSelectionCharFormat(CFM_BOLD, CFE_BOLD, enable);
}

void GLDRichTextEdit::italic()
{
    m_d->setSelectionCharFormat(CFM_ITALIC, CFE_ITALIC);
}

void GLDRichTextEdit::setItalic(bool enable)
{
    m_d->setSelectionCharFormat(CFM_ITALIC, CFE_ITALIC, enable);
}

void GLDRichTextEdit::underline()
{
    m_d->setSelectionCharFormat(CFM_UNDERLINE, CFE_UNDERLINE);
}

void GLDRichTextEdit::setUnderline(bool enable)
{
    m_d->setSelectionCharFormat(CFM_UNDERLINE, CFE_UNDERLINE, enable);
}

void GLDRichTextEdit::strikeout()
{
    m_d->setSelectionCharFormat(CFM_STRIKEOUT, CFE_STRIKEOUT);
}

void GLDRichTextEdit::setStrikeout(bool enable)
{
    m_d->setSelectionCharFormat(CFM_STRIKEOUT, CFE_STRIKEOUT, enable);
}

void GLDRichTextEdit::subscript()
{
    m_d->setSelectionCharFormat(CFM_SUBSCRIPT, CFE_SUBSCRIPT);
}

void GLDRichTextEdit::setSubscript(bool enable)
{
    m_d->setSelectionCharFormat(CFM_SUBSCRIPT, CFE_SUBSCRIPT, enable);
}

void GLDRichTextEdit::superscript()
{
    m_d->setSelectionCharFormat(CFM_SUPERSCRIPT, CFE_SUPERSCRIPT);
}

void GLDRichTextEdit::setSuperscript(bool enable)
{
    m_d->setSelectionCharFormat(CFM_SUPERSCRIPT, CFE_SUPERSCRIPT, enable);
}

void GLDRichTextEdit::setFont(const QFont &fnt)
{
    m_d->setSelectionCharFont(fnt);

    m_d->setCursorFocus();
}

void GLDRichTextEdit::setFontSize(int size)
{
    m_d->setSelectionCharSize(size);
}

void GLDRichTextEdit::setFontColor(const QColor &clr)
{
    m_d->setSelectionCharColor(clr);

    m_d->setCursorFocus();
}

void GLDRichTextEdit::setFontBackgroundColor(const QColor &color)
{
    m_d->setSelectionCharBackgroundColor(color);
    m_d->setCursorFocus();
}

void GLDRichTextEdit::setReadOnly(bool value)
{
    m_d->setReadOnly(value);
}

void GLDRichTextEdit::setZoomFactor(bool bIsZoomIn)
{
    double dCurrentZoom = m_dZoomFactor;

    // 缩放比例改变0.1
    if (bIsZoomIn && (m_dZoomFactor + 0.1 < c_maxZoom))
    {
        dCurrentZoom = m_dZoomFactor + 0.1;
    }
    else if ((!bIsZoomIn) && (m_dZoomFactor - 0.1 > c_minZoom))
    {
        dCurrentZoom = m_dZoomFactor - 0.1;
    }

    setZoomFactorValue(dCurrentZoom);

    emit updateScroll();
    emit zoomFactorChanged();
}

void GLDRichTextEdit::setZoomFactorValue(const double &zoomFactor)
{
    HWND hParentHwnd = ::GetParent(m_d->m_hEdit);
    RECT ParentRect;

    ::GetClientRect(hParentHwnd, &ParentRect);

    double dSlope   = 0;
    double dSection = 0;

    if (zoomFactor >= 1)
    {
        dSlope   = c_kBigThanOne;   // 直线的斜率和斜截值
        dSection = c_bBigThanOne;
    }
    else
    {
        dSlope   = c_kSmallThanOne; // 直线的斜率和斜截值
        dSection = c_bSmallThanOne;
    }

    long destWdith = ParentRect.right * (dSlope * zoomFactor + dSection);
    RECT rect;
    ::GetClientRect(m_d->m_hEdit, &rect);
    if ((rect.right - rect.left) > m_nScrollWidth)
    {
        ::MoveWindow(m_d->m_hEdit, 0, 1,
                 ParentRect.right - destWdith * 2,
                 ParentRect.bottom, TRUE);
    }
    else
    {
        ::MoveWindow(m_d->m_hEdit, destWdith, 1,
                 ParentRect.right - destWdith * 2,
                 ParentRect.bottom, TRUE);
    }

    m_d->setZoomFactor(zoomFactor * c_FontAndBckZoom);
    m_dZoomFactor = zoomFactor;

    m_d->setCursorFocus();
}

void GLDRichTextEdit::resizeEvent(QResizeEvent *sizeEvent)
{
    setZoomFactorValue(m_dZoomFactor);
    Q_UNUSED(sizeEvent)
}

void GLDRichTextEdit::closeEvent(QCloseEvent *e)
{
    if (isModify())
    {
        QMessageBox::Button btn = QMessageBox::warning(this,
                                  QString::fromStdWString(L"提示"),
                                  QString::fromStdWString(L"文件已修改，是否保存？"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        switch (btn)
        {
            case QMessageBox::Yes:
                if (0 == save())
                {
                    QWidget::closeEvent(e);
                }
                else
                {
                    e->ignore();
                }

                break;

            case QMessageBox::No:
                QWidget::closeEvent(e);
                break;

            case QMessageBox::Cancel:
                e->ignore();
                m_d->setCursorFocus();
                break;

            default:
                break;
        }
    }
    else
    {
        QWidget::closeEvent(e);
    }
}

void GLDRichTextEdit::setAlign(Align align)
{
    WORD alignment = PFA_LEFT;

    switch (align)
    {
        case GLDRichTextEdit::Left:
            break;

        case GLDRichTextEdit::Center:
            alignment = PFA_CENTER;
            break;

        case GLDRichTextEdit::Right:
            alignment = PFA_RIGHT;
            break;

        case GLDRichTextEdit::Justify:
            alignment = PFA_JUSTIFY;
            break;

        default:
            break;
    }

    m_d->setParaFormat(alignment);

    m_d->setCursorFocus();
}

bool GLDRichTextEdit::findText(const QString &find, bool bCase, bool bWord, bool bNext)
{
    return m_d->findText(find, bCase, bWord, bNext);
}

bool GLDRichTextEdit::replaceSelectText(const QString &find, bool bCase, bool bWord,
                                        bool bNext, const QString &replace, bool findAndImReplace)
{
    return m_d->replaceSelectText(find, bCase, bWord, bNext, replace, findAndImReplace);
}

bool GLDRichTextEdit::replaceSelectTextToImage(const QString &find, bool bCase, bool bWord, bool bNext,
                                               const QString &imageFileName, bool findAndImReplace, int w,
                                               int h, bool bMonochrome)
{
    return m_d->replaceTextToImage(find, bCase, bWord, bNext, imageFileName, findAndImReplace, w, h, bMonochrome);
}

void GLDRichTextEdit::replaceSelectText(const QString &find, bool bCase, bool bWord, bool bNext,
                                        const QString &replace)
{
    m_d->replaceSelectText(find, bCase, bWord, bNext, replace, false);

    m_d->setCursorFocus();
}

void GLDRichTextEdit::replaceAllText(const QString &find, bool bCase, bool bWord, const QString &replace)
{
    m_d->replaceAllText(find, bCase, bWord, replace);

    m_d->setCursorFocus();
}

void GLDRichTextEdit::replaceSelectTextToImage(const QString &find, bool bCase, bool bWord, bool bNext,
                                               const QString &image, int w, int h, bool bMonochrome)
{
    m_d->replaceTextToImage(find, bCase, bWord, bNext, image, false, w, h, bMonochrome);

    m_d->setCursorFocus();
}

void GLDRichTextEdit::setTextCursorToHead()
{
    m_d->setTextCursorToHead();
}

void GLDRichTextEdit::setTextCursorToTail()
{
    m_d->setTextCursorToTail();
}

bool GLDRichTextEdit::selectRegionText(const QString &beginText, const QString &endText, bool bCase)
{
    return m_d->selectRegionText(beginText, endText, bCase);
}

void GLDRichTextEdit::test()
{
    m_d->test();
}

void GLDRichTextEdit::updateUI(QEvent::Type eType, Qt::MouseButtons btn, Qt::Key eKey)
{
    if (QEvent::MouseButtonPress == eType)
    {
        emit activeWindow(this);
        emit updateUIState(this, true);

        if ((Qt::RightButton == btn))
        {
            addRightBtnAction();
        }
    }
    else
    {
        emit updateUIState(this, false);
    }

    Q_UNUSED(eKey)
}

void GLDRichTextEdit::insertTitleHead(const QString &titleText, int num, GLDRichTextEdit::Align align)
{
    m_d->insertTitleHead(titleText, num, align);
}

QString GLDRichTextEdit::getTextDocTitle() const
{
    QFileInfo fl(m_d->m_fileName);
    return fl.baseName();
}

void GLDRichTextEdit::initRBtnMenu()
{
    m_pMenu = new QMenu(this);

    m_pMenuCut = m_pMenu->addAction(GLD_REICH_TEXT("剪切"));
    m_pMenuCut->setIcon(QIcon(c_cutIcon));
    connect(m_pMenuCut, SIGNAL(triggered(bool)), this, SLOT(cut()));

    m_pMenuCopy = m_pMenu->addAction(GLD_REICH_TEXT("复制"));
    m_pMenuCopy->setIcon(QIcon(c_copyIcon));
    connect(m_pMenuCopy, SIGNAL(triggered(bool)), this, SLOT(copy()));

    m_pMenuPaste = m_pMenu->addAction(GLD_REICH_TEXT("粘贴"));
    m_pMenuPaste->setIcon(QIcon(c_pasteIcon));

    connect(m_pMenuPaste, SIGNAL(triggered(bool)), this, SLOT(paste()));
}

void GLDRichTextEdit::addRightBtnAction()
{
    QCursor cur = this->cursor();

    if (getSelectText().isEmpty())
    {
        m_pMenuCopy->setEnabled(false);
        m_pMenuCut->setEnabled(false);
    }
    else
    {
        m_pMenuCopy->setEnabled(true);
        m_pMenuCut->setEnabled(true);
    }

    QClipboard *clipboard = QApplication::clipboard();

    if (clipboard->text().isEmpty())
    {
        m_pMenuPaste->setEnabled(false);
    }
    else
    {
        m_pMenuPaste->setEnabled(true);
    }

//    m_pMenu->exec(cur.pos());
}

void GLDRichTextEdit::saveAsDoc()
{
    if (getFileName().isEmpty())
    {
        QFileInfo flIn(QFileDialog::getSaveFileName(this, GLD_REICH_TEXT("另存文件"),
                                                    GLD_REICH_TEXT("./未命名.doc"),
                                                    GLD_REICH_TEXT("文件 (*.doc)")));
        QString sFile = flIn.absoluteFilePath();

        if (!sFile.endsWith(".doc", Qt::CaseInsensitive))
        {
            sFile += ".doc";
        }

        // 临时文件目录
        QString tempFile;
        QDir dirTemp = QDir::temp();
        QString uid = QUuid::createUuid().toString().replace("-", "_");

        uid.chop(1);
        uid.remove(0, 1);
        tempFile = dirTemp.absoluteFilePath(uid + ".rtf");

        saveAs(tempFile, false);

        saveToDoc(sFile, tempFile);

        QFile::remove(tempFile);
    }
    else
    {
        QString srcFileName = getFileName();
        QStringList Filelist = srcFileName.split("/");
        QString srcFileAbsName = Filelist[Filelist.size() - 1];

        // 将文件后缀名3个字符替换为doc
        srcFileAbsName = srcFileAbsName.replace(srcFileAbsName.size() - 3, 3, "doc");

        QFileInfo flIn(QFileDialog::getSaveFileName(this, GLD_REICH_TEXT("另存文件"),
                       GLD_REICH_TEXT("%1").arg(srcFileAbsName),
                       GLD_REICH_TEXT("文件 (*.doc)")));

        QString sAbsPathFile = flIn.absoluteFilePath();

        if (!sAbsPathFile.endsWith(".doc", Qt::CaseInsensitive))
        {
            sAbsPathFile += ".doc";
        }

        save();

        saveToDoc(sAbsPathFile, srcFileName);
    }
}

void GLDRichTextEdit::saveToDoc(QString desDocName, QString srcFileName)
{
    QAxObject word("Word.Application", NULL);
    word.setProperty("Visible", false);
    QAxObject *documents = word.querySubObject("Documents");

    documents->querySubObject("Open(QString&)", srcFileName);
    QAxObject *document = word.querySubObject("ActiveDocument");
    Q_ASSERT(NULL != document);

    QVariantList saveAsParam;
    saveAsParam.append(desDocName);
    saveAsParam.append(0);
    document->dynamicCall("SaveAs(QVariant&, QVariant&)", saveAsParam);
    word.dynamicCall("Quit()");

    delete document;
    delete documents;
}

bool GLDRichTextEdit::canUndo()
{
    return m_d->canUndo();
}

bool GLDRichTextEdit::canRedo()
{
    return m_d->canRedo();
}

double GLDRichTextEdit::zoomFactor() const
{
    return m_dZoomFactor;
}

void GLDRichTextEdit::emitRButtonUp()
{
    emit onRButtonUp();
}

void GLDRichTextEdit::emitUpdateScroll()
{
    emit updateScroll();
}

void GLDRichTextEdit::setCursorFocus()
{
    m_d->setCursorFocus();
}

void GLDRichTextEdit::enterKeyPress()
{
    g_bResize = true;
}

int GLDRichTextEdit::contentHeight()
{
    return m_d->contentHeight();
}

int GLDRichTextEdit::contentWidth()
{
    return m_d->contentWidth();
}

void GLDRichTextEdit::alwaysHideScrollBar()
{
    ShowScrollBar(m_d->m_hEdit, SB_VERT, FALSE);
    EnableScrollBar(m_d->m_hEdit, SB_VERT, ESB_DISABLE_BOTH);
}

GLDRichEditScrollArea::GLDRichEditScrollArea(QWidget *parent, GLDRichTextEdit *richEdit)
    : QScrollArea(parent), m_pRichEditObject(richEdit), m_pRichEditPrivate(NULL)
{
    m_nContentHeight = 800;  // 800为任意的大于初始窗口高度400值
    setMouseTracking(true);
    m_bIsResize = true;

    if (NULL != richEdit)
    {
        m_pRichEditObject = richEdit;
        m_pRichEditPrivate = richEdit->m_d;
        setWidget(richEdit);
    }
}

void GLDRichEditScrollArea::resizeEvent(QResizeEvent *e)
{
    if (m_bIsResize)
    {
        QSize sz = e->size();
        this->m_pRichEditObject->setGeometry(0, 0, sz.width(), sz.height());
        m_nScrollAreaHeight = sz.height();
        m_nScrollAreaWidth = sz.width();
        m_pRichEditObject->m_nScrollWidth = sz.width();

        if (m_nContentHeight > m_nScrollAreaHeight)
        {
            m_nContentHeight = m_pRichEditObject->contentHeight();
            m_pRichEditObject->setFixedSize(sz.width(), m_nContentHeight);
        }
        else
        {
            m_nContentHeight = m_pRichEditObject->contentHeight();
            m_pRichEditObject->setFixedSize(sz.width(), m_nScrollAreaHeight);
        }
    }
}

void GLDRichEditScrollArea::closeEvent(QCloseEvent *e)
{
    this->m_pRichEditObject->closeEvent(e);
}

void GLDRichEditScrollArea::setContentHeight()
{
    m_pRichEditObject->alwaysHideScrollBar();
    GLDRichTextEdit *pEdit = qobject_cast<GLDRichTextEdit *>(widget());
    if  (NULL == pEdit)
    {
        return ;
    }

    if (m_nContentHeight > m_nScrollAreaHeight)
    {
        pEdit->setFixedHeight(1000);
        m_nContentHeight = pEdit->contentHeight();

        int nWidth = pEdit->contentWidth();
        if (nWidth > m_nScrollAreaWidth)
        {
           pEdit->setFixedSize(nWidth, m_nContentHeight);
        }
        else
        {
           pEdit->setFixedSize(m_nScrollAreaWidth + 1, m_nContentHeight); //加1是比resizeEvent中大一个像素
        }
    }
    else
    {
        m_nContentHeight = pEdit->contentHeight();
        pEdit->setFixedHeight(m_nScrollAreaHeight);
    }

    m_pRichEditObject->alwaysHideScrollBar();
    m_bIsResize = false;
}

void GLDRichEditScrollArea::updateUISlot()
{
    setContentHeight();
}
