/****************************************************************************************************

  报告编辑

  作者: jiangjb 2015-03-06
  备注: 报告编辑
  审核:

  Copyright (c) 1994-2014 Glodon Corporation

****************************************************************************************************/
#ifndef GSHANALYSEEDITPREVIEWFRAME_H
#define GSHANALYSEEDITPREVIEWFRAME_H

#include "GLDFrame.h"

#include "GLDFrame.h"
#include "GLDTypes.h"
#include "GSPCore.h"
#include "GBQModelConsts.h"

class QVBoxLayout;
class QToolButton;
class QTextCursor;
class QSignalMapper;
class QClipboard;
class QTextDocument;
class QFontComboBox;
class QSlider;
class GLDSpinBox;
class GLDComboBox;
class GLDTextEdit;
class GLDAction;
class GLDRichTextEdit;
class GLDRichEditScrollArea;
class GColorListEx;

class IGBQJobService;
class IGBQBidService;

class GSHAnalyseEditPreviewFrame : public GLDFrame
{
    Q_OBJECT
public:
    GSHAnalyseEditPreviewFrame();
    virtual ~GSHAnalyseEditPreviewFrame();

    void generateActionLog(GTime startTime, GTime endTime, GString actionName);
protected:
    virtual void initialize();
    virtual void activate();
    virtual void deactivate();
    virtual IGLDFrameLayout *createFrameLayout();
    virtual IGLDFrameNavigator *createFrameNavigator();

    virtual IGLDToolBar *createToolBar();
    virtual void updateActions();
    virtual void updateActionsReadonlyExpt();

private:
    enum GBQModelType
    {
        MTBid = 0,
        MTProject
    };

    enum GBQMenuIndex
    {
        MIFirst = 1,
        MISecond
    };

    enum GBQIconName
    {
        ginUndo,
        ginRedo,
        BtnEdit,
        BtnCut,
        BtnCopy,
        BtnPaste,
        BtnBold,
        BtnItalic,
        BtnUnderline,
        ginStrikeout,
        ginSub,
        ginSup,
        BtnLeft,
        BtnCenter,
        BtnRight,
        ginZoomOut,
        ginSlider,
        ginZoomIn,
        ginAutoWidth,
        ginBackgroundColor,
        ginFontColor
    };

    void addMacroToPopMenu(); //将宏插入右键菜单
    void addMacroItem(const GMap<GString, GString> &macroList, const GString &parentURL);

    void getMacrosInContent(const GString &content, GStrings &macros); // 获取所有宏
    void getMacroValue(GString &macro, const GStrings &macroList); // 不带宏，不带格式的内容
    void macroValueWithPattern(GString &macro, const GStrings &macroList); // 不带宏，带格式的内容
    GString getValueByMacro(const GString &macro); // 根据宏名获取对应的值

    void attachPlugins();
    void detachPlugins();

    void initData();
    void createItems();
    void initLayout();
    void createConnects();

    void createPopMenu();
    void insertIntoMenu(GLDAction *action, const GString &parentURL = "");

    void loadPreviewData();
    void loadContentData();

    void setIcons(GBQIconName btn, bool pressed = false);
    void setItemsEnable();

    void getMacroValues();
    void showProjectInfoMacro();
    void showProjectCharaterMacro();
    void showSysDefMacro();
    void showAllCustomMacro();
    void doShowCustomItem(GSPRecord customMacroItem);
    bool needShowField(const GString &fieldName, bool hasMeasure);
    bool getMeasureItemEnable();
    void savePreviewContents();

public slots:
    void onDoubleClickedInsertMacro(GSPRecord record);
    void onUpdateDefCustomMacros(const GMap<GString, GString> &macroValueHash);

private slots:
    void undo(bool);
    void redo(bool);
    void cut(bool);
    void copy(bool);
    void paste(bool);

    void onSetfont(const QFont &fnt);
    void onSetSize(int size);
    void onsetSize(const QString &size);
    void onSetColor(QColor color);
    void onSetBackground(QColor color);

    void bold(bool);
    void italic(bool);
    void underline(bool);
    void strikeout(bool);
    void sub(bool);
    void sup(bool);

    void alignLeft(bool);
    void alignCenter(bool);
    void alignRight(bool);
    void onZoomIn();
    void onZoomOut();
    void onAutoWidth();
    void onSaveToWord();
    void onZoomFactor(int value);
    void onResizeZoomFactor();
    void onSetZoomFactor(int value);

    void onUpdateUIState(GLDRichTextEdit * edit, bool bMousePress);    
    void onPopupMenu();

private slots:
    void onBtnEditClicked(bool);
    void onClipboardChanged();
    void onActImportFile();
    void onActExportFile();
    void onMacroItemTirggered(const GString &text);

private:
    QToolButton *m_pBtnEdit;

    QToolButton *m_pBtnUndo;
    QToolButton *m_pBtnRedo;
    QToolButton *m_pBtnCut;
    QToolButton *m_pBtnCopy;
    QToolButton *m_pBtnPaste;

    GLDComboBox *m_pFontSizeComboBox;
    QFontComboBox *m_pFontsBox;

    GColorListEx *m_pCmbBackgroundColor;
    GColorListEx *m_pCmbColor;
    QToolButton *m_pBtnBold;
    QToolButton *m_pBtnItalic;
    QToolButton *m_pBtnUnderline;
    QToolButton *m_pBtnStrikeout;
    QToolButton *m_pBtnSub;
    QToolButton *m_pBtnSup;

    QToolButton *m_pBtnLeft;
    QToolButton *m_pBtnCenter;
    QToolButton *m_pBtnRight;
    QSlider *m_pSlider;
    QToolButton *m_pBtnZoomOut;
    QToolButton *m_pBtnZoomIn;
    QToolButton *m_pBtnAutoWidth;
//    GLDSpinBox *m_pSpbZoomFactor;
    QSlider *m_pZoomFactorSlider;
    QLabel *m_pZoomFactorLabel;

    QClipboard* m_pClipboard;

    GLDAction *m_pActEdit;
    GLDAction *m_pActInsertMacro;
    GLDAction *m_pActImportFile;
    GLDAction *m_pActExportFile;
    GLDAction *m_pActExportWordFile;
    GLDAction *m_pActCut;
    GLDAction *m_pActCopy;
    GLDAction *m_pActPaste;

    GLDAction *m_pActProjInfo; // 单位工程信息
    GLDAction *m_pActProjCharacter; //工程特征

    GLDAction *m_pActEntityInfo; //标段工程项目信息
    GLDAction *m_pActEntityExtra; //标段工程附加信息

    bool m_bEditable;
    int  m_eBold;
    int  m_nPointSize;
    bool m_bItalic;
    bool m_bUnderline;
    GString m_sFontFamily;
    GString m_sExportPath;
    GBQModelType m_eModelType;
    GSPModel m_oProjModel;
    GSPDatabase m_oDatabase;
    GSPPropTable m_oPrefaceTable;

    QSignalMapper *m_pMapper;

    GMap<GString, GString> m_oProjInfoHash;
    GMap<GString, GString> m_oProjAttrHash;

    GMap<GString, GString> m_oBidInfoHash;
    GMap<GString, GString> m_oBidExtraHash;

    GSPTable m_oDeltaSummaryMacroTBL;
    GSPTable m_oAnalyseMarcroTBL;
    GMap<GString, GString> m_oMacroValueHash;
    IGBQJobService *m_piJobService;
    IGBQBidService *m_piBidService;

    GLDRichTextEdit *m_pRichTextEdit;
    GLDRichEditScrollArea *m_pRichEditScrollArea;
    GSPPropTable m_oAnalyseReportTBL;

    double m_dZoomFactor;
};


class GSHAnalyseEditPreviewLayout : public GLDFrameLayout
{
public:
    GSHAnalyseEditPreviewLayout(GLDFrame *owner, IGLDFrameNavigator *navigator);
    virtual ~GSHAnalyseEditPreviewLayout();

public:
    virtual void alignFrame(const QList<QWidget*> &frames);
    virtual void applyLayout();
    virtual void addWidget(QWidget *widget);

public:
    void addLayout(QBoxLayout *layout);

private:
    QVBoxLayout* m_pMainLayout;
};

DEFGLDFRAMEFACTORY(GSHAnalyseEditPreviewFrame)

#endif // GSHANALYSEEDITPREVIEWFRAME_H
