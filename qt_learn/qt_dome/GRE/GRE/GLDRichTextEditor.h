#ifndef GLDRICHTEXTEDITOR_H
#define GLDRICHTEXTEDITOR_H

#include <QtWidgets/QMainWindow>

#include "ui_GLDRichTextEditor.h"

#include <QMdiArea>
class QComboBox;
class QFontComboBox;
class GLDRichTextEdit;
class GLDRichTextEditFindAndReplaceWidget;
class GLDRichTextEditorUpdatUIEvent;
class QMdiArea;
class QMdiSubWindow;
class GLDRichEditScrollArea;
class GMdiArea;

#ifdef _DEBUG
    class GLDRichTextEditTableValueTest;
#endif

class GLDRichTextEditor : public QMainWindow
{
    Q_OBJECT

public:
    GLDRichTextEditor(QWidget *parent = 0);
    ~GLDRichTextEditor();

    void initMenuAndToolBar();
    void initMenuBar();

    QToolBar * addMenuActionToToolBar();
    void addFontSettingToToolBar(QToolBar *pTBar);
    void addAlignToToolBar(QToolBar *pTBar);
    void addEditSetToToolBar(QToolBar *pTBar);
    void addZoomSetToToolBar(QToolBar *pTBar);

private:
    void init();

private slots:
    void subWindowChanged(QMdiSubWindow * wnd);

    void newfile(bool);
    void open(bool);
    void save(bool);
    void saveAs(bool);

    void undo(bool);
    void redo(bool);
    void cut(bool);
    void copy(bool);
    void paste(bool);

    void onSetfont(const QFont & fnt);
    void onsetSize(const QString & fntSize);
    void onSetColor(bool);
    void onSetBackground(bool);

    void bold(bool);
    void italic(bool);
    void underline(bool);
    void strikeOut(bool);
    void sub(bool);
    void sup(bool);

    void alignLeft(bool);
    void alignCenter(bool);
    void alignRight(bool);

    void insertPic(bool);
    void insertTable(bool);

    void onFind(bool);
    void onReplace(bool);
    void onFinded();
    void onReplaced();
    void onAllReplaced();

    void tested(bool);
    void ontest();
    void onSetReadOnly(const int value);
    void onZoomIn();
    void onZoomOut();

    //根据滑块设置缩放比例
    void onSetZoomFactor(int nPos);

    void onActiveEdit(GLDRichTextEdit * edit);
    void onUpdateUIState(GLDRichTextEdit * edit, bool bMousePress);
    void onZoomFactorChanged();
    void onTheRButtonUp();
    void onFindAndReplaced();

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    GLDRichTextEdit * currentEdit();

    /*!
     * @brief    将rtf保存为doc
     * @param    [i]rtfFileName rtf文件名
     * @param    [i]docFileName 
     * @param    [i]docAppType
     * @return   void
     */
     void saveToDoc(const QString & rtfFileName, const QString & docFileName, int docAppType);
     int m_nState;
     GLDRichTextEdit *m_pEditOpen;
     GLDRichTextEdit *m_pEditNew;

private:
    Ui::GLDRichTextEditorClass ui;

private:
    GMdiArea *m_pArea;

    QAction *m_pacNew;
    QAction *m_pacOpen;
    QAction *m_pacSave;
    QAction *m_pacSaveAs;

    QAction *m_pacUndo;
    QAction *m_pacRedo;
    QAction *m_pacCut;
    QAction *m_pacCopy;
    QAction *m_pacPaste;

    QFontComboBox *m_pCmbFont;
    QComboBox *m_pCmbSize;
    QComboBox *m_pReadOnlyCmb;
    QAction *m_pacBackgroundColor;
    QAction *m_pacColor;

    QAction *m_pacBold;
    QAction *m_pacItalic;
    QAction *m_pacUnderline;
    QAction *m_pacStrikeOut;
    QAction *m_pacSub;
    QAction *m_pacSup;

    QAction *m_pacAlignLeft;
    QAction *m_pacAlignCenter;
    QAction *m_pacAlignRight;

    QAction *m_pacInsertPic;
    QAction *m_pacInsertTable;

    QAction *m_readOnly;

    QAction *m_pacFind;
    QAction *m_pacReplace;

    GLDRichTextEditFindAndReplaceWidget *m_findAndReplace;

    GLDRichTextEdit *m_pEdit;
    GLDRichEditScrollArea *m_pTempScrollArea;



#ifdef _DEBUG
    GLDRichTextEditTableValueTest *m_tableTest;
    QAction                       *m_pacTableTest;
#endif

    QAction *m_zoomIn;  // 放大
    QAction *m_zoomOut; // 缩小
    QSlider *m_slider;
};

class GMdiArea : public QMdiArea
{
protected:
    void mousePressEvent(QMouseEvent * event);
};

#endif // GLDRICHTEXTEDITOR_H
