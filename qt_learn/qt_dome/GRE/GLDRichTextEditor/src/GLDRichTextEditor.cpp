#include <QAction>
#include <QActionGroup>
#include <QAxWidget>
#include <QAxObject>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>
#include <QToolBar>
#include <QUuid>

#include "GLDRichTextEdit.h"
#include "GLDRichTextEditAddTableDlg.h"
#include "GLDRichTextEditFindAndReplaceWidget.h"
#include "GLDRichTextEditGlobal.h"
#include "GLDRichTextEditor.h"
#include "GLDRichTextEditTableValueTest.h"

#define GLD_RICH_TEXT_EDITOR_DOC_MARK "QMdiSubWindow::RichEditWindow"

#define STATE_NEW  1
#define STATE_OPEN 2
const int c_sliderMinPos         = 10;
const int c_sliderMaxPos         = 500;

// 图片资源文件路径
const QString c_newIcon        = ":/smallnew.png";
const QString c_openIcon       = ":/smallOpen.png";
const QString c_saveIcon       = ":/smallSave.png";
const QString c_saveAsIcon     = ":/smallSaveAsFile.png";

const QString c_undoIcon       = ":/smallUndo.png";
const QString c_redoIcon       = ":/smallRedo.png";
const QString c_cutIcon        = ":/smallcut.png";
const QString c_copyIcon       = ":/smallcopy.png";
const QString c_pasteIcon      = ":/smallpaste.png";

const QString c_backgroundIcon = ":/smallBackgroundColor.png";
const QString c_colorsIcon     = ":/smallcolors.png";

const QString c_textboldIcon   = ":/smalltextbold.png";
const QString c_textItalic     = ":/smalltextitalic.png";
const QString c_textUnderIcon  = ":/smalltextunder.png";
const QString c_strThroughIcon = ":/smallstrikethrough.png";
const QString c_subScriptIcon  = ":/smallsubscript.png";
const QString c_superScriptIcon= ":/smallsuperscript.png";

const QString c_alignLeftIcon  = ":/smallalignleft.png";
const QString c_centerIcon     = ":/smallcenter.png";
const QString c_alignRightIcon = ":/smallalignright.png";

const QString c_insertPicIcon  = ":/smallinsertpic.png";
const QString c_insertTableIcon= ":/smallinserttable.png";
const QString c_findIcon       = ":/smallfind.png";
const QString c_replaceIcon    = ":/smallreplace.png";

const QString c_zoomInIcon     = ":/smallZoomIn.png";
const QString c_zoomOutIcon    = ":/smallZoomOut.png";
const QString c_pageBordersIcon= ":/largePageBorders.png";


GLDRichTextEditor::GLDRichTextEditor(QWidget *parent)
    : QMainWindow(parent)
    , m_pArea(NULL)
    , m_pacNew(NULL)
    , m_pacOpen(NULL)
    , m_pacSave(NULL)
    , m_pacSaveAs(NULL)

    , m_pacUndo(NULL)
    , m_pacRedo(NULL)
    , m_pacCut(NULL)
    , m_pacCopy(NULL)
    , m_pacPaste(NULL)

    , m_pCmbFont(NULL)
    , m_pCmbSize(NULL)
    , m_pacColor(NULL)

    , m_pacBold(NULL)
    , m_pacItalic(NULL)
    , m_pacUnderline(NULL)
    , m_pacStrikeOut(NULL)
    , m_pacSub(NULL)
    , m_pacSup(NULL)

    , m_pacAlignLeft(NULL)
    , m_pacAlignCenter(NULL)
    , m_pacAlignRight(NULL)

    , m_pacInsertPic(NULL)
    , m_pacInsertTable(NULL)

    , m_findAndReplace(NULL)
    , m_pacFind(NULL)
    , m_pacReplace(NULL)

#ifdef _DEBUG
    , m_tableTest(NULL)
    , m_pacTableTest(NULL)
#endif
{
    ui.setupUi(this);

    m_nState = 0;
    init();
}

GLDRichTextEditor::~GLDRichTextEditor()
{
    delete m_slider;
    if (NULL != m_findAndReplace)
    {
        delete m_findAndReplace;
        m_findAndReplace = NULL;
    }
#ifdef _DEBUG
    if (NULL != m_tableTest)
    {
        delete m_tableTest;
        m_tableTest = NULL;
    }
#endif
}

void GLDRichTextEditor::init()
{
    setWindowTitle(GLD_REICH_TEXT("文本编辑器"));

    m_pArea = new GMdiArea();
    m_pArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_pArea);

    connect(m_pArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subWindowChanged(QMdiSubWindow *)));

    initMenuAndToolBar();
}

void GLDRichTextEditor::initMenuBar()
{
    QMenuBar *pMenuBar = menuBar();

    QMenu *pMuFile = pMenuBar->addMenu(GLD_REICH_TEXT("文件"));
    QMenu *pMuEdit = pMenuBar->addMenu(GLD_REICH_TEXT("编辑"));

    m_pacNew = pMuFile->addAction(GLD_REICH_TEXT("新建"));
    m_pacNew->setIcon(QIcon(c_newIcon));
    connect(m_pacNew, SIGNAL(triggered(bool)), this, SLOT(newfile(bool)));

    m_pacOpen = pMuFile->addAction(GLD_REICH_TEXT("打开"));
    m_pacOpen->setIcon(QIcon(c_openIcon));
    connect(m_pacOpen, SIGNAL(triggered(bool)), this, SLOT(open(bool)));

    m_pacSave = pMuFile->addAction(GLD_REICH_TEXT("保存"));
    m_pacSave->setIcon(QIcon(c_saveIcon));
    connect(m_pacSave, SIGNAL(triggered(bool)), this, SLOT(save(bool)));

    m_pacSaveAs = pMuFile->addAction(GLD_REICH_TEXT("另存为..."));
    m_pacSaveAs->setIcon(QIcon(c_saveAsIcon));
    connect(m_pacSaveAs, SIGNAL(triggered(bool)), this, SLOT(saveAs(bool)));

    m_pacUndo = pMuEdit->addAction(GLD_REICH_TEXT("撤销"));
    m_pacUndo->setEnabled(false);
    m_pacUndo->setIcon(QIcon(c_undoIcon));
    connect(m_pacUndo, SIGNAL(triggered(bool)), this, SLOT(undo(bool)));

    m_pacRedo = pMuEdit->addAction(GLD_REICH_TEXT("重做"));
    m_pacRedo->setEnabled(false);
    m_pacRedo->setIcon(QIcon(c_redoIcon));
    connect(m_pacRedo, SIGNAL(triggered(bool)), this, SLOT(redo(bool)));

    m_pacCut = pMuEdit->addAction(GLD_REICH_TEXT("剪切"));
    m_pacCut->setIcon(QIcon(c_cutIcon));
    connect(m_pacCut, SIGNAL(triggered(bool)), this, SLOT(cut(bool)));

    m_pacCopy = pMuEdit->addAction(GLD_REICH_TEXT("拷贝"));
    m_pacCopy->setIcon(QIcon(c_copyIcon));
    connect(m_pacCopy, SIGNAL(triggered(bool)), this, SLOT(copy(bool)));

    m_pacPaste = pMuEdit->addAction(GLD_REICH_TEXT("粘贴"));
    m_pacPaste->setIcon(QIcon(c_pasteIcon));
    connect(m_pacPaste, SIGNAL(triggered(bool)), this, SLOT(paste(bool)));
}

QToolBar * GLDRichTextEditor::addMenuActionToToolBar()
{
    QToolBar *pTBar = ui.mainToolBar;

    pTBar->addAction(m_pacNew);
    pTBar->addAction(m_pacOpen);
    pTBar->addAction(m_pacSave);
    pTBar->addAction(m_pacSaveAs);
    pTBar->addSeparator();

    pTBar->addAction(m_pacUndo);
    pTBar->addAction(m_pacRedo);
    pTBar->addAction(m_pacCut);
    pTBar->addAction(m_pacCopy);
    pTBar->addAction(m_pacPaste);
    pTBar->addSeparator();

    return pTBar;
}

void GLDRichTextEditor::addFontSettingToToolBar(QToolBar *pTBar)
{
    m_pCmbFont = new QFontComboBox(pTBar);
    pTBar->addWidget(m_pCmbFont);

    connect(m_pCmbFont, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onSetfont(const QFont &)));
    m_pCmbSize = new QComboBox(pTBar);
    m_pCmbSize->setEditable(true);

    QList<int> fntSizes = QFontDatabase::standardSizes();
    for (int i = 0; i < fntSizes.size(); ++i)
    {
        m_pCmbSize->addItem(QString::number(fntSizes.at(i)));
    }
    pTBar->addWidget(m_pCmbSize);
    connect(m_pCmbSize, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onsetSize(const QString &)));

    m_pReadOnlyCmb = new QComboBox(this);
    m_pReadOnlyCmb->addItem("Write");
    m_pReadOnlyCmb->addItem("Read");
    pTBar->addWidget(m_pReadOnlyCmb);
    connect(m_pReadOnlyCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(onSetReadOnly(int)));

    m_pacBackgroundColor = pTBar->addAction("backgroundColor");
    m_pacBackgroundColor->setIcon(QIcon(c_backgroundIcon));
    connect(m_pacBackgroundColor, SIGNAL(triggered(bool)), this, SLOT(onSetBackground(bool)));

    m_pacColor = pTBar->addAction("color");
    m_pacColor->setIcon(QIcon(c_colorsIcon));
    connect(m_pacColor, SIGNAL(triggered(bool)), this, SLOT(onSetColor(bool)));

    m_pacBold = pTBar->addAction(GLD_REICH_TEXT("粗体"));
    m_pacBold->setIcon(QIcon(c_textboldIcon));
    connect(m_pacBold, SIGNAL(triggered(bool)), this, SLOT(bold(bool)));

    m_pacItalic = pTBar->addAction(GLD_REICH_TEXT("斜体"));
    m_pacItalic->setIcon(QIcon(c_textItalic));
    connect(m_pacItalic, SIGNAL(triggered(bool)), this, SLOT(italic(bool)));

    m_pacUnderline = pTBar->addAction(GLD_REICH_TEXT("下划线"));
    m_pacUnderline->setIcon(QIcon(c_textUnderIcon));
    connect(m_pacUnderline, SIGNAL(triggered(bool)), this, SLOT(underline(bool)));

    m_pacStrikeOut = pTBar->addAction(GLD_REICH_TEXT("删除线"));
    m_pacStrikeOut->setIcon(QIcon(c_strThroughIcon));
    connect(m_pacStrikeOut, SIGNAL(triggered(bool)), this, SLOT(strikeOut(bool)));

    m_pacSub = pTBar->addAction(GLD_REICH_TEXT("下标"));
    m_pacSub->setIcon(QIcon(c_subScriptIcon));
    connect(m_pacSub, SIGNAL(triggered(bool)), this, SLOT(sub(bool)));

    m_pacSup = pTBar->addAction(GLD_REICH_TEXT("上标"));
    m_pacSup->setIcon(QIcon(c_superScriptIcon));
    connect(m_pacSup, SIGNAL(triggered(bool)), this, SLOT(sup(bool)));

    pTBar->addSeparator();
}

void GLDRichTextEditor::addAlignToToolBar(QToolBar *pTBar)
{
    m_pacAlignLeft = pTBar->addAction(GLD_REICH_TEXT("左"));
    m_pacAlignLeft->setIcon(QIcon(c_alignLeftIcon));
    connect(m_pacAlignLeft, SIGNAL(triggered(bool)), this, SLOT(alignLeft(bool)));

    m_pacAlignCenter = pTBar->addAction(GLD_REICH_TEXT("中"));
    m_pacAlignCenter->setIcon(QIcon(c_centerIcon));
    connect(m_pacAlignCenter, SIGNAL(triggered(bool)), this, SLOT(alignCenter(bool)));

    m_pacAlignRight = pTBar->addAction(GLD_REICH_TEXT("右"));
    m_pacAlignRight->setIcon(QIcon(c_alignRightIcon));
    connect(m_pacAlignRight, SIGNAL(triggered(bool)), this, SLOT(alignRight(bool)));

    pTBar->addSeparator();
}

void GLDRichTextEditor::addEditSetToToolBar(QToolBar *pTBar)
{
    m_pacInsertPic = pTBar->addAction(GLD_REICH_TEXT("插入图片"));
    m_pacInsertPic->setIcon(QIcon(c_insertPicIcon));
    connect(m_pacInsertPic, SIGNAL(triggered(bool)), this, SLOT(insertPic(bool)));

    m_pacInsertTable = pTBar->addAction(GLD_REICH_TEXT("插入表格"));
    m_pacInsertTable->setIcon(QIcon(c_insertTableIcon));
    connect(m_pacInsertTable, SIGNAL(triggered(bool)), this, SLOT(insertTable(bool)));

    pTBar->addSeparator();

    m_pacFind = pTBar->addAction(GLD_REICH_TEXT("查找"));
    m_pacFind->setIcon(QIcon(c_findIcon));
    connect(m_pacFind, SIGNAL(triggered(bool)), this, SLOT(onFind(bool)));

    m_pacReplace = pTBar->addAction(GLD_REICH_TEXT("替换"));
    m_pacReplace->setIcon(QIcon(c_replaceIcon));
    connect(m_pacReplace, SIGNAL(triggered(bool)), this, SLOT(onReplace(bool)));

    m_findAndReplace = new GLDRichTextEditFindAndReplaceWidget(this);

    connect(m_findAndReplace, SIGNAL(finded()), this, SLOT(onFinded()));
    connect(m_findAndReplace, SIGNAL(replaced()), this, SLOT(onReplaced()));
    connect(m_findAndReplace, SIGNAL(allReplaced()), this, SLOT(onAllReplaced()));
    connect(m_findAndReplace, SIGNAL(findAndReplaced()), this, SLOT(onFindAndReplaced()));

    pTBar->addSeparator();

    //test
#ifdef _DEBUG
    m_tableTest = new GLDRichTextEditTableValueTest(this);
    m_pacTableTest = pTBar->addAction(GLD_REICH_TEXT("测试"));
    m_pacTableTest->setIcon(QIcon(c_insertPicIcon));

    connect(m_pacTableTest, SIGNAL(triggered(bool)), this, SLOT(tested(bool)));
    connect(m_tableTest, SIGNAL(triggered()), this, SLOT(ontest()));

#endif
    pTBar->addSeparator();
}

void GLDRichTextEditor::addZoomSetToToolBar(QToolBar *pTBar)
{
    m_zoomIn = pTBar->addAction("zoom in");
    m_zoomIn->setIcon(QIcon(c_zoomInIcon));
    connect(m_zoomIn, SIGNAL(triggered()), this, SLOT(onZoomIn()));

    m_zoomOut = pTBar->addAction("zoom out");
    m_zoomOut->setIcon(QIcon(c_zoomOutIcon));
    connect(m_zoomOut, SIGNAL(triggered()), this, SLOT(onZoomOut()));

    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setRange(c_sliderMinPos, c_sliderMaxPos);
    m_slider->setValue(100);
    m_slider->setSingleStep(1);
    m_slider->setMaximumWidth(150);
    m_slider->setMinimumWidth(150);

    pTBar->addWidget(m_slider);
    connect(m_slider, SIGNAL(sliderMoved(int)), this, SLOT(onSetZoomFactor(int)));
}

void GLDRichTextEditor::initMenuAndToolBar()
{
    initMenuBar();

    QToolBar *pTBar = addMenuActionToToolBar();

    addFontSettingToToolBar(pTBar);

    addAlignToToolBar(pTBar);

    addEditSetToToolBar(pTBar);

    addZoomSetToToolBar(pTBar);
}

void GLDRichTextEditor::subWindowChanged(QMdiSubWindow * wnd)
{
    // 更新界面按钮状态
    Q_UNUSED(wnd)
}

void GLDRichTextEditor::newfile(bool)
{
    GLDRichTextEdit *pEdit = new GLDRichTextEdit(NULL);
    pEdit->setAttribute(Qt::WA_DeleteOnClose);
    m_pEditNew = pEdit;
    GLDRichEditScrollArea *m_pScrollArea = new GLDRichEditScrollArea(this, pEdit);

    QMdiSubWindow *pSubWnd = m_pArea->addSubWindow(m_pScrollArea);
    pSubWnd->setWindowIcon(QIcon(":/largePageBorders.png"));
    pSubWnd->showMaximized();
    m_pScrollArea->setWindowTitle(GLD_REICH_TEXT("新建文档"));
    pEdit->setProperty(GLD_RICH_TEXT_EDITOR_DOC_MARK, QVariant((qlonglong)pSubWnd));
    m_pTempScrollArea = m_pScrollArea;
    m_nState = STATE_NEW;
    m_pScrollArea->show();

    connect(pEdit, SIGNAL(activeWindow(GLDRichTextEdit *)), this, SLOT(onActiveEdit(GLDRichTextEdit *)));
    connect(pEdit, SIGNAL(updateUIState(GLDRichTextEdit *, bool)), this, SLOT(onUpdateUIState(GLDRichTextEdit *, bool)));
    connect(pEdit, SIGNAL(updateScroll()), m_pScrollArea, SLOT(updateUISlot()));

}

void GLDRichTextEditor::open(bool)
{
    QFileInfo flIn(QFileDialog::getOpenFileName(this, GLD_REICH_TEXT("选择文件"), QString(), GLD_REICH_TEXT("*.rtf")));
    if (!flIn.isFile() || 0 != flIn.suffix().compare("rtf", Qt::CaseInsensitive))
    {
        return;
    }
    GLDRichTextEdit *pEdit = new GLDRichTextEdit(NULL);
    pEdit->setAttribute(Qt::WA_DeleteOnClose);
    pEdit->open(flIn.absoluteFilePath());
    m_pEditOpen = pEdit;
    GLDRichEditScrollArea *m_pScrollArea = new GLDRichEditScrollArea(this, pEdit);

    QMdiSubWindow *pSubWnd = m_pArea->addSubWindow(m_pScrollArea);
    pSubWnd->showMaximized();
    pEdit->setProperty(GLD_RICH_TEXT_EDITOR_DOC_MARK, QVariant((qlonglong)pSubWnd));
    m_pTempScrollArea = m_pScrollArea;
    m_nState = STATE_OPEN;
    m_pScrollArea->show();

    connect(pEdit, SIGNAL(activeWindow(GLDRichTextEdit *)), this, SLOT(onActiveEdit(GLDRichTextEdit *)));
    connect(pEdit, SIGNAL(updateUIState(GLDRichTextEdit *, bool)), this, SLOT(onUpdateUIState(GLDRichTextEdit *, bool)));
    connect(pEdit, SIGNAL(updateScroll()), m_pScrollArea, SLOT(updateUISlot()));
}

void GLDRichTextEditor::save(bool)
{
    QMdiSubWindow *pSubWnd = m_pArea->activeSubWindow();

    if (NULL == pSubWnd)
    {
        return;
    }

    GLDRichEditScrollArea *pScrollArea = qobject_cast<GLDRichEditScrollArea *>(pSubWnd->widget());
    GLDRichTextEdit *pEdit = qobject_cast<GLDRichTextEdit *>(pScrollArea->widget());

    if (NULL != pEdit)
    {
        pEdit->save();
        pSubWnd->setWindowTitle(pEdit->getTextDocTitle());
        pEdit->setCursorFocus();
    }
}

void GLDRichTextEditor::saveAs(bool)
{
    QMdiSubWindow *pSubWnd = m_pArea->activeSubWindow();
    if (NULL == pSubWnd)
    {
        return;
    }

    GLDRichEditScrollArea *pScrollArea = qobject_cast<GLDRichEditScrollArea *>(pSubWnd->widget());
    GLDRichTextEdit *pEdit = qobject_cast<GLDRichTextEdit *>(pScrollArea->widget());

    if (NULL != pEdit)
    {
        QString sDoc;

        int nWordAppType = 0;
        if (checkInstallOfficeWord())
        {
            nWordAppType = 1;
        }
        else if (checkInstallWpsWord())
        {
            nWordAppType = 2;
        }
        if (0 != nWordAppType)
        {
            sDoc = ";;Office Word (*.doc)";
        }

        QString sSelectedFilter; 
        QFileInfo flIn(QFileDialog::getSaveFileName(this, GLD_REICH_TEXT("另存文件"), GLD_REICH_TEXT("./未命名.rtf"),
                       GLD_REICH_TEXT("文件 (*.rtf);;文件 (*.txt)%1").arg(sDoc), &sSelectedFilter));
        QString sFile = flIn.absoluteFilePath();

        if (GLD_REICH_TEXT("文件 (*.rtf)") == sSelectedFilter)
        {
            if (!sFile.endsWith(".rtf", Qt::CaseInsensitive))
            {
                sFile += ".rtf";
            }

            pEdit->saveAs(sFile, true);
            pSubWnd->setWindowTitle(pEdit->getTextDocTitle());
        }
        else if (GLD_REICH_TEXT("文件 (*.txt)") == sSelectedFilter)
        {
            if (!sFile.endsWith(".txt", Qt::CaseInsensitive))
            {
                sFile += ".txt";
            }

            QFile fl(sFile);
            if (fl.open(QIODevice::WriteOnly))
            {
                QTextStream ts(&fl);
                ts << pEdit->getAllText();
                fl.close();
            }
        }
        else if (GLD_REICH_TEXT("Office Word (*.doc)") == sSelectedFilter)
        {
            if (!sFile.endsWith(".doc", Qt::CaseInsensitive))
            {
                sFile += ".doc";
            }

            if (pEdit->getFileName().isEmpty())
            {
                // 临时文件目录
                QString tempFile;
                QDir dirTemp = QDir::temp();
                QString uid = QUuid::createUuid().toString().replace("-", "_");
                uid.chop(1);
                uid.remove(0, 1);
                tempFile = dirTemp.absoluteFilePath(uid + ".rtf");
                pEdit->saveAs(tempFile, false);
                
                saveToDoc(tempFile, sFile, nWordAppType);
                QFile::remove(tempFile);
            }
            else
            {
                pEdit->save();
                saveToDoc(pEdit->getFileName(), sFile, nWordAppType);
            }
        }
        else
        {
            pEdit->setCursorFocus();
            return;
        }
       pEdit->setCursorFocus();
    }
}

void GLDRichTextEditor::undo(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if ((NULL != pEdit) && (pEdit->canUndo()))
    {
        pEdit->undo();
        m_pacRedo->setEnabled(true);
    }
    else
    {
        m_pacUndo->setEnabled(false);
    }
}

void GLDRichTextEditor::redo(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if ((NULL != pEdit) && (pEdit->canRedo()))
    {
        pEdit->redo();
        m_pacUndo->setEnabled(true);
    }
    else
    {
        m_pacRedo->setEnabled(false);
    }
}

void GLDRichTextEditor::cut(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->cut();
    }
}

void GLDRichTextEditor::copy(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->copy();
    }
}

void GLDRichTextEditor::paste(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->paste();
    }
}

void GLDRichTextEditor::onSetfont(const QFont & fnt)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->setFont(fnt);
    }
}

void GLDRichTextEditor::onsetSize(const QString & fntSize)
{
    int nSize = fntSize.toInt();

    if (0 >= nSize)
    {
        return;
    }

    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->setFontSize(nSize);
    }
}

void GLDRichTextEditor::onSetColor(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        QColor clr = QColorDialog::getColor();
        pEdit->setFontColor(clr);
    }
}

void GLDRichTextEditor::onSetBackground(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        QColor clr = QColorDialog::getColor();
        pEdit->setFontBackgroundColor(clr);
    }
}

void GLDRichTextEditor::bold(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->bold();
    }
}

void GLDRichTextEditor::italic(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->italic();
    }
}

void GLDRichTextEditor::underline(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->underline();
    }
}

void GLDRichTextEditor::strikeOut(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->strikeout();
    }
}

void GLDRichTextEditor::sub(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->subscript();
    }
}

void GLDRichTextEditor::sup(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        pEdit->superscript();
    }
}

void GLDRichTextEditor::alignLeft(bool b)
{
    Q_UNUSED(b);
    {
        GLDRichTextEdit *pEdit = currentEdit();

        if (NULL != pEdit)
        {
            pEdit->setAlign(GLDRichTextEdit::Left);
        }
    }
}

void GLDRichTextEditor::alignCenter(bool b)
{
    Q_UNUSED(b);
    {
        GLDRichTextEdit *pEdit = currentEdit();

        if (NULL != pEdit)
        {
            pEdit->setAlign(GLDRichTextEdit::Center);
        }
    }
}

void GLDRichTextEditor::alignRight(bool b)
{
    Q_UNUSED(b);
    {
        GLDRichTextEdit *pEdit = currentEdit();

        if (NULL != pEdit)
        {
            pEdit->setAlign(GLDRichTextEdit::Right);
        }
    }
}

void GLDRichTextEditor::insertPic(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        QFileInfo flIn(QFileDialog::getOpenFileName(this, GLD_REICH_TEXT("选择图片文件"), QString(),
                       GLD_REICH_TEXT("Bmp (*.bmp);;Jpg (*.jpg);; Jpeg (*.jpeg);; Png(*.png)")));
        if (!flIn.isFile())
        {
            pEdit->setCursorFocus();
            return;
        }
        QString sSuffix = flIn.suffix();
        bool bContinue = false;
        if (0 == sSuffix.compare("bmp", Qt::CaseInsensitive)
            || 0 == sSuffix.compare("jpg", Qt::CaseInsensitive)
            || 0 == sSuffix.compare("jpeg", Qt::CaseInsensitive)
            || 0 == sSuffix.compare("gif", Qt::CaseInsensitive)
            || 0 == sSuffix.compare("png", Qt::CaseInsensitive))
        {
            bContinue = true;
        }
        if (!bContinue)
        {
            return;
        }
        pEdit->insertImage(flIn.absoluteFilePath(), 0, 0, false);
    }
}

void GLDRichTextEditor::insertTable(bool)
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        GLDRichTextEditAddTableDlg dlg;
        if (QDialog::Accepted == dlg.exec())
        {
            int nRows = dlg.rows();
            int nColumns = dlg.columns();
            pEdit->insertTable(nRows, nColumns);
        }
        else
        {
            pEdit->setCursorFocus();
        }
    }
}

void GLDRichTextEditor::onFind(bool)
{
    if (NULL != m_findAndReplace)
    {
        m_findAndReplace->setWindowTitle(GLD_REICH_TEXT("查找"));
        m_findAndReplace->setFind();
        m_findAndReplace->show();
    }
}

void GLDRichTextEditor::onReplace(bool)
{
    if (NULL != m_findAndReplace)
    {
        m_findAndReplace->setWindowTitle(GLD_REICH_TEXT("替换"));
        m_findAndReplace->setReplace();
        m_findAndReplace->show();
    }
}

void GLDRichTextEditor::onFinded()
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        QString text = m_findAndReplace->getFindText();
        bool bWord = m_findAndReplace->isWord();
        bool bCase = m_findAndReplace->isCase();
        bool bNext = m_findAndReplace->isNext();

        pEdit->findText(text, bCase, bWord, bNext);
    }
}

void GLDRichTextEditor::onReplaced()
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        QString sFindText = m_findAndReplace->getFindText();
        QString replaceText = m_findAndReplace->getReplaceText();
        bool bWord = m_findAndReplace->isWord();
        bool bCase = m_findAndReplace->isCase();
        bool bNext = m_findAndReplace->isNext();

        pEdit->replaceSelectText(sFindText, bCase, bWord, bNext, replaceText);
    }
}

void GLDRichTextEditor::onAllReplaced()
{
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        QString sFindText = m_findAndReplace->getFindText();
        QString replaceText = m_findAndReplace->getReplaceText();
        bool bWord = m_findAndReplace->isWord();
        bool bCase = m_findAndReplace->isCase();

        pEdit->replaceAllText(sFindText, bCase, bWord, replaceText);
    }
}

void GLDRichTextEditor::tested(bool)
{
#ifdef _DEBUG
    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL == pEdit)
    {
        return;
    }

    QByteArray by;
    QFile fl("d:/123.rtf");

    if (fl.open(QIODevice::ReadOnly))
    {
        by = fl.readAll();
    }

    fl.close();

    pEdit->readFromBuf(by);
    
    return;

    if (NULL != m_tableTest)
    {
        m_tableTest->show();
    }

    QString initText = m_tableTest->getInitText();
    pEdit->selectRegionText(initText, initText, false);

#endif
}

void GLDRichTextEditor::ontest()
{
#ifdef _DEBUG
    {
        QList<QMdiSubWindow *> lst = m_pArea->subWindowList();

        newfile(false);
        GLDRichTextEdit *pEdit = currentEdit();
        pEdit->insertTitleHead(QString::fromStdWString(L"合并文档"), 1, GLDRichTextEdit::Center);

        for (int i = 0; i < lst.size(); ++i)
        {
            GLDRichTextEdit *p = qobject_cast<GLDRichTextEdit *>(lst.at(i)->widget());
            pEdit->insertTitleHead(lst.at(i)->windowTitle(), 2, GLDRichTextEdit::Center);
            QPoint curTextCur = p->getTextCursor();
            p->selectAll();
            p->copy();
            p->setTextCursor(curTextCur);
            pEdit->paste();
            pEdit->moveTextCursorNext();
        }

        return;
    }

    GLDRichTextEdit *pEdit = currentEdit();

    if (NULL != pEdit)
    {
        int type = m_tableTest->getType();
        int tableRow = m_tableTest->row();
        int tableCol = m_tableTest->column();
        int table = m_tableTest->getTable();
        QString initText = m_tableTest->getInitText();

        pEdit->insertTitleHead(m_tableTest->getText(), tableRow, (GLDRichTextEdit::Align)tableCol);
        return;

        bool bFind = false;
        int tableIndex = 0;
        int textRowIndex = 0;
        GLDRichTextEdit::TextRowIterator start, end;
        while ((pEdit->getSeriesTableIterator(start, end, textRowIndex)))
        {
            textRowIndex = end.row();
            ++tableIndex;
            if (tableIndex == table)
            {
                bFind = true;
                break;
            }
            else if (tableIndex >= table)
            {
                break;
            }
        }
        Q_ASSERT(start <= end);

        if (0 == type)
        {
            if (bFind)
            {
                bFind = false;
                int i = 0;
                
                while (start != end)
                {
                    ++i;
                    if (i == tableRow)
                    {
                        bFind = true;
                        break;
                    }
                    ++start;
                }
                if (bFind)
                {
                    QString text = start.getText(tableCol);
                    m_tableTest->setText(text);
                }
            }
        }
        else if (1 == type)
        {
            if (bFind)
            {
                bFind = false;
                int i = 0;
                while (start != end)
                {
                    ++i;
                    if (i == tableRow)
                    {
                        bFind = true;
                        break;
                    }
                    ++start;
                }
                if (bFind)
                {
                    QString text = m_tableTest->getText();
                    start.setText(tableCol, text);
                }
            }
        }
        else if (2 == type)
        {
            if (bFind)
            {
                while (start != end)
                {
                    int cellCount = start.cellCount();
                    for (int i = 1; i <= cellCount; ++i)
                    {
                        start.setText(i, initText);
                    }
                    ++start;
                }
            }
        }
    }
#endif
}

void GLDRichTextEditor::onSetReadOnly(const int value)
{
    GLDRichTextEdit *richEdit = currentEdit();

    if (NULL != richEdit)
    {
        bool bReadOnly = value == 1 ? true : false;
        richEdit->setReadOnly(bReadOnly);
    }
}

void GLDRichTextEditor::onZoomIn()
{
    GLDRichTextEdit *richEdit = currentEdit();

    if (NULL != richEdit)
    {
        richEdit->setZoomFactor(true);
    }

}

void GLDRichTextEditor::onZoomOut()
{
    GLDRichTextEdit *richEdit = currentEdit();

    if (NULL != richEdit)
    {
        richEdit->setZoomFactor(false);
    }

}

void GLDRichTextEditor::onSetZoomFactor(int nPos)
{
    GLDRichTextEdit *richEdit = currentEdit();

    if (NULL != richEdit)
    {
        // 转化成放大比例
        richEdit->setZoomFactorValue( nPos * 1.0 / 100);
        richEdit->emitUpdateScroll();
    }
}

void GLDRichTextEditor::onActiveEdit(GLDRichTextEdit * edit)
{
    Q_ASSERT(NULL != edit);
    qlonglong supptr = edit->property(GLD_RICH_TEXT_EDITOR_DOC_MARK).toLongLong();

    if (0 != supptr)
    {
        QMdiSubWindow *pWd = (QMdiSubWindow *)supptr;
        if (pWd != m_pArea->currentSubWindow())
        {
            m_pArea->setActiveSubWindow(pWd);
        }
        pWd->setFocus();
    }
}

void GLDRichTextEditor::onUpdateUIState(GLDRichTextEdit * edit, bool bMousePress)
{
    Q_ASSERT(NULL != edit);
    QFont fnt = edit->selectionCharFont(bMousePress);

    if (fnt.family() != m_pCmbFont->currentFont().family())
    {
        disconnect(m_pCmbFont, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onSetfont(const QFont &)));
        m_pCmbFont->setCurrentFont(fnt);
        connect(m_pCmbFont, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onSetfont(const QFont &)));

        disconnect(m_pCmbSize, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onsetSize(const QString &)));
        int nIndex = m_pCmbSize->findText(QString::number(fnt.pointSize()));
        if (-1 != nIndex)
        {
            m_pCmbSize->setCurrentIndex(nIndex);
        }
        else
        {
            m_pCmbSize->setCurrentText(QString::number(fnt.pointSize()));
        }
        connect(m_pCmbSize, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onsetSize(const QString &)));
    }

    if (edit->canUndo())
    {
        m_pacUndo->setEnabled(true);
    }
    else
    {
        m_pacUndo->setEnabled(false);
    }
    edit->setCursorFocus();
}

void GLDRichTextEditor::onZoomFactorChanged()
{
    GLDRichTextEdit *richEdit = currentEdit();

    if (NULL != richEdit)
    {
        m_slider->setValue(richEdit->zoomFactor() * 100);
    }
}

void GLDRichTextEditor::onTheRButtonUp()
{
     GLDRichTextEdit *richEdit = currentEdit();

     if (NULL != richEdit)
     {
        richEdit->addRightBtnAction();
     }
}

void GLDRichTextEditor::onFindAndReplaced()
{
    GLDRichTextEdit *richEdit = currentEdit();

    if (NULL != richEdit)
    {
         richEdit->setCursorFocus();
    }
}

void GLDRichTextEditor::closeEvent(QCloseEvent *e)
{
    int nState = 0;

    QList<QMdiSubWindow *> lst = m_pArea->subWindowList();
    while (!lst.isEmpty())
    {
        QMdiSubWindow *pWnd = lst.takeLast();
        Q_ASSERT(NULL != pWnd);
        QScrollArea *pWidget = qobject_cast<QScrollArea *>(pWnd->widget());
        GLDRichTextEdit *pEdit = qobject_cast<GLDRichTextEdit *>(pWidget->widget());
        Q_ASSERT(NULL != pEdit);

        if (1 == nState)
        {
            if (pEdit->isModify())
            {
                pEdit->save();
            }
        }
        else if (-1 == nState)
        {
            pEdit->setModify(false);
        }
        else if (-2 == nState)
        {
            break;
        }
        else
        {
            if (pEdit->isModify())
            {
                QMessageBox::Button btn = QMessageBox::warning(this,
                    QString::fromStdWString(L"提示"),
                    QString::fromStdWString(L"文件已修改，是否保存？"),
                    QMessageBox::Yes | QMessageBox::YesAll | QMessageBox::No
                                 | QMessageBox::NoAll | QMessageBox::Cancel);

                switch (btn)
                {
                case QMessageBox::Yes:
                    pEdit->save();
                    break;
                case QMessageBox::YesAll:
                    nState = 1;
                    pEdit->save();
                    break;
                case QMessageBox::No:
                    break;
                case QMessageBox::NoAll:
                    nState = -1;
                    break;
                case QMessageBox::Cancel:
                    e->ignore();
                    nState = -2;
                    break;
                default:
                    break;
                }
            }
        }
        
        if (-2 != nState)
        {
            pEdit->setModify(false);
            pEdit->close();
        }
    }
    
    if (-2 != nState)
    {
        QMainWindow::closeEvent(e);
    }
}

GLDRichTextEdit * GLDRichTextEditor::currentEdit()
{
    QMdiSubWindow *pSubWnd = m_pArea->activeSubWindow();

    if (NULL == pSubWnd)
    {
        return NULL;
    }
    QScrollArea *pWidget = qobject_cast<QScrollArea *>(pSubWnd->widget());

    return qobject_cast<GLDRichTextEdit *>(pWidget->widget());
}

void GLDRichTextEditor::saveToDoc(const QString & rtfFileName, const QString & docFileName, int docAppType)
{
    switch (docAppType)
    {
    case 1:
    {
          QAxObject word("Word.Application", NULL);
          word.setProperty("Visible", false);
          QAxObject *documents = word.querySubObject("Documents");

          if (NULL == documents)
          {
              break;
          }

          documents->querySubObject("Open(QString&)", rtfFileName);
          QAxObject *document = word.querySubObject("ActiveDocument");
          Q_ASSERT(NULL != document);

          QVariantList saveAsParam;
          saveAsParam.append(docFileName);
          saveAsParam.append(0);
          document->dynamicCall("SaveAs(QVariant&, QVariant&)", saveAsParam);          
          word.dynamicCall("Quit()");

          delete document;
          delete documents;
    }
        break;

    default:
        break;
    }
}

void GMdiArea::mousePressEvent(QMouseEvent *event)
{

}
