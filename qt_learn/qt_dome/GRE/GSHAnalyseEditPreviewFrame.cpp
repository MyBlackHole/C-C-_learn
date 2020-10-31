#include "GSHAnalyseEditPreviewFrame.h"

#include <QApplication>
#include <QFontDatabase>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QTextCursor>
#include <QClipboard>
#include <QFileDialog>
#include <QSignalMapper>

#include <QColorDialog>
#include <QFont>
#include <QFontComboBox>
#include <QFileInfo>
#include "GLDColorListEx.h"

#include "GLDException.h"
#include "GLDUIUtils.h"
#include "GLDFileUtils.h"
#include "GLDSysUtils.h"
#include "GLDControl.h"
#include "GSPInterface.h"
#include "rtfreader.h"
#include "TextDocumentRtfOutput.h"
#include "GBQServiceIntf.h"
#include "GBQConsts.h"
#include "GBQShareTriggerUtils.h"
#include "GBQInstallPathUtils.h"
#include "GBQBidServiceIntf.h"
#include "GBQIniCfgFile.h"

#include "GSPDataSource.h"
#include "GLDRichTextEdit.h"
#include "GBQServiceIntf.h"
#include "GBQContextServiceIntf.h"
#include "GBQModelConsts.h"
#include "GSHModelConsts.h"
#include "GSHModelTypes.h"
#include "GSHConsts.h"
#include "GLDUserActivityIntf.h"

DEFINE_CONSTSTR(c_sDefaultFamily, "宋体")
DEFINE_CONSTSTR(c_sMacroFormat, "{%1}")
DEFINE_CONSTSTR(c_sMacroWithTitle, "{%1.%2}")
DEFINE_CONSTSTR(c_sMacroBeginFlag, "{")
DEFINE_CONSTSTR(c_sMacroEndFlag, "}")
DEFINE_CONSTSTR(c_sContent, "Content")
DEFINE_CONSTSTR(c_sPercent, "%")

DEFINE_CONSTSTR(c_sEditToolTip, "切换到编辑报告")
DEFINE_CONSTSTR(c_sEditPreViewTip, "切换到预览报告")
DEFINE_CONSTSTR(c_sUndoToolTip, "撤销(Ctrl+Z)")
DEFINE_CONSTSTR(c_sRedoToolTip, "重做(Ctrl+Y)")
DEFINE_CONSTSTR(c_sCutToolTip, "剪切(Ctrl+X)")
DEFINE_CONSTSTR(c_sCopyToolTip, "复制(Ctrl+C)")
DEFINE_CONSTSTR(c_sPasteToolTip, "粘贴(Ctrl+V)")
DEFINE_CONSTSTR(c_sFontToolTip, "字体")
DEFINE_CONSTSTR(c_sFontSizeToolTip, "字号")
DEFINE_CONSTSTR(c_sBoldToolTip, "加粗")
DEFINE_CONSTSTR(c_sItalicToolTip, "倾斜")
DEFINE_CONSTSTR(c_sUnderlineToolTip, "下划线")
DEFINE_CONSTSTR(c_sStrikeoutToolTip, "删除线")
DEFINE_CONSTSTR(c_sSubToolTip, "上标")
DEFINE_CONSTSTR(c_sSupToolTip, "下标")
DEFINE_CONSTSTR(c_sAlignLeftToolTip, "左对齐")
DEFINE_CONSTSTR(c_sAlignCenterToolTip, "居中")
DEFINE_CONSTSTR(c_sAlignRightToolTip, "右对齐")
DEFINE_CONSTSTR(c_sZoomOutToolTip, "缩小")
DEFINE_CONSTSTR(c_sZoomInToolTip, "放大")
DEFINE_CONSTSTR(c_sAutoWidthToolTip, "自适应宽度")
DEFINE_CONSTSTR(c_sZoomFactorToolTip, "放大比例")
DEFINE_CONSTSTR(c_sAnalyseTemplateFile, "GSPFiles/Analyse/Template/分析报告模板-NO.1.rtf")

static const int c_nDefaultSize = 9;


/* GSHAnalyseEditPreviewFrame */

GSHAnalyseEditPreviewFrame::GSHAnalyseEditPreviewFrame() : GLDFrame(),
    m_pBtnEdit(NULL), m_pBtnCut(NULL), m_pBtnCopy(NULL), m_pBtnPaste(NULL),
    m_pFontSizeComboBox(NULL), m_pFontsBox(NULL), m_pBtnBold(NULL), m_pBtnItalic(NULL), m_pBtnUnderline(NULL),
    m_pBtnLeft(NULL), m_pBtnCenter(NULL), m_pBtnRight(NULL),
    m_pClipboard(NULL),
    m_pActEdit(NULL), m_pActInsertMacro(NULL), m_pActImportFile(NULL), m_pActExportFile(NULL),
    m_pActProjInfo(NULL), m_pActProjCharacter(NULL), m_pActEntityInfo(NULL), m_pActEntityExtra(NULL),
    m_bEditable(true), m_sExportPath(""), m_eModelType(MTBid),
    m_oDatabase(NULL), m_oProjModel(NULL), m_oPrefaceTable(NULL),
    m_pRichTextEdit(NULL), m_pBtnUndo(NULL),
    m_pBtnRedo(NULL), m_dZoomFactor(1.0),
    m_pZoomFactorSlider(NULL), m_pZoomFactorLabel(NULL)
{
    m_pMapper = new QSignalMapper(this);

    m_oProjInfoHash.clear();
    m_oProjAttrHash.clear();

    m_oBidInfoHash.clear();
    m_oBidExtraHash.clear();

    m_eBold         = QFont::Normal;
    m_bItalic       = false;
    m_bUnderline    = false;
}

GSHAnalyseEditPreviewFrame::~GSHAnalyseEditPreviewFrame()
{
    freeAndNil(m_pMapper);

}

void GSHAnalyseEditPreviewFrame::initialize()
{
    initData();
    createItems();

    initLayout();    

    createPopMenu();

    connectPopupMenu(m_pRichTextEdit);

    addMacroToPopMenu();
    createConnects();
}

void GSHAnalyseEditPreviewFrame::activate()
{
    GLDFrame::activate();

    getMacroValues();
    if (!m_bEditable)
    {
        loadContentData();
        savePreviewContents();
        loadPreviewData();
    }
    else
    {
        loadContentData();
    }
}

void GSHAnalyseEditPreviewFrame::deactivate()
{
    QByteArray oByteArray;
    if (sameFloat(m_pRichTextEdit->saveToBuf(oByteArray), 0) && m_bEditable)
    {
        GVariant oVariant(oByteArray.toBase64());
        m_oAnalyseReportTBL.setValue(pfnFormatContent, oVariant);
        savePreviewContents();
    }
    GLDFrame::deactivate();
}

IGLDFrameLayout *GSHAnalyseEditPreviewFrame::createFrameLayout()
{
    return new GSHAnalyseEditPreviewLayout(this, m_piFrameNavigator);
}

IGLDFrameNavigator *GSHAnalyseEditPreviewFrame::createFrameNavigator()
{
    return new GLDToolBarNavigator(getRegisteredFrameInfos(), this);
}

IGLDToolBar *GSHAnalyseEditPreviewFrame::createToolBar()
{
    return new GLDMainToolBar(m_pActionContainer, uiService()->getMainToolBar());
}

void GSHAnalyseEditPreviewFrame::updateActions()
{
    if (m_pBtnEdit != NULL)
    {
        m_pBtnEdit->setEnabled(true);
    }

    m_pActInsertMacro->setEnable(!m_bEditable);
    m_pActImportFile->setEnable(m_bEditable);
    if (m_pActEntityExtra != NULL)
    {
        m_pActEntityExtra->setVisible((MTBid == m_eModelType) && m_oBidExtraHash.count() > 0);
    }

    if ((m_pBtnUndo != NULL) &&(m_pRichTextEdit != NULL))
    {
        m_pBtnUndo->setEnabled(m_pRichTextEdit->canUndo());
    }

    if ((m_pBtnRedo != NULL) &&(m_pRichTextEdit != NULL))
    {
        m_pBtnRedo->setEnabled(m_pRichTextEdit->canRedo());
    }
}

void GSHAnalyseEditPreviewFrame::updateActionsReadonlyExpt()
{
    if (m_pBtnEdit != NULL)
    {
        m_pBtnEdit->setEnabled(false);
    }

    if (m_pActExportFile != NULL)
    {
        m_pActExportFile->setEnable(true);
    }
}

void GSHAnalyseEditPreviewFrame::addMacroToPopMenu()
{
    if (MTBid == m_eModelType)
    {
        addMacroItem(
                    m_oBidInfoHash,
                    m_pActInsertMacro->getName() + "|" + m_pActEntityInfo->getName());

        IGBQBidService *pService = dynamic_cast<IGBQBidService*>(uiService()->getService());
        if (pService != NULL)
        {
            GBQBidType eBidType = pService->getBidInfo()->getBidType();
            if ((btOwner == eBidType) || (btBidder == eBidType) || (btOwnerEstimate == eBidType))
            {
                addMacroItem(
                            m_oBidExtraHash,
                            m_pActInsertMacro->getName() + "|" + m_pActEntityExtra->getName());
            }
        }
    }
    else
    {
        addMacroItem(
                    m_oProjInfoHash,
                    m_pActInsertMacro->getName() + "|" + m_pActProjInfo->getName());
        addMacroItem(
                    m_oProjAttrHash,
                    m_pActInsertMacro->getName() + "|" + m_pActProjCharacter->getName());
    }

    connect(
                m_pMapper, static_cast<void (QSignalMapper::*)(const GString&)>(&QSignalMapper::mapped),
                this, &GSHAnalyseEditPreviewFrame::onMacroItemTirggered);
}

void GSHAnalyseEditPreviewFrame::addMacroItem(
        const GMap<GString, GString> &macroList, const GString &parentURL)
{
    GLDAction *pAction = NULL;
    GMap<GString, GString>::const_iterator it = macroList.begin();

    while (it != macroList.end())
    {
        pAction = addAction(it.key(), it.key());
        connect(pAction, SIGNAL(triggered()), m_pMapper, SLOT(map()));
        m_pMapper->setMapping(pAction, it.key());

        getPopupMenu()->addAction(parentURL, it.key());

        ++it;
    }
}

void GSHAnalyseEditPreviewFrame::getMacrosInContent(const GString &content, GStrings &macros)
{
    GString sBodyContent = content;
    macros.clear();
    int nPos = 0;
    while (nPos < sBodyContent.length())
    {
        int nBegin = sBodyContent.indexOf(c_sMacroBeginFlag, nPos);
        if (nBegin < 0)
        {
            break;
        }

        int nEnd = sBodyContent.indexOf(c_sMacroEndFlag, nBegin);
        if (nEnd < nBegin)
        {
            break;
        }

        GString sMacro = sBodyContent.mid(nBegin, nEnd - nBegin + 1);
        macros.push_back(sMacro);
        nPos = nEnd + 1;
    }
}

void GSHAnalyseEditPreviewFrame::getMacroValue(GString &macro, const GStrings &macroList)
{
    GString sValue;
    for (int i = 0; i < macroList.count(); ++i)
    {
        sValue = getValueByMacro(macroList.at(i));
        macro.replace(macroList.at(i), sValue);
    }
}

void GSHAnalyseEditPreviewFrame::macroValueWithPattern(GString &macro, const GStrings &macroList)
{
    for (int i = 0; i < macroList.count(); ++i)
    {
        if (macro.indexOf(macroList.at(i)) >= 0)
        {
            macro.replace(macroList.at(i), getValueByMacro(macroList.at(i)));
        }
    }
}

/****************************************************************************************************
 作者： yanyq-a 2013-10-21
 参数： const GString&
 返回： GString
 功能： 根据宏名得到宏值，如果没有查询到该宏，则其值为它本身
****************************************************************************************************/
GString GSHAnalyseEditPreviewFrame::getValueByMacro(const GString &macro)
{
    if (MTBid == m_eModelType)
    {
        if (m_oBidInfoHash.contains(macro))
        {
            return m_oBidInfoHash[macro];
        }
        else if (m_oBidExtraHash.contains(macro))
        {
            return m_oBidExtraHash[macro];
        }
    }
    else
    {
        if (m_oProjInfoHash.contains(macro))
        {
            return m_oProjInfoHash[macro];
        }
        else if (m_oProjAttrHash.contains(macro))
        {
            return m_oProjAttrHash[macro];
        }
    }

    return macro;
}

void GSHAnalyseEditPreviewFrame::initData()
{
    IGBQService *pService = dynamic_cast<IGBQService*>(uiService()->getService());
    if (NULL == pService)
    {
        return;
    }


    m_piJobService = dynamic_cast<IGBQJobService*>(pService);
    m_piBidService = dynamic_cast<IGBQBidService*>(pService);

    if ((m_piJobService != NULL) && (m_piJobService->getOwner() != NULL))
    {
        m_piBidService = dynamic_cast<IGBQBidService*>(m_piJobService->getOwner());
        m_oProjModel = m_piBidService->getProjModel();
    }
    else if ((m_piJobService != NULL) && (m_piJobService->getOwner() == NULL))
    {
        m_oProjModel = m_piJobService->getProjModel();
    }
    else
    {
        m_oProjModel = m_piBidService->getProjModel();
    }

    if (NULL == m_oProjModel)
    {
        return;
    }

    m_oDatabase = m_oProjModel.find(dbnBusinessDB);
    m_oAnalyseReportTBL = GSPPropTable(m_oProjModel.findTable(dbnBusinessDB, ptnAnalyseReport));
    m_oDeltaSummaryMacroTBL = m_oProjModel.findTable(dbnBusinessDB, ptnDeltaSummaryMacro);

    m_sExportPath = GBQInstallPathUtils::getDefaultGBQFilePath() + pService->getRegionName() + "/";
}

void GSHAnalyseEditPreviewFrame::createItems()
{
    m_pBtnEdit = new QToolButton(this);
    m_pBtnEdit->setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIcons(BtnEdit);
    m_pBtnEdit->setFixedWidth(64);
    m_pBtnEdit->setFixedHeight(22);
    //    m_pBtnEdit->setText(tr("Edit"));
    m_pBtnEdit->setToolTip(c_sEditPreViewTip);

    m_pBtnUndo = new QToolButton(this);
    setIcons(ginUndo);
    m_pBtnUndo->setToolTip(c_sUndoToolTip);
    m_pBtnUndo->setFixedHeight(22);

    m_pBtnRedo = new QToolButton(this);
    setIcons(ginRedo);
    m_pBtnRedo->setToolTip(c_sRedoToolTip);
    m_pBtnRedo->setFixedHeight(22);

    m_pBtnCut = new QToolButton(this);
    setIcons(BtnCut);
    m_pBtnCut->setToolTip(c_sCutToolTip);
    m_pBtnCut->setFixedHeight(22);

    m_pBtnCopy = new QToolButton(this);
    setIcons(BtnCopy);
    m_pBtnCopy->setToolTip(c_sCopyToolTip);
    m_pBtnCopy->setFixedHeight(22);

    m_pBtnPaste = new QToolButton(this);
    setIcons(BtnPaste);
    m_pBtnPaste->setToolTip(c_sPasteToolTip);
    m_pBtnPaste->setFixedHeight(22);

    m_pFontsBox = new QFontComboBox(this);
    m_pFontsBox->setToolTip(c_sFontToolTip);
    m_pFontsBox->setFixedHeight(22);

    m_pFontSizeComboBox = new GLDComboBox(uiService(), this);
    m_pFontSizeComboBox->setEditable(true);
    m_pFontSizeComboBox->setToolTip(c_sFontSizeToolTip);
    m_pFontSizeComboBox->setFixedHeight(22);
    m_pFontSizeComboBox->setEditable(false);

    QList<int> fntSizes = QFontDatabase::standardSizes();
    for (int i = 0; i < fntSizes.size(); ++i)
    {
        m_pFontSizeComboBox->addItem(QString::number(fntSizes.at(i)));
    }
    m_pFontSizeComboBox->setCurrentText(intToStr(c_nDefaultSize));
    //m_pSpinBox->setWindowFlags(Qt::Popup);

    m_pCmbBackgroundColor = new GColorListEx(this, QColor(255, 255, 255));
    m_pCmbColor = new GColorListEx(this);
    m_pCmbColor->setFixedHeight(22);
    m_pCmbBackgroundColor->setFixedHeight(22);
    setIcons(ginBackgroundColor);
    setIcons(ginFontColor);

    m_pBtnBold = new QToolButton(this);
    setIcons(BtnBold);
    m_pBtnBold->setToolTip(c_sBoldToolTip);
    m_pBtnBold->setFixedHeight(22);

    m_pBtnItalic = new QToolButton(this);
    setIcons(BtnItalic);
    m_pBtnItalic->setToolTip(c_sItalicToolTip);
    m_pBtnItalic->setFixedHeight(22);

    m_pBtnUnderline = new QToolButton(this);
    setIcons(BtnUnderline);
    m_pBtnUnderline->setToolTip(c_sUnderlineToolTip);
    m_pBtnUnderline->setFixedHeight(22);

    m_pBtnStrikeout = new QToolButton(this);
    setIcons(ginStrikeout);
    m_pBtnStrikeout->setToolTip(c_sStrikeoutToolTip);
    m_pBtnStrikeout->setFixedHeight(22);

    m_pBtnSub = new QToolButton(this);
    setIcons(ginSub);
    m_pBtnSub->setToolTip(c_sSubToolTip);
    m_pBtnSub->setFixedHeight(22);

    m_pBtnSup = new QToolButton(this);
    setIcons(ginSup);
    m_pBtnSup->setToolTip(c_sSupToolTip);
    m_pBtnSup->setFixedHeight(22);

    m_pBtnLeft = new QToolButton(this);
    setIcons(BtnLeft);
    m_pBtnLeft->setToolTip(c_sAlignLeftToolTip);
    m_pBtnLeft->setFixedHeight(22);

    m_pBtnCenter = new QToolButton(this);
    setIcons(BtnCenter);
    m_pBtnCenter->setToolTip(c_sAlignCenterToolTip);
    m_pBtnCenter->setFixedHeight(22);

    m_pBtnRight = new QToolButton(this);
    setIcons(BtnRight);
    m_pBtnRight->setToolTip(c_sAlignRightToolTip);
    m_pBtnRight->setFixedHeight(22);

//    m_pSlider = new QSlider(Qt::Horizontal, this);
//    setIcons(ginSlider);
//    m_pSlider->setFixedSize(100, 22);

    m_pBtnZoomIn = new QToolButton(this);
    setIcons(ginZoomIn);
    m_pBtnZoomIn->setToolTip(c_sZoomInToolTip);
    m_pBtnZoomIn->setFixedHeight(22);

    m_pBtnZoomOut = new QToolButton(this);
    setIcons(ginZoomOut);
    m_pBtnZoomOut->setToolTip(c_sZoomOutToolTip);
    m_pBtnZoomOut->setFixedHeight(22);

    m_pBtnAutoWidth = new QToolButton(this);
    setIcons(ginAutoWidth);
    m_pBtnAutoWidth->setToolTip(c_sAutoWidthToolTip);
    m_pBtnAutoWidth->setFixedHeight(22);

//    m_pSpbZoomFactor = new GLDSpinBox(uiService(), this);
//    m_pSpbZoomFactor->setMaximum(500);
//    m_pSpbZoomFactor->setMinimum(10);
//    m_pSpbZoomFactor->setValue(100);
//    m_pSpbZoomFactor->setToolTip(c_sZoomFactorToolTip);
//    m_pSpbZoomFactor->setFixedHeight(22);
    m_pZoomFactorSlider = new QSlider(Qt::Horizontal, this);
    m_pZoomFactorSlider->setRange(10, 500);
    m_pZoomFactorSlider->setValue(100);
    m_pZoomFactorSlider->setSingleStep(1);
    m_pZoomFactorSlider->setToolTip(c_sZoomFactorToolTip);
    m_pZoomFactorSlider->setFixedHeight(22);
    m_pZoomFactorSlider->setMaximumWidth(150);
    m_pZoomFactorSlider->setMinimumWidth(150);

    connect(m_pZoomFactorSlider, SIGNAL(valueChanged(int)), this, SLOT(onSetZoomFactor(int)));
    m_pZoomFactorLabel = new QLabel(this);
    m_pZoomFactorLabel->setFixedHeight(22);

    m_pRichTextEdit = new GLDRichTextEdit(0);
    m_pRichEditScrollArea = new GLDRichEditScrollArea(this, m_pRichTextEdit);
    connect(m_pRichTextEdit, SIGNAL(updateScroll()), m_pRichEditScrollArea, SLOT(updateUISlot()));
    m_pRichTextEdit->setAttribute(Qt::WA_DeleteOnClose);
    m_pRichTextEdit->show();
    if (m_oAnalyseReportTBL.isNull(pfnFormatContent))
    {
        IGBQService *pService = dynamic_cast<IGBQService*>(uiService()->getService());
        GString sCorePath = GBQInstallPathUtils::getSHCorePath(
                    pService->getContextService()->getAppWorkPath(),
                    pService->getContextService()->getRegionName());
        m_pRichTextEdit->setModify(false);
        m_pRichTextEdit->open(sCorePath + c_sAnalyseTemplateFile);

        QByteArray oByteArray;
        if (sameFloat(m_pRichTextEdit->saveToBuf(oByteArray), 0))
        {
            GVariant oVariant(oByteArray.toBase64());
            m_oAnalyseReportTBL.setValue(pfnFormatContent, oVariant);
        }
    }
    else
    {
        loadContentData();
    }
    //    m_pClipboard = QApplication::clipboard();

    m_pActEdit = addAction("Mode_Editable", tr("Mode_Editable"));
    m_pActInsertMacro = addAction("Insert_Macro", tr("Insert_Macro"));
    m_pActImportFile = addAction("File_Import", tr("File_Import"));
    m_pActExportFile = addAction("File_Export", tr("File_Export"));
    m_pActExportWordFile = addAction("File_ExportWord", tr("File_ExportWord"));
    m_pActExportFile->setIcon(":/GSHAnalyse/Icons/Analyse/SaveTemplate.png");
    m_pActImportFile->setIcon(":/GSHAnalyse/Icons/Analyse/LoadTemplate.png");
    m_pActExportWordFile->setIcon(":/GSHAnalyse/Icons/Analyse/ExportWord.png");

    m_pActCut = addAction("GPS_Cut", c_sCutToolTip);
    m_pActCut->setShortcut("Ctrl + Z");
    m_pActCopy = addAction("GPS_Copy", c_sCopyToolTip);
    m_pActCopy->setShortcut("Ctrl + C");
    m_pActPaste = addAction("GPS_Paste", c_sPasteToolTip);
    m_pActPaste->setShortcut("Ctrl + V");

    getToolBar()->addAction(
                "GPS_Analyse", tr("GPS_Analyse"), "Import", tr("GPS_Import"),
                m_pActImportFile->getName(), "", GLD::ToolButtonTextUnderIcon);
    getToolBar()->addAction(
                "GPS_Analyse", tr("GPS_Analyse"), "Export", tr("GPS_Import"),
                m_pActExportFile->getName(), "", GLD::ToolButtonTextUnderIcon);
    getToolBar()->addAction(
                "GPS_Analyse", tr("GPS_Analyse"), "ExportWord", tr("GPS_ExportWord"),
                m_pActExportWordFile->getName(), "", GLD::ToolButtonTextUnderIcon);

    if (MTBid == m_eModelType)
    {
        m_pActEntityInfo = addAction("Entity_Info", tr("Entity_Info")); // 项目信息
        m_pActEntityExtra = addAction("Entity_Extra", tr("Entity_Extra")); // 附加信息
    }
    else
    {
        m_pActProjInfo = addAction("Proj_Info", tr("Proj_Info")); // 工程信息
        m_pActProjCharacter = addAction("Proj_Character", tr("Proj_Character")); // 工程特征
    }
}

void GSHAnalyseEditPreviewFrame::initLayout()
{
    QHBoxLayout *pTopLayout = new QHBoxLayout();
    pTopLayout->addWidget(m_pBtnEdit);
    pTopLayout->addWidget(m_pBtnUndo);
    pTopLayout->addWidget(m_pBtnRedo);
    pTopLayout->addWidget(m_pBtnCut);
    pTopLayout->addWidget(m_pBtnCopy);
    pTopLayout->addWidget(m_pBtnPaste);
    pTopLayout->addWidget(m_pFontsBox);
    pTopLayout->addWidget(m_pFontSizeComboBox);
    pTopLayout->addWidget(m_pCmbBackgroundColor);
    pTopLayout->addWidget(m_pCmbColor);
    pTopLayout->addWidget(m_pBtnBold);
    pTopLayout->addWidget(m_pBtnItalic);
    pTopLayout->addWidget(m_pBtnUnderline);
    pTopLayout->addWidget(m_pBtnStrikeout);
    pTopLayout->addWidget(m_pBtnSub);
    pTopLayout->addWidget(m_pBtnSup);
    pTopLayout->addWidget(m_pBtnLeft);
    pTopLayout->addWidget(m_pBtnCenter);
    pTopLayout->addWidget(m_pBtnRight);
    pTopLayout->addWidget(m_pBtnZoomIn);
    pTopLayout->addWidget(m_pBtnZoomOut);
    pTopLayout->addWidget(m_pBtnAutoWidth);
    //pTopLayout->addWidget(m_pSpbZoomFactor);
    pTopLayout->addWidget(m_pZoomFactorSlider);
    pTopLayout->addWidget(m_pZoomFactorLabel);
    pTopLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    pTopLayout->setSpacing(4);
    pTopLayout->setContentsMargins(5, 0, 0, 10);

    dynamic_cast<GSHAnalyseEditPreviewLayout*>(m_piFrameLayout)->addLayout(pTopLayout);

    QHBoxLayout *pHBLRichTextEdit = new QHBoxLayout();
    pHBLRichTextEdit->addWidget(m_pRichEditScrollArea);
    dynamic_cast<GSHAnalyseEditPreviewLayout*>(m_piFrameLayout)->addLayout(pHBLRichTextEdit);
}

void GSHAnalyseEditPreviewFrame::createConnects()
{
    connect(m_pBtnEdit, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::onBtnEditClicked);
    connect(m_pBtnUndo, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::undo);
    connect(m_pBtnRedo, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::redo);
    connect(m_pBtnCut, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::cut);
    connect(m_pBtnCopy, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::copy);
    connect(m_pBtnPaste, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::paste);
    connect(m_pActCut, &GLDAction::triggered, this, &GSHAnalyseEditPreviewFrame::cut);
    connect(m_pActCopy, &GLDAction::triggered, this, &GSHAnalyseEditPreviewFrame::copy);
    connect(m_pActPaste, &GLDAction::triggered, this, &GSHAnalyseEditPreviewFrame::paste);

    connect(m_pFontsBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onSetfont(const QFont &)));
    connect(m_pFontSizeComboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onsetSize(const QString &)));
    connect(m_pCmbBackgroundColor, SIGNAL(colorIndexChange(QColor)), this, SLOT(onSetBackground(QColor)));
    connect(m_pCmbColor, SIGNAL(colorIndexChange(QColor)), this, SLOT(onSetColor(QColor)));

    connect(m_pBtnBold, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::bold);
    connect(m_pBtnItalic, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::italic);
    connect(m_pBtnUnderline, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::underline);
    connect(m_pBtnStrikeout, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::strikeout);
    connect(m_pBtnSub, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::sub);
    connect(m_pBtnSup, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::sup);
    connect(m_pBtnLeft, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::alignLeft);
    connect(m_pBtnCenter, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::alignCenter);
    connect(m_pBtnRight, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::alignRight);
    connect(m_pBtnZoomOut, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::onZoomOut);
    connect(m_pBtnZoomIn, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::onZoomIn);
    connect(m_pBtnAutoWidth, &QToolButton::clicked, this, &GSHAnalyseEditPreviewFrame::onAutoWidth);
//    connect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));

    connect(m_pClipboard, &QClipboard::dataChanged, this, &GSHAnalyseEditPreviewFrame::onClipboardChanged);

    connect(m_pActEdit, &GLDAction::triggered, this, &GSHAnalyseEditPreviewFrame::onBtnEditClicked);
    connect(m_pActImportFile, &GLDAction::triggered, this, &GSHAnalyseEditPreviewFrame::onActImportFile);
    connect(m_pActExportFile, &GLDAction::triggered, this, &GSHAnalyseEditPreviewFrame::onActExportFile);
    connect(m_pActExportWordFile, &GLDAction::triggered, this, &GSHAnalyseEditPreviewFrame::onSaveToWord);
    connect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
            this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);


    connect(m_pRichTextEdit, SIGNAL(updateUIState(GLDRichTextEdit *, bool)),
            this, SLOT(onUpdateUIState(GLDRichTextEdit *, bool)));
    connect(m_pRichTextEdit, SIGNAL(onRButtonUp()), this, SLOT(onPopupMenu()));
}

void GSHAnalyseEditPreviewFrame::createPopMenu()
{
    //    insertIntoMenu(m_pActEdit);
    //    insertIntoMenu(m_pActInsertMacro);
    insertIntoMenu(m_pActCut);
    insertIntoMenu(m_pActCopy);
    insertIntoMenu(m_pActPaste);
    insertIntoMenu(m_pActImportFile);
    insertIntoMenu(m_pActExportFile);
    insertIntoMenu(m_pActExportWordFile);

    //    if (MTBid == m_eModelType)
    //    {
    //        insertIntoMenu(m_pActEntityInfo, m_pActInsertMacro->getName());
    //        insertIntoMenu(m_pActEntityExtra, m_pActInsertMacro->getName());
    //    }
    //    else
    //    {
    //        insertIntoMenu(m_pActProjInfo, m_pActInsertMacro->getName());
    //        insertIntoMenu(m_pActProjCharacter, m_pActInsertMacro->getName());
    //    }
}

void GSHAnalyseEditPreviewFrame::insertIntoMenu(GLDAction *action, const GString &parentURL)
{
    getPopupMenu()->addAction(parentURL, action->getName());
}

void GSHAnalyseEditPreviewFrame::loadPreviewData()
{
    GVariant oVariant = m_oAnalyseReportTBL.value(pfnPreviewContent);
    QByteArray oByteArray = QByteArray::fromBase64(oVariant.toByteArray());
    m_pRichTextEdit->readFromBuf(oByteArray);
}

void GSHAnalyseEditPreviewFrame::loadContentData()
{
    GVariant oVariant = m_oAnalyseReportTBL.value(pfnFormatContent);
    QByteArray oByteArray = QByteArray::fromBase64(oVariant.toByteArray());
    m_pRichTextEdit->readFromBuf(oByteArray);
//    m_pRichTextEdit->setZoomFactorValue(100);
//    m_pRichTextEdit->emitUpdateScroll();
}

void GSHAnalyseEditPreviewFrame::setIcons(GBQIconName btn, bool pressed)
{
    switch (btn)
    {
    case ginUndo:
        m_pBtnUndo->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Undo.png"));
        break;
    case ginRedo:
        m_pBtnRedo->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Redo.png"));
        break;
    case ginStrikeout:
        m_pBtnStrikeout->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Strikeout.png"));
        break;
    case ginSub:
        m_pBtnSub->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Sub.png"));
        break;
    case ginSup:
        m_pBtnSup->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Sup.png"));
        break;
    case BtnEdit:
        m_pBtnEdit->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Preview.png"));
        m_pBtnEdit->setIconSize(QSize(64, 22));
        break;
    case BtnCut:
        m_pBtnCut->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Cut.png"));
        break;
    case BtnCopy:
        m_pBtnCopy->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Copy.png"));
        break;
    case BtnPaste:
        m_pBtnPaste->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Paste.png"));
        break;
    case BtnBold:
        m_pBtnBold->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Bold.png"));
        //        if (pressed)
        //        {
        //            m_pBtnBold->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Bold-pressed.png"));
        //        }
        //        else
        //        {
        //            m_pBtnBold->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Bold.png"));
        //        }
        break;
    case BtnItalic:
        m_pBtnItalic->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Italic.png"));
        //        if (pressed)
        //        {
        //            m_pBtnItalic->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Italic-pressed.png"));
        //        }
        //        else
        //        {
        //            m_pBtnItalic->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Italic.png"));
        //        }
        break;
    case BtnUnderline:
        m_pBtnUnderline->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Underline.png"));
        //        if (pressed)
        //        {
        //            m_pBtnUnderline->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Underline-pressed.png"));
        //        }
        //        else
        //        {
        //            m_pBtnUnderline->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Underline.png"));
        //        }
        break;
    case BtnLeft:
        m_pBtnLeft->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Left.png"));
        //        if (pressed)
        //        {
        //            m_pBtnLeft->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Left-pressed.png"));
        //        }
        //        else
        //        {
        //            m_pBtnLeft->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Left.png"));
        //        }
        break;
    case BtnCenter:
        m_pBtnCenter->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Center.png"));
        //        if (pressed)
        //        {
        //            m_pBtnCenter->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Center-pressed.png"));
        //        }
        //        else
        //        {
        //            m_pBtnCenter->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Center.png"));
        //        }
        break;
    case BtnRight:
        m_pBtnRight->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Right.png"));
        //        if (pressed)
        //        {
        //            m_pBtnRight->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Right-pressed.png"));
        //        }
        //        else
        //        {
        //            m_pBtnRight->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Right.png"));
        //        }
        break;
    case ginBackgroundColor:
        m_pCmbBackgroundColor->setIconUrlInEdit(":/GSHAnalyse/Icons/Analyse/BackgroudColor.png");
        break;
    case ginFontColor:
        m_pCmbColor->setIconUrlInEdit(":/GSHAnalyse/Icons/Analyse/FontColor.png");
        break;
    case ginZoomIn:
        m_pBtnZoomIn->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/ZoomIn.png"));
        break;
    case ginZoomOut:
        m_pBtnZoomOut->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/ZoomOut.png"));
        break;
    case ginAutoWidth:
        m_pBtnAutoWidth->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/AutoWidth.png"));
        break;
    case ginSlider:
        //        m_pSlider->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Slider.png"));
        break;
    default:
        break;
    }
}

void GSHAnalyseEditPreviewFrame::setItemsEnable()
{
    GLDVector<QWidget*> oItems;
    oItems.push_back(m_pBtnUndo);
    oItems.push_back(m_pBtnRedo);
    oItems.push_back(m_pBtnCut);
    oItems.push_back(m_pBtnCopy);
    oItems.push_back(m_pBtnPaste);
    oItems.push_back(m_pFontsBox);
    oItems.push_back(m_pFontSizeComboBox);
    oItems.push_back(m_pCmbBackgroundColor);
    oItems.push_back(m_pCmbColor);
    oItems.push_back(m_pBtnBold);
    oItems.push_back(m_pBtnItalic);
    oItems.push_back(m_pBtnUnderline);
    oItems.push_back(m_pBtnStrikeout);
    oItems.push_back(m_pBtnSub);
    oItems.push_back(m_pBtnSup);
    oItems.push_back(m_pBtnLeft);
    oItems.push_back(m_pBtnCenter);
    oItems.push_back(m_pBtnRight);

    foreach (QWidget *item, oItems)
    {
        item->setEnabled(!m_bEditable);
    }

    m_pRichTextEdit->setReadOnly(m_bEditable);
    m_bEditable = !m_bEditable;
}


void GSHAnalyseEditPreviewFrame::getMacroValues()
{
    showProjectInfoMacro();
    showProjectCharaterMacro();
    showSysDefMacro();
    showAllCustomMacro();
}

void GSHAnalyseEditPreviewFrame::showProjectInfoMacro()
{
    GSPTable oProjectInfoTBL = m_oProjModel.findTable(dbnBusinessDB, ptnProjectInfo);
    GSPTable oProjectInfoTitleTBL = m_oProjModel.findTable(dbnBusinessDB, ptnProjectInfoTitle);

    GSPPropTable oOptionTBL = (GSPPropTable)(m_oProjModel.findTable(dbnBusinessDB, ptnOption));
    GSHProductType eProductType = (GSHProductType)(oOptionTBL.asInteger(pfnAuditingProductType));
    GString sFilter = "";

    switch (eProductType)
    {
    case int(gptGJS):
        sFilter = "ProjectInfoTitleID in [1, 4]";
        break;
    case int(gptGYS):
        sFilter = "ProjectInfoTitleID in [1,2,3,4]";
        break;
    default:
        break;
    }

    GSPDataSource oDataSource;
    oDataSource.viewNodes()->add(oProjectInfoTitleTBL.createView(sFilter));
    oDataSource.viewNodes()->add(oProjectInfoTBL.createView(sFilter));
    oDataSource.setActive(true);
    GLDList<GSPCustomRowNode *> oAllRowNodes;
    oDataSource.allRowNodes(oAllRowNodes);

    for (int i = 0; i < oAllRowNodes.count(); ++i)
    {
        GSPRecord oInfoRec = oAllRowNodes[i]->record();
        if (oInfoRec.table() != oProjectInfoTBL)
        {
            continue;
        }

        GString sMacroName = oInfoRec.asString(pfnDescription);
        GString sMacroValue = oInfoRec.asString(pfnContent);

        if (sMacroName.isEmpty())
        {
            continue;
        }

        GSPRecord oTitleRec = oInfoRec.findField(pfnProjectInfoTitleID).masterRecord();
        if (oTitleRec != NULL)
        {
            sMacroName = oTitleRec.asString(pfnDescription) + "." + sMacroName;
        }

        m_oMacroValueHash.insert(sMacroName, sMacroValue);
    }
}

void GSHAnalyseEditPreviewFrame::showProjectCharaterMacro()
{
    GSPTable oProjectAttrTBL = m_oProjModel.findTable(dbnBusinessDB, ptnProjectAttr);
    if (oProjectAttrTBL != NULL)
    {
        for (int i = 0; i < oProjectAttrTBL.recordCount(); ++i)
        {
            GString sMacroName = c_sCaption_SS + "." + oProjectAttrTBL[i].asString(pfnDescription);
            GString sMacroValue = oProjectAttrTBL[i].asString(pfn_SS_Content);
            m_oMacroValueHash.insert(sMacroName, sMacroValue);
        }

        for (int i = 0; i < oProjectAttrTBL.recordCount(); ++i)
        {
            GString sMacroName = c_sCaption_SD + "." + oProjectAttrTBL[i].asString(pfnDescription);
            GString sMacroValue = oProjectAttrTBL[i].asString(pfnContent);
            m_oMacroValueHash.insert(sMacroName, sMacroValue);
        }
    }
}

void GSHAnalyseEditPreviewFrame::showSysDefMacro()
{
    GSPTable oAuditingIndexInfoTBL = m_oProjModel.findTable(dbnBusinessDB, ptnAuditingIndexInfo);
    if (oAuditingIndexInfoTBL != NULL)
    {
        for (int i = 0; i < oAuditingIndexInfoTBL.recordCount(); ++i)
        {
            GString sMacroName = oAuditingIndexInfoTBL[i].asString(pfnName);
            GString sMacroValue = oAuditingIndexInfoTBL[i].asString(pfnIndex);
            m_oMacroValueHash.insert(sMacroName, sMacroValue);
        }
    }
}

void GSHAnalyseEditPreviewFrame::showAllCustomMacro()
{
    if (m_oDeltaSummaryMacroTBL == NULL)
    {
        return;
    }

    for (int i = 0; i < m_oDeltaSummaryMacroTBL.recordCount(); ++i)
    {
        doShowCustomItem(m_oDeltaSummaryMacroTBL[i]);
    }
}

void GSHAnalyseEditPreviewFrame::doShowCustomItem(GSPRecord customMacroItem)
{
    if (NULL == customMacroItem)
    {
        return;
    }

    for (int i = 0; i < customMacroItem.fieldCount(); ++i)
    {
        if (needShowField(customMacroItem[i].fieldName(), getMeasureItemEnable()))
        {
            GString sMacroName = customMacroItem.asString(pfnDescription) + "."
                    + customMacroItem[i].fieldSchema().displayName();
            GString sMacroValue = customMacroItem[i].asString();
            m_oMacroValueHash.insert(sMacroName, sMacroValue);
        }
    }
}

bool GSHAnalyseEditPreviewFrame::needShowField(const GString &fieldName, bool hasMeasure)
{
    bool bNeedShowField = false;

    GStrings oFieldNames;
    oFieldNames << pfnDeltaSummaryMacroID << pfnDescription << pfnRemark;

    if (oFieldNames.indexOf(fieldName) == -1)
    {
        bNeedShowField = true;
    }

    if (!bNeedShowField)
    {
        return bNeedShowField;
    }

    oFieldNames.clear();
    oFieldNames << pfnCSXMAuditingPlusDeltaAmount << pfnCSXMAuditingNegativeDeltaAmount;

    if (oFieldNames.indexOf(fieldName) == -1)
    {
        bNeedShowField = hasMeasure;
    }

    return bNeedShowField;
}

bool GSHAnalyseEditPreviewFrame::getMeasureItemEnable()
{
    // 标段
    if ((m_piJobService == NULL) && (m_piBidService != NULL))
    {
        return true;
    }
    else if ((m_piJobService != NULL) && (m_piBidService == NULL))
    {
        GSPPropTable oOptionTBL = (GSPPropTable)(m_oProjModel.findTable(dbnBusinessDB, ptnOption));
        return ((GBQEstiType(oOptionTBL.asInteger(pfnEstiType)) == etBQEsti)
                || (GBQEstiType(oOptionTBL.asInteger(pfnEstiType)) == etBQEsti)
                || ((oOptionTBL.findField(pfnMeasureItemEnable) != NULL))
                && (oOptionTBL.asBoolean(pfnMeasureItemEnable)));
    }

}

void GSHAnalyseEditPreviewFrame::savePreviewContents()
{
    // todo jiangjb 这块回头需要优化
    GMap<GString, GString>::iterator oIter = m_oMacroValueHash.begin();
    for (; oIter != m_oMacroValueHash.end(); ++oIter)
    {
        GString value = oIter.value();
        m_pRichTextEdit->replaceAllText(
                    c_sMacroBeginFlag + oIter.key() + c_sMacroEndFlag, true, false, value);
    }

    QByteArray oByteArray;
    if (sameFloat(m_pRichTextEdit->saveToBuf(oByteArray), 0))
    {
        GVariant oVariant(oByteArray.toBase64());
        m_oProjModel.beginPauseCommand();
        m_oAnalyseReportTBL.setValue(pfnPreviewContent, oVariant);
        m_oProjModel.endPauseCommand();
    }
}

void GSHAnalyseEditPreviewFrame::undo(bool)
{
    m_pRichTextEdit->undo();
}

void GSHAnalyseEditPreviewFrame::redo(bool)
{
    m_pRichTextEdit->redo();
}

void GSHAnalyseEditPreviewFrame::cut(bool)
{
    m_pRichTextEdit->cut();
}

void GSHAnalyseEditPreviewFrame::copy(bool)
{
    m_pRichTextEdit->copy();
}

void GSHAnalyseEditPreviewFrame::paste(bool)
{
    m_pRichTextEdit->paste();
}

void GSHAnalyseEditPreviewFrame::onSetfont(const QFont &fnt)
{
    m_pRichTextEdit->setFont(fnt);
}

void GSHAnalyseEditPreviewFrame::onSetSize(int size)
{
    m_pRichTextEdit->setFontSize(size);
}

void GSHAnalyseEditPreviewFrame::onsetSize(const QString &size)
{
    int nSize = size.toInt();

    if (0 >= nSize)
    {
        return;
    }

    m_pRichTextEdit->setFontSize(nSize);
}

void GSHAnalyseEditPreviewFrame::onSetColor(QColor color)
{
    m_pRichTextEdit->setFontColor(color);
}

void GSHAnalyseEditPreviewFrame::onSetBackground(QColor color)
{
    m_pRichTextEdit->setFontBackgroundColor(color);
}

void GSHAnalyseEditPreviewFrame::bold(bool)
{
    m_pRichTextEdit->bold();
}

void GSHAnalyseEditPreviewFrame::italic(bool)
{
    m_pRichTextEdit->italic();
}

void GSHAnalyseEditPreviewFrame::underline(bool)
{
    m_pRichTextEdit->underline();
}

void GSHAnalyseEditPreviewFrame::strikeout(bool)
{
    m_pRichTextEdit->strikeout();
}

void GSHAnalyseEditPreviewFrame::sub(bool)
{
    m_pRichTextEdit->superscript();
}

void GSHAnalyseEditPreviewFrame::sup(bool)
{
    m_pRichTextEdit->subscript();
}

void GSHAnalyseEditPreviewFrame::alignLeft(bool b)
{
    Q_UNUSED(b);
    //if (b)
    {
        m_pRichTextEdit->setAlign(GLDRichTextEdit::Left);
    }
}

void GSHAnalyseEditPreviewFrame::alignCenter(bool b)
{
    Q_UNUSED(b);
    //if (b)
    {
        m_pRichTextEdit->setAlign(GLDRichTextEdit::Center);
    }
}

void GSHAnalyseEditPreviewFrame::alignRight(bool b)
{
    Q_UNUSED(b);
    //if (b)
    {
        m_pRichTextEdit->setAlign(GLDRichTextEdit::Right);
    }
}

void GSHAnalyseEditPreviewFrame::onZoomIn()
{
    double dValue = 0.1;
    if (m_dZoomFactor > 1)
    {
        dValue = 0.5;
    }
    if (m_dZoomFactor < 4.5)
    {
        m_dZoomFactor += dValue;
    }

    disconnect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
               this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);
    m_pRichTextEdit->setZoomFactorValue(m_dZoomFactor);
    m_pRichTextEdit->emitUpdateScroll();
//    disconnect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));
//    m_pSpbZoomFactor->setValue(m_dZoomFactor * 100);
    m_pZoomFactorSlider->setValue(m_dZoomFactor * 100);
//    connect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));
    connect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
            this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);
}

void GSHAnalyseEditPreviewFrame::onZoomOut()
{
    disconnect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
               this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);
    double dValue = 0.1;
    if (m_dZoomFactor > 1.5)
    {
        dValue = 0.5;
    }
    if (m_dZoomFactor > 0.1)
    {
        m_dZoomFactor -= dValue;
    }
    m_pRichTextEdit->setZoomFactorValue(m_dZoomFactor);
    m_pRichTextEdit->emitUpdateScroll();
//    disconnect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));
//    m_pSpbZoomFactor->setValue(m_dZoomFactor * 100);
    m_pZoomFactorSlider->setValue(m_dZoomFactor * 100);
//    connect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));
    connect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
            this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);
}

void GSHAnalyseEditPreviewFrame::onAutoWidth()
{
    disconnect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
               this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);
    m_dZoomFactor = 1.0;
    m_pRichTextEdit->setZoomFactorValue(m_dZoomFactor);
//    disconnect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));
//    m_pSpbZoomFactor->setValue(m_dZoomFactor * 100);
    m_pZoomFactorSlider->setValue(m_dZoomFactor * 100);
//    connect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));
    connect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
            this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);
}

void GSHAnalyseEditPreviewFrame::onSaveToWord()
{
    GTime startTime = GTime::currentTime();
    if (m_bEditable)
    {
        QByteArray oByteArray;
        if (sameFloat(m_pRichTextEdit->saveToBuf(oByteArray), 0))
        {
            GVariant oVariant(oByteArray.toBase64());
            m_oAnalyseReportTBL.setValue(pfnFormatContent, oVariant);
            m_oAnalyseReportTBL.setValue(pfnPreviewContent, oVariant);
            savePreviewContents();
        }
        loadPreviewData();
        m_pRichTextEdit->saveAsDoc();
        loadContentData();
    }
    else
    {
        m_pRichTextEdit->saveAsDoc();
    }

    GTime endTime = GTime::currentTime();
    generateActionLog(startTime, endTime, m_pActExportWordFile->getName());

}

void GSHAnalyseEditPreviewFrame::onZoomFactor(int value)
{
    disconnect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
               this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);
    m_dZoomFactor = double(value) / 100;
    m_pRichTextEdit->setZoomFactorValue(m_dZoomFactor);
    connect(m_pRichTextEdit, &GLDRichTextEdit::zoomFactorChanged,
            this, &GSHAnalyseEditPreviewFrame::onResizeZoomFactor);
}

void GSHAnalyseEditPreviewFrame::onResizeZoomFactor()
{
    m_dZoomFactor = m_pRichTextEdit->zoomFactor();
//    disconnect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));
//    m_pSpbZoomFactor->setValue(m_dZoomFactor * 100);
    m_pZoomFactorSlider->setValue(m_dZoomFactor * 100);
    //    connect(m_pSpbZoomFactor, SIGNAL(valueChanged(int)), this, SLOT(onZoomFactor(int)));
}

void GSHAnalyseEditPreviewFrame::onSetZoomFactor(int value)
{
    if (NULL != m_pRichTextEdit)
    {
        m_pRichTextEdit->setZoomFactorValue(value * 1.0 / 100);
        m_pRichTextEdit->emitUpdateScroll();
    }
    m_dZoomFactor = value * 0.01;
    m_pZoomFactorLabel->setText(intToStr(value) + c_sPercent);
}

void GSHAnalyseEditPreviewFrame::onUpdateUIState(GLDRichTextEdit *edit, bool bMousePress)
{
    Q_ASSERT(NULL != edit);
    QColor oBackgroundColor = edit->selectionCharBackgroundColor();
    QColor oFontColor = edit->selectionCharFontColor();
    disconnect(m_pCmbBackgroundColor, SIGNAL(colorIndexChange(QColor)), this, SLOT(onSetBackground(QColor)));
    disconnect(m_pCmbColor, SIGNAL(colorIndexChange(QColor)), this, SLOT(onSetColor(QColor)));
    m_pCmbBackgroundColor->setCurrentColor(oBackgroundColor);
    m_pCmbColor->setCurrentColor(oFontColor);
    connect(m_pCmbBackgroundColor, SIGNAL(colorIndexChange(QColor)), this, SLOT(onSetBackground(QColor)));
    connect(m_pCmbColor, SIGNAL(colorIndexChange(QColor)), this, SLOT(onSetColor(QColor)));

    QFont oFont = edit->selectionCharFont(bMousePress);
    if (oFont.family() != m_pFontsBox->currentFont().family())
    {
        disconnect(m_pFontsBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onSetfont(const QFont &)));
        m_pFontsBox->setCurrentFont(oFont);
        connect(m_pFontsBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onSetfont(const QFont &)));
    }

    if (edit->canUndo())
    {
        m_pBtnUndo->setEnabled(true);
    }
    else
    {
        m_pBtnUndo->setEnabled(false);
    }
//    edit->setCursorFocus();

//    m_pSpinBox->setValue(oFont.pointSize());
}

void GSHAnalyseEditPreviewFrame::onPopupMenu()
{
    GLDMenuBar *pMenuBar = dynamic_cast<GLDMenuBar*>(getPopupMenu());
    pMenuBar->menu()->popup(QCursor::pos());
}

void GSHAnalyseEditPreviewFrame::onDoubleClickedInsertMacro(GSPRecord record)
{
    GString sMacroName = record.asString(pfnDescription);
    GString sMacroValue = record.asString(pfnContent);
    GString sInsertText = c_sMacroBeginFlag + sMacroName + c_sMacroEndFlag;
    if (m_bEditable)
    {
        m_pRichTextEdit->insertText(sInsertText);
    }
}

void GSHAnalyseEditPreviewFrame::onUpdateDefCustomMacros(const GMap<GString, GString> &macroValueHash)
{
    GMap<GString, GString>::const_iterator oIter = macroValueHash.begin();
    for (; oIter != macroValueHash.end(); ++oIter)
    {
        if (m_oMacroValueHash.find(oIter.key()) != m_oMacroValueHash.end())
        {
            m_oMacroValueHash[oIter.key()] = oIter.value();
        }
        else
        {
            m_oMacroValueHash.insert(oIter.key(), oIter.value());
        }
    }
}

void GSHAnalyseEditPreviewFrame::onBtnEditClicked(bool)
{
    GTime startTime = GTime::currentTime();
    GString sURL = "AnalyseEditPreview";
    GString sActionUUID;
    GString sActionName;
    if (m_bEditable)
    {
        sActionName = "GPS_GSHView";
        sActionUUID = sURL + "|" + sActionName;
        // 加载预览模式
        m_pBtnEdit->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Edit.png"));
        m_pBtnEdit->setIconSize(QSize(64, 22));
        m_pBtnEdit->setToolTip(c_sEditToolTip);

        QByteArray oByteArray;
        if (sameFloat(m_pRichTextEdit->saveToBuf(oByteArray), 0))
        {
            GVariant oVariant(oByteArray.toBase64());
            m_oAnalyseReportTBL.setValue(pfnFormatContent, oVariant);
//            m_oAnalyseReportTBL.setValue(pfnPreviewContent, oVariant);
            savePreviewContents();
        }
        loadPreviewData();
    }
    else
    {
        sActionName = "GPS_GSHEdit";
        sActionUUID = sURL + "|" + sActionName;
        // 加载编辑模式
        m_pBtnEdit->setIcon(QIcon(":/GSHAnalyse/Icons/Analyse/Preview.png"));
        m_pBtnEdit->setIconSize(QSize(64, 22));
        m_pBtnEdit->setToolTip(c_sEditPreViewTip);

//        QByteArray oByteArray;
//        if (sameFloat(m_pRichTextEdit->saveToBuf(oByteArray), 0))
//        {
//            GVariant oVariant(oByteArray.toBase64());
//            m_oAnalyseReportTBL.setValue(pfnFormatContent, oVariant);
//            m_oAnalyseReportTBL.setValue(pfnPreviewContent, oVariant);
//            savePreviewContents();
//        }

        loadContentData();
    }

    setItemsEnable();
    m_pRichTextEdit->setCursorFocus();
    m_pRichTextEdit->emitUpdateScroll();

    GTime endTime = GTime::currentTime();

    GString sLog;
    QByteArray oData;
    // 进行用户行为记录
    if ((NULL != uiService()) && (NULL != (uiService()->getMainFormIntf()))
            && (NULL != uiService()->getMainFormIntf()->getUserActivityLogger())
            && (uiService()->getService() != NULL))
    {
        sLog = uiService()->getService()->generateActionLog(sURL, sActionName, startTime, endTime, "");
        uiService()->getMainFormIntf()->getUserActivityLogger()->logAction(uiService()->getService()->getLogVersion(), sActionUUID, sLog);
    }
}

void GSHAnalyseEditPreviewFrame::onClipboardChanged()
{
    if (!m_pClipboard->text().isEmpty() && !m_bEditable)
    {
        m_pBtnPaste->setEnabled(true);
    }
    else
    {
        m_pBtnPaste->setEnabled(false);
    }
}

void GSHAnalyseEditPreviewFrame::onActImportFile()
{
    GTime startTime = GTime::currentTime();
    GString sPath = GBQIniCfgFile::instance()->readString(scnAnalyse, idnImportFilePath, "");
    QFileInfo oFileInfo(QFileDialog::getOpenFileName(this, QStringLiteral("选择文件"), sPath, QStringLiteral("*.rtf")));
    if (!oFileInfo.isFile() || 0 != oFileInfo.suffix().compare("rtf", Qt::CaseInsensitive))
    {
        return;
    }

    m_pRichTextEdit->setModify(false);
    m_pRichTextEdit->open(oFileInfo.absoluteFilePath());
    GBQIniCfgFile::instance()->writeString(scnAnalyse, idnImportFilePath, oFileInfo.absolutePath());
    GTime endTime = GTime::currentTime();
    generateActionLog(startTime, endTime, m_pActImportFile->getName());

}

void GSHAnalyseEditPreviewFrame::onActExportFile()
{
    GTime startTime = GTime::currentTime();
    GString sPath = GBQIniCfgFile::instance()->readString(scnAnalyse, idnExportFilePath, "");
    QString sDoc = ";;Office Word (*.doc)";;

    QString sSelectedFilter;
    QFileInfo oFileInfo(
                QFileDialog::getSaveFileName(
                    this, QStringLiteral("另存文件"), sPath,
                    QStringLiteral("文件 (*.rtf);;文件 (*.txt)%1").arg(sDoc), &sSelectedFilter));
    QString sFile = oFileInfo.absoluteFilePath();

    if (QStringLiteral("文件 (*.rtf)") == sSelectedFilter)
    {
        if (!sFile.endsWith(".rtf", Qt::CaseInsensitive))
        {
            sFile += ".rtf";
        }

        if (m_bEditable)
        {
            m_pRichTextEdit->saveAs(sFile, true);
        }
        else
        {
            loadContentData();
            m_pRichTextEdit->saveAs(sFile, true);
            loadPreviewData();
        }
    }
    else if (QStringLiteral("文件 (*.txt)") == sSelectedFilter)
    {
        if (!sFile.endsWith(".txt", Qt::CaseInsensitive))
        {
            sFile += ".txt";
        }

        QFile oFile(sFile);
        if (oFile.open(QIODevice::WriteOnly))
        {
            QTextStream ts(&oFile);
            ts << m_pRichTextEdit->getAllText();
            oFile.close();
        }
    }
    else if (QStringLiteral("Office Word (*.doc)") == sSelectedFilter)
    {
        //onSaveToWord();

        //        if (!sFile.endsWith(".doc", Qt::CaseInsensitive))
        //        {
        //            sFile += ".doc";
        //        }

        //        // 拷贝粘贴的方式
        //        /*QPoint pos = pEdit->getTextCursor();
        //        pEdit->selectAll();
        //        pEdit->copy();
        //        pEdit->setTextCursor(pos);*/

        //        if (m_pRichTextEdit->getFileName().isEmpty())
        //        {
        //            // 临时文件目录
        //            QString sTempFile;
        //            QDir oTempDir = QDir::temp();
        //            QString sUid = QUuid::createUuid().toString().replace("-", "_");
        //            sUid.chop(1);
        //            sUid.remove(0, 1);
        //            sTempFile = oTempDir.absoluteFilePath(sUid + ".rtf");
        //            m_pRichTextEdit->saveAs(sTempFile, false);

        //            saveToDoc(sTempFile, sFile, 1);
        //            QFile::remove(sTempFile);
        //        }
        //        else
        //        {
        //            m_pRichTextEdit->save();
        //            saveToDoc(m_pRichTextEdit->getFileName(), sFile, 1);
        //        }
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
            m_pRichTextEdit->saveAs(tempFile, false);

            m_pRichTextEdit->saveToDoc(sFile, tempFile);
            QFile::remove(tempFile);
    }
    m_pRichTextEdit->setCursorFocus();

    GBQIniCfgFile::instance()->writeString(scnAnalyse, idnExportFilePath, oFileInfo.absolutePath());
    GTime endTime = GTime::currentTime();
    generateActionLog(startTime, endTime, m_pActExportFile->getName());

}

void GSHAnalyseEditPreviewFrame::generateActionLog(GTime startTime, GTime endTime, GString actionName)
{
    GString sLog;
    QByteArray oData;
    GString m_sURL = getFrameURL();
    GString sActionUUID = m_sURL + "|" + actionName;

    // 进行用户行为记录
     if ((NULL != uiService()) && (NULL != (uiService()->getMainFormIntf()))
             && (NULL != uiService()->getMainFormIntf()->getUserActivityLogger())
             && (uiService()->getService() != NULL))
     {
         sLog = uiService()->getService()->generateActionLog(m_sURL, actionName, startTime, endTime, "");
         uiService()->getMainFormIntf()->getUserActivityLogger()->logAction(uiService()->getService()->getLogVersion(), sActionUUID, sLog);
     }
}

void GSHAnalyseEditPreviewFrame::onMacroItemTirggered(const GString &text)
{
    //    if (NULL == m_pContentEdit)
    //    {
    //        return;
    //    }

    //    QTextCursor cursor = getSelectionEndTextCursor();
    //    GString sContent = m_pContentEdit->toPlainText();
    //    GString sLeftText = sContent.left(cursor.anchor());
    //    GString sRightText = sContent.right(sContent.length() - cursor.anchor());

    //    if (!m_bEditable)
    //    {
    //        sLeftText = GString("%1%2").arg(sLeftText, text);
    //    }
    //    else
    //    {
    //        sLeftText = GString("%1%2").arg(sLeftText, getValueByMacro(text));
    //    }

    //    m_pContentEdit->setText(GString("%1%2").arg(sLeftText, sRightText));
    //    cursor.setPosition(sLeftText.length());
    //    m_pContentEdit->setTextCursor(cursor);
}

/* GSHAnalyseEditPreviewLayout */

GSHAnalyseEditPreviewLayout::GSHAnalyseEditPreviewLayout(GLDFrame *owner, IGLDFrameNavigator *navigator)
    : GLDFrameLayout(owner, navigator),
      m_pMainLayout(NULL)
{
}

GSHAnalyseEditPreviewLayout::~GSHAnalyseEditPreviewLayout()
{
    freeAndNil(m_pMainLayout);
}

void GSHAnalyseEditPreviewLayout::alignFrame(const QList<QWidget*> &frames)
{
    for (int i = 0; i != frames.count(); ++i)
    {
        m_pBoxLayout->addWidget(frames[i]);
    }
}

void GSHAnalyseEditPreviewLayout::applyLayout()
{
    if (NULL == m_pBoxLayout)
    {
        m_pBoxLayout = new QHBoxLayout(m_pOwner);
    }

    if (NULL == m_pMainLayout)
    {
        m_pMainLayout = new QVBoxLayout(m_pOwner);
    }

    m_pBoxLayout->addLayout(m_pMainLayout);
    GLDFrameLayout::applyLayout();
}

void GSHAnalyseEditPreviewLayout::addWidget(QWidget *widget)
{
    m_pMainLayout->addWidget(widget);
}

void GSHAnalyseEditPreviewLayout::addLayout(QBoxLayout *layout)
{
    m_pMainLayout->addLayout(layout);
}
