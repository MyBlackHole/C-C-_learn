/*!
 * \file GLDRichTextEdit.h
 *
 * \author xg
 * \date 八月 2014
 *
 * 
 */
#ifndef GLDRICHTEXTEDIT_H
#define GLDRICHTEXTEDIT_H

#include <QEvent>
#include <QFrame>
#include <QMenu>
#include <QScrollBar>
#include <QScrollArea>
#include <QWidget>
#include <QResizeEvent>

class GLDRichTextEditPrivate;
class GLDRichEditScrollArea;
class GLDRichTextEditor;

static bool g_bResize = true;

class GLDRichTextEdit : public QWidget
{
    Q_OBJECT

public:
    enum Align
    {
        Left,
        Center,
        Right,
        Justify
    };

    // 文本行迭代器
    friend class TextRowIterator;
    class TextRowIterator
    {
        friend class GLDRichTextEdit;
    public:
        TextRowIterator();
        explicit TextRowIterator(GLDRichTextEdit * edit, int row);
        ~TextRowIterator();

        bool isVaild() const;

        QString getText(int cell) const;
        void setText(int cell, const QString & text);
        bool hasCell() const;
        int cellCount() const;

        TextRowIterator next() const;

        int row() const
        {
            return m_row;
        }

        bool operator==(const TextRowIterator & o)
        {
            return (m_row == o.m_row);
        }

        bool operator!=(const TextRowIterator & o)
        {
            return (m_row != o.m_row);
        }

        bool operator<=(const TextRowIterator & o)
        {
            return (m_row <= o.m_row);
        }

        int operator++(int)
        {
            return m_row++;
        }

        int operator++()
        {
            return ++m_row;
        }

        int operator--(int)
        {
            return m_row--;
        }

        int operator--()
        {
            return --m_row;
        }

    private:
        GLDRichTextEdit *m_edit;
        int              m_row;
    };

public:
    GLDRichTextEdit(QWidget *parent);
    ~GLDRichTextEdit();

    void alwaysHideScrollBar();
    int contentHeight();
    int contentWidth();

    bool canUndo();
    bool canRedo();

    void open(const QString &fileName);
    int save(); // 返回0表示成功
    void saveAs(const QString &fileName, bool userSaveAsFileName);

    /**
     * @brief saveToBuf 保存到二进制流
     * @param byteArray 二进制流
     * @return 1--成功 0--失败
     */
    int saveToBuf(QByteArray &byteArray);

    /**
     * @brief readFromBuf 读二进制流到文件
     * @param byteArray 二进制流
     * @return 1--成功 0--失败
     */
    int readFromBuf(QByteArray &byteArray);


    bool isModify() const;
    void setModify(bool modified);

    void setDefaultFont(const QFont &fnt, int size, const QColor &clr);
    QString getSelectText() const;

	QString getAllText() const;

    const QString &getFileName() const;

    QFont selectionCharFont(bool bMousePress) const;
    QColor selectionCharFontColor() const;
    QColor selectionCharBackgroundColor() const;

    QPoint getTextCursor() const;
    void setTextCursor(const QPoint &pos);

    int getCurrentRow() const;
    void moveTextCursorNext();

    /*!
    * @brief    返回行数 [1, n]闭区间
    * @return   如果仅有一行切第一行没有数据返回0
    */
    int getLineCount() const;

    /*!
     * @brief    行迭代器
     * @param    [i]textRowIndex 文本行号
     * @return   GLDRichTextEditRow * 返回行索引
     * 注意      仅针对当前行
     */
    TextRowIterator getRowIterator(_In_ int textRowIndex);

     /*!
      * @brief    获取连续的表起始位置和结束 [start, end)
      * @param    [i]start 返回连续表的起始位置
      * @param    [i]end   返回连续表的结束位置
      * @param    [i]textRowIndex 文本行索引
      * @return   bool 如果有table行 返回true否则返回false
      */
     bool getSeriesTableIterator(_Out_ TextRowIterator &start, _Out_ TextRowIterator &end,
                                 _In_ int textRowIndex = 0);

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
     bool replaceSelectText(const QString &find, bool bCase, bool bWord,
                            bool bNext, const QString &replace, bool findAndImReplace);

     /*!
     * @brief    查找替换
     * @param    [i]find 查找的字符串
     * @param    [i]isCase 是否大写
     * @param    [i]isWord 整个单词
     * @param    [i]isNext 向下
     * @param    [i]replace 替换图片
     * @param    [i]findAndImReplace 找到是否马上替换
     * @return   替换成功返回true
     */
     bool replaceSelectTextToImage(const QString &find, bool isCase, bool isWord,
                                   bool isNext, const QString &imageFileName,
                                   bool findAndImReplace, int w, int h, bool bMonochrome);

     /*!
      * @brief    插入标题
      * @param    [i]title 标题文本
      * @param    [i]Num   标题号
      * @param    [i]align 对齐方式
      * @return   void
      */
     void insertTitleHead(const QString &titleText, int num, GLDRichTextEdit::Align align);

     QString getTextDocTitle() const;
     void addRightBtnAction();
     void initRBtnMenu();

     void saveAsDoc();    
     void saveToDoc(QString desDocName, QString srcFileName);

     double zoomFactor() const;

     void emitRButtonUp();
     void emitUpdateScroll();
     void setCursorFocus();
     void enterKeyPress();
     void enterKeyPressed();

     virtual void closeEvent(QCloseEvent *e);

signals:
     void activeWindow(GLDRichTextEdit *edit);
     void updateUIState(GLDRichTextEdit *edit, bool bMousePress);
     void onRButtonUp();
     void zoomFactorChanged();
     void updateScroll();
     void updateFocus(GLDRichTextEdit *edit);

public slots:
    void open();
    void onSave();
    void saveAs();

    void catchWheelup();
    void cut();
    void copy();
    void paste();
    void del();
    void undo();
    void redo();
    void setUndoCount(int undoCount);

    void selectAll();

    void insertText(const QString &text);
    void insertTable(int rows, int columns);
    void insertImage(const QString &imageFileName, int w, int h, bool bMonochrome);

    void bold();
    void setBold(bool enable);

    void italic();
    void setItalic(bool enable);

    void underline();
    void setUnderline(bool enable);

    void strikeout();
    void setStrikeout(bool enable);

    void subscript();
    void setSubscript(bool enable);

    void superscript();
    void setSuperscript(bool enable);

    void setFont(const QFont &fnt);
    void setFontSize(int size);
    void setFontColor(const QColor &clr);
    void setFontBackgroundColor(const QColor &color);

    void setReadOnly(bool value);

    // 支持缩放比例在 10% ~ 600% 之间
    void setZoomFactor(bool bIsZoomIn);

    void setZoomFactorValue(const double &zoomFactor);

    void setAlign(Align align);

    bool findText(const QString &find, bool bCase, bool bWord, bool bNext);
    void replaceSelectText(const QString &find, bool bCase, bool bWord, bool bNext, const QString &replace);
    void replaceAllText(const QString &find, bool bCase, bool bWord, const QString &replace);
    void replaceSelectTextToImage(const QString &find, bool bCase, bool bWord, bool bNext,
                                  const QString &imageFileName, int w, int h, bool bMonochrome);

    void setTextCursorToHead();
    void setTextCursorToTail();
    
    /*!
     * @brief    选择中beginText到endText的文本包括beginText, endText
     * @param    [i]beginText
     * @param    [i]endText
     , @param    [i]bCase 是否区分大小写
     * @return   bool
     */
    bool selectRegionText(const QString &beginText, const QString &endText, bool bCase);
    void test();
    
protected:
    virtual QString getOpenFileName() const;
    virtual QString getSaveFileName(const QString &defaultName) const;
    virtual void resizeEvent(QResizeEvent *e);


private:
    friend class GLDRichTextEditNativeEventFilter;
    void updateUI(QEvent::Type eType, Qt::MouseButtons btn, Qt::Key eKey);

public:
    GLDRichTextEditPrivate *m_d;
    int m_nScrollWidth;

private:
    double m_dZoomFactor;//比例的缩放
    static double s_PreZoom;
    bool m_bCanUndo;
    bool m_bCanRedo;
    QScrollBar *m_pVerticalScrollBar;
    int m_nCurrentScrollBarPos;
    QMenu *m_pMenu;
    QAction *m_pMenuCut;
    QAction *m_pMenuCopy;
    QAction *m_pMenuPaste;

};

class GLDRichEditScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit GLDRichEditScrollArea(QWidget* parent = 0,
                                   GLDRichTextEdit *richEdit = NULL);
    void setContentHeight();

public slots:
    void updateUISlot();

protected:
    void resizeEvent(QResizeEvent *e);
    void closeEvent(QCloseEvent *e);

private:
    bool m_bIsResize;
    int m_nContentHeight;
    int m_nScrollAreaHeight;
    int m_nScrollAreaWidth;
    GLDRichTextEdit *m_pRichEditObject;
    GLDRichTextEditPrivate *m_pRichEditPrivate;
};
#endif // GLDRICHTEXTEDIT_H
