#include "ThumbnailManager.h"

#include <private/qlistview_p.h>

#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QWaitCondition>

struct Page
{
    Page(const QUrl &pSourceUrl, QImage *pPix = 0)
        : sourceUrl(pSourceUrl)
        , w(0)
        , h(0)
        , titleNo(0)
        , pix(pPix)
    {

    }

    void setImage(QImage *img)
    {
        if (img)
        {
            if (pix)
            {
                delete pix;
                pix = 0;
            }
            pix = img;

            w = pix ? pix->width() : 0;
            h = pix ? pix->height() : 0;
        }
    }

    QUrl sourceUrl;
    int w;
    int h;
    int titleNo;
    QImage *pix;
};
Q_DECLARE_METATYPE(Page*)

class DPSlider : public QSlider
{
    Q_OBJECT

public:
    DPSlider(QWidget *parent = 0)
        : QSlider(parent)
    {
        setOrientation(Qt::Horizontal);
    }

protected:
    void mouseReleaseEvent(QMouseEvent *ev)
    {
        emit issreleased();
        QSlider::mouseReleaseEvent(ev);
    }

signals:
    void issreleased();

};

class DynPicturesManagerlPrivate
{
public:
    DynPicturesManagerlPrivate(DynPicturesManager *pq)
        : q(pq)
        , mModel(0)
        , mDelegate(0)
        , mGeneratorThread(0)
        , mCentralWidget(0)
        , mCleaningMemTimer(0)
        , mBlockCleanerTimer(false)
    {
        mModel = new DPListModel(&pageList);
        mDelegate = new DPItemDelegate();
        QSize emptyPageSize = QSize(mCellSize, mCellSize);
        emptyImagePatterns.insert(DynPicturesManager::sizeToString(emptyPageSize), createBlancImage(emptyPageSize));
        mModel->setEmptyImagePatterns(&emptyImagePatterns);
        setupUi();
        foreach (DPListView *view, mRegisteredViews) {
            view->setModel(mModel);
            view->setItemDelegate(mDelegate);

            QObject::connect(view, SIGNAL(manipulateContentsStarted()), q, SLOT(pauseCleaningTimer()));
            QObject::connect(view, SIGNAL(manipulateContentsFinished()), q, SLOT(playCleaningTimer()));
            QObject::connect(mSlider, SIGNAL(issreleased()), view, SLOT(reactOnSliderReleased()));
            QObject::connect(q, SIGNAL(memoryCleaned()), view, SLOT(reactOnSliderReleased()));
            QObject::connect(view, SIGNAL(iconSizeChanged(QSize)), q, SLOT(setMaxIconSize(QSize)));
        }
        mCleaningMemTimer = new QTimer();
        QObject::connect(mCleaningMemTimer, SIGNAL(timeout()), q, SLOT(cleanMemory()));
        if (Globals::useMemoryCleaner) {
            QTimer::singleShot(0, q, SLOT(startCleaningTimer()));
            qDebug() << "Starting application using memory cleaner timer";
        }
    }

    ~DynPicturesManagerlPrivate()
    {
        if(mCleaningMemTimer) {
            delete mCleaningMemTimer;
        }
        if (mCentralWidget) {
            delete mCentralWidget;
        }
        qDeleteAll(pageList);
        pageList.clear();

        if (mModel) {
            delete mModel;
        }
        if (mGeneratorThread) {
            delete mGeneratorThread;
        }
    }

    void createPages()
    {
        if (!mGeneratorThread) {
            return;
        }

        qint64 pageCount = mGeneratorThread->imageCount();
        QTime currentTime = QTime::currentTime();
        for (int i = 0; i < pageCount; i++) {
                Page *curPage = new Page(QUrl());
                pageList.append(curPage);
        }
        qDebug() << "creation time " << currentTime.msecsTo(QTime::currentTime());
    }

    DPListView *view(int index) {
        if (index < 0 && index > mRegisteredViews.count()) {
            return 0;
        }
        return mRegisteredViews.at(index);
    }

    QImage *createBlancImage(const QSize &size)
    {
        QImage *newPix = new QImage(size, QImage::Format_ARGB32);
        newPix->fill(Qt::transparent);
        QPainter painter;
        painter.begin(newPix);
        painter.setRenderHints(QPainter::Antialiasing, true);

        int shadowThickness = 3;
        int width = size.width() - 1;
        int height = size.height() - 1;

        QLinearGradient gradient;
        gradient.setStart(0,0);
        gradient.setFinalStop(width,0);
        QColor grey1(150,150,150,125);
        QColor grey2(225,225,225,125);

        gradient.setColorAt((qreal)0, grey1);
        gradient.setColorAt((qreal)1, grey2);

        QBrush brush(gradient);
        painter.setBrush( brush);

        painter.setPen(Qt::NoPen);

        QPointF topLeft (shadowThickness+0, shadowThickness+0);
        QPointF bottomRight (width, height);

        QRectF rect (topLeft, bottomRight);

        painter.drawRoundRect(rect,20,20);

        // draw the top box, the visible one
        QBrush brush2(QColor(250,250,250,255),Qt::SolidPattern);

        painter.setBrush(brush2);

        QPointF topLeft2 (0, 0);
        QPointF bottomRight2 (width - shadowThickness, height - shadowThickness);

        QRectF rect2 (topLeft2, bottomRight2);

        painter.drawRoundRect(rect2,25,25);
        painter.end();

        return newPix;
    }

    void installPageGenerator(DPImageServicer *generator)
    {
        if (!generator)
        {
            return;
        }

//        if (mGeneratorThread) {
//            delete mGeneratorThread;
//        }
        mModel->clear();
        DPImageServicer *oldGenerator = mGeneratorThread;

        mGeneratorThread = generator;
        if (mGeneratorThread)
        {

            createPages();

            foreach (DPListView *view, mRegisteredViews)
            {
                QObject::disconnect(view, SIGNAL(sendRequest(DPImageRequest)), oldGenerator, SLOT(replyOnRequest(DPImageRequest)));
                QObject::connect(view, SIGNAL(sendRequest(DPImageRequest)), mGeneratorThread, SLOT(replyOnRequest(DPImageRequest)));
            }

            QObject::connect(mGeneratorThread, SIGNAL(sendReply(DPImageReply)), mModel, SLOT(reactOnImageReply(DPImageReply)));
            QObject::connect(q, SIGNAL(requestCleanImageServicerQueue()), mGeneratorThread, SLOT(replyCleanImageServicerQueue()));

            mGeneratorThread->start(QThread::LowPriority);
        }
    }

    void changeServicerData(const QVariant &newData, int role)
    {
        if (mGeneratorThread)
        {
            mGeneratorThread->setData(newData, role);
        }
    }

    void reload()
    {
        mModel->clear();
        createPages();
        emit q->sliderReleased();
    }

    void setupUi()
    {
        DPListView *mMainView = new DPListView();
        mRegisteredViews.append(mMainView);
        DPListView *mThumbView = new DPListView();
        mThumbView->setAdditionalFlags(DPListView::fAutoExpanding);
        mRegisteredViews.append(mThumbView);

        mCentralWidget = new QWidget();
        QVBoxLayout *mainLayer = new QVBoxLayout();
        mainLayer->setContentsMargins(2, 2, 2, 2);

        QSplitter *widgetsSplitter = new QSplitter(Qt::Horizontal);
        widgetsSplitter->addWidget(mThumbView);
        widgetsSplitter->addWidget(mMainView);
        widgetsSplitter->setStretchFactor(0, 0);
        widgetsSplitter->setStretchFactor(1, 1);

        QHBoxLayout *sliderLayout = new QHBoxLayout();
        sliderLayout->setContentsMargins(0, 0, 0, 0);
        sliderLayout->addSpacerItem(new QSpacerItem(300, 0, QSizePolicy::Expanding));
        QPushButton *magicButton = new QPushButton("Don't push");
        QObject::connect(magicButton, SIGNAL(clicked()), q, SLOT(cleanMemory()));
        sliderLayout->addWidget(magicButton);
        mSlider = new DPSlider();
        mSlider->setMinimum(Globals::defaultCellSize);
        mSlider->setMaximum(Globals::maxCellSize);

        QObject::connect(mSlider, SIGNAL(valueChanged(int)), mMainView, SLOT(setNewGridSize(int)));

        mSlider->setValue(150);
        sliderLayout->addWidget(mSlider);

        mainLayer->addWidget(widgetsSplitter);
        mainLayer->addLayout(sliderLayout);


        mCentralWidget->setLayout(mainLayer);
        mCentralWidget->setGeometry(0, 0, 800, 600);
        mCentralWidget->setVisible(false);
    }

    void setMaxIconSize(const QSize &pSz, DPListView *senderList)
    {
        qDebug() << "old icon size" << mMaxIconSize;
        QSize newSz = pSz;
        if (pSz.width() <= mMaxIconSize)
        {
            foreach (DPListView *view, mRegisteredViews)
            {
                if (view == senderList)
                {
                    continue;
                }
                QSize currentSize = view->iconSize();
                if (currentSize.width() >= pSz.width())
                {
                    return;
                }
            }
        }

        mMaxIconSize = newSz.width();
        qDebug() << "new icon size" << mMaxIconSize;
    }

private:
    DynPicturesManager *q;
    QUrl storageUrl;
    QList<Page*> pageList;
    QList<DPListView*> mRegisteredViews;
    QSlider *mSlider;
    DPListModel *mModel;
    DPItemDelegate *mDelegate;
    DynPicturesManager::EmptyPatterns emptyImagePatterns;
    DPImageServicer *mGeneratorThread;
    QWidget *mCentralWidget;
    QTimer *mCleaningMemTimer;
    bool mBlockCleanerTimer;

    static int mCellSize;
    static int mMaxIconSize;

    friend class DynPicturesManager;
};

int DynPicturesManagerlPrivate::mCellSize = Globals::defaultCellSize;
int DynPicturesManagerlPrivate::mMaxIconSize = Globals::defaultCellSize;

DynPicturesManager::DynPicturesManager(QObject *parent)
    : QObject(parent)
    , d(new DynPicturesManagerlPrivate(this))
{
    qRegisterMetaType<DPImageRequest>("DPImageRequest");
    qRegisterMetaType<DPImageReply>("DPImageReply");
}

DynPicturesManager::~DynPicturesManager()
{
    if (d) {
        delete d;
    }
}

void DynPicturesManager::setVisible(bool pVisible)
{
    Q_ASSERT(d->mCentralWidget);
    d->mCentralWidget->setVisible(pVisible);
}

DPListView *DynPicturesManager::view(int index) const
{
    return d->view(index);
}

QWidget *DynPicturesManager::widget() const
{
    return d->mCentralWidget;
}

void DynPicturesManager::installPageGenerator(DPImageServicer *generator)
{
    d->installPageGenerator(generator);
}

void DynPicturesManager::changeServicerData(const QVariant &newData, int role)
{
    d->changeServicerData(newData, role);
}

void DynPicturesManager::reload()
{
    d->reload();
}

void DynPicturesManager::setCellSize(int newSize)
{
    DynPicturesManagerlPrivate::mCellSize = newSize;
}

QSize DynPicturesManager::gridSize()
{
    return QSize(DynPicturesManagerlPrivate::mCellSize, DynPicturesManagerlPrivate::mCellSize);
}

QSize DynPicturesManager::iconSize()
{
    return iconSizeFromGridSize(gridSize());
}

QSize DynPicturesManager::iconSizeFromGridSize(const QSize &gridSize)
{
    return gridSize - QSize(40, 40);
}

int DynPicturesManager::maxIconSize()
{
    return DynPicturesManagerlPrivate::mMaxIconSize;
}

QString DynPicturesManager::sizeToString(const QSize &pSize)
{
    return QString("%1x%2").arg(pSize.width()).arg(pSize.height());
}

namespace Utils {
class SpanUnionHandler {
public:

    struct Span {
        Span(int pmin, int pmax)
            : mMin(qMin(pmin, pmax)), mMax(qMax(pmin, pmax))
        {;}
        Span() : mMin(-1), mMax(-1) {;}

        bool contains(int pValue)
        {
            return (pValue >= mMin) && (pValue <= mMax);
        }

        operator bool() {return mMin < 0 || mMax < 0;} //only positive and equal to zero values allowed

        int mMin;
        int mMax;
    };


    typedef QList<Span> SpanList;
    typedef SpanList::Iterator Iterator;

    void print()
    {
        for (int i = 0; i < mUnion.count(); i++) {
            Span nextSpan = mUnion.at(i);
            qDebug() << "Span" << i << "{" << nextSpan.mMin << nextSpan.mMax << "}";
        }

    }
    bool contains(int index)
    {
        foreach (Span nextSpan, mUnion) {
            if (nextSpan.contains(index)) {
                return true;
            }
        }

        return false;
    }



    void addSpan(Span pSpan)
    {
        if (mUnion.isEmpty()) {
            mUnion.append(pSpan);
        } else {
            processNext(pSpan, 0);
        }
    }

private:

    void processNext(Span candidate, uint index)
    {
        Span &nextSpan = mUnion[index];
        if (candidate.mMin < nextSpan.mMin) {
            if (candidate.mMax < nextSpan.mMin) { //Span starts and ends before the current span within the union
                mUnion.insert(index, candidate);
                return;
            } else if (nextSpan.contains(candidate.mMax)) { //Starts before and ends within the current span of the union
                nextSpan.mMin = candidate.mMin;
                return;
            } else { //Span starts before and ends after the current span
                mUnion.removeAt(index);
                if (index == static_cast<uint>(mUnion.size())) {
                    mUnion.append(candidate); //no more indecies left
                } else {
                    processNextExpanding(candidate, index);
                }
                return;
            }
        } else if (nextSpan.contains(candidate.mMin)) {
            if (nextSpan.contains(candidate.mMax)) {//Span origin lies within the current span and ends there
                return;
            } else { //Span origin lies within the current span and ends outside it
                candidate.mMin = nextSpan.mMin;
                mUnion.removeAt(index);
                if (index == static_cast<uint>(mUnion.size())) {
                    mUnion.append(candidate); //no more indecies left
                } else {
                    processNextExpanding(candidate, index);
                }
                return;
            }
        }

        if (++index < static_cast<uint>(mUnion.size())) { //There is at least one next Span in union
            processNext(candidate, index);
        } else { //no more indecies left
            mUnion.append(candidate);
        }
    }

    void processNextExpanding(Span candidate, uint index)
    {
        Span &nextSpan = mUnion[index];
        if (candidate.mMax < nextSpan.mMin) { //candidate ends before current span origin
            mUnion.insert(index, candidate);
        } else if (nextSpan.contains(candidate.mMax)) { //candidate ends within the current span
            nextSpan.mMin = candidate.mMin;
        } else { //current span is overlapped by candidate
            mUnion.removeAt(index);
            if (index == static_cast<uint>(mUnion.size())) {
                mUnion.append(candidate); //no more indecies left
            } else {
                processNextExpanding(candidate, index);
            }
        }
    }

private:
    SpanList mUnion;

};
} //namespace Utils

void DynPicturesManager::cleanMemory()
{
    if (d->mBlockCleanerTimer) {
        return;
    }

    QTime currentTime = QTime::currentTime();
    emit requestCleanImageServicerQueue();

    Utils::SpanUnionHandler spHandler;
    bool viewsAreHidden = true;
    foreach (DPListView *view, d->mRegisteredViews) {
        if (view->isVisible()) {
            QVector<QModelIndex> cachIndexes(view->cachedIndexes());
            spHandler.addSpan(Utils::SpanUnionHandler::Span(cachIndexes.first().row(), cachIndexes.last().row()));
            qDebug() << "first" << cachIndexes.first().row() << "last" << cachIndexes.last().row();
            viewsAreHidden = false;
        }
    }

    if (viewsAreHidden) {
        return;
    }

    spHandler.print();

    int counter = 0;
    for (int i = 0; i < d->pageList.count(); i++) {
        Page *curPage = d->pageList.at(i);
        if (curPage->pix && !spHandler.contains(i)) {
            counter++;
            delete curPage->pix;
            curPage->pix = 0;
        }
    }

    QPixmapCache::clear();
    qDebug() << "Cleaning memory finished. Elapsed time" << currentTime.msecsTo(QTime::currentTime());
    qDebug() << "alive objects" << counter;

    emit memoryCleaned();
}

void DynPicturesManager::startCleaningTimer()
{
    if (d->mCleaningMemTimer) {
        d->mCleaningMemTimer->start(Globals::cleanerTimerInterval);
    }
}

void DynPicturesManager::pauseCleaningTimer()
{
    d->mBlockCleanerTimer = true;
}

void DynPicturesManager::playCleaningTimer()
{
    d->mBlockCleanerTimer = false;
}

void DynPicturesManager::setMaxIconSize(const QSize &pSz) {
    DPListView *senderList = qobject_cast<DPListView*>(sender());
    if (senderList) {
        d->setMaxIconSize(pSz, senderList);
    }
}

class DPListModelPrivate
{

public:
    DPListModelPrivate(QList<Page*> *pPageList)
        : pageList(pPageList)
        , emptyImagePatterns(0)
    {
    }

    ~DPListModelPrivate()
    {
        foreach(QImage *pix, *emptyImagePatterns) {
            delete pix;
            pix = 0;
        }
        emptyImagePatterns->clear();
    }
    void clear()
    {
        qDeleteAll(*pageList);
        pageList->clear();
    }

private:
    QList<Page*> *pageList;
    DynPicturesManager::EmptyPatterns *emptyImagePatterns;

    friend class DPListModel;
};

DPListModel::DPListModel(QList<Page*> *pPageList, QObject *parent)
    : QAbstractListModel(parent)
    , d(new DPListModelPrivate(pPageList))
{
}

DPListModel::~DPListModel()
{
    if (d) {
        delete d;
    }
}

void DPListModel::setEmptyImagePatterns(DynPicturesManager::EmptyPatterns *pPatterns)
{
    d->emptyImagePatterns = pPatterns;
}

void DPListModel::clear()
{
    beginResetModel();
    d->clear();
    endResetModel();
}

int DPListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->pageList->count();
}

int DPListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant DPListModel::data(const QModelIndex &index, int role) const
{    
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Globals::pageRole :
        return QVariant::fromValue(d->pageList->at(index.row()));

    case Qt::TextAlignmentRole :
        return Qt::AlignCenter;
        break;
    case Qt::DisplayRole :
        return QString("Page %1").arg(index.row());
        break;
    case Qt::DecorationRole :
        QImage *&curPix = d->pageList->at(index.row())->pix;

        if (curPix) {
            return *curPix;
        } else {
            return 0;
        }

        break;
    }

    return QVariant();
}

void DPListModel::reactOnImageReply(DPImageReply reply)
{
    int indexRow = reply.pageNo;
    if (d->pageList && indexRow != -1 && indexRow < d->pageList->count()) {
        Page *curPage = d->pageList->at(indexRow);
        if (!reply.image.isNull()) {
            curPage->setImage(new QImage(reply.image));
        }
        QModelIndex curIndex  = index(indexRow);
        emit dataChanged(curIndex, curIndex);
    }
}

DPListView::DPListView(QWidget *parent)
    : QListView(parent)
    , mAdditionalFlags(0)
{
    setViewMode(IconMode);
    setGridSize(DynPicturesManager::gridSize());
    setIconSize(DynPicturesManager::iconSize());
    setResizeMode(Adjust);
    setSelectionMode(ContiguousSelection);
}

QVector<QModelIndex> DPListView::visibleInArea(const QRect &pArea)
{
    QRect rect(pArea.x() + horizontalOffset(), pArea.y() + verticalOffset(), pArea.width(), pArea.height());
    QTime currentTime = QTime::currentTime();
    QVector<QModelIndex> intersectVector = privPtr()->intersectingSet(rect);
    qDebug() << "counting vector" << currentTime.msecsTo(QTime::currentTime());

    currentTime = QTime::currentTime();
    for (int i  = 0; i < intersectVector.count(); i++) {
        QModelIndex index = intersectVector.at(i);
        if (!index.isValid() || !visualRect(index).intersects(pArea)) {
            intersectVector.remove(i);
        }
    }
    qDebug() << "processing vector" << currentTime.msecsTo(QTime::currentTime());

    return intersectVector;
}

//QVector<QModelIndex> DPListView::visibleInArea(const QRect &pArea)
//{
//    QVector<QModelIndex> result;

////    QRect rect(pArea.x() + horizontalOffset(), pArea.y() + verticalOffset(), pArea.width(), pArea.height());
//    QRect rect = pArea;

//    int gridHCenterOffsetTop = gridSize().width() / 2;
//    int gridVBottomOffsetTop = gridSize().height() / 2;
//    QPoint topLeftOffset(gridHCenterOffsetTop, gridVBottomOffsetTop);

//    int gridHCenterOffsetBottom = (rect.width() % gridSize().width()) + (gridSize().width() / 2);
//    int gridVBottomOffsetBottom = gridSize().height() / 2;
//    QPoint bottomRightOffset(gridHCenterOffsetBottom, gridVBottomOffsetBottom);

//    QModelIndex topLeftIndex = indexAt(rect.topLeft() + topLeftOffset);
//    QModelIndex bottomRightIndex = indexAt(rect.bottomRight() - bottomRightOffset);

//    if (!topLeftIndex.isValid() || ! bottomRightIndex.isValid()) {
//        return QVector<QModelIndex>();
//    }
//    result.append(topLeftIndex);
//    for (int i = topLeftIndex.row() + 1; i <= bottomRightIndex.row(); i++) {
//        QModelIndex curIndex = model()->index(i, 0);
//        if(curIndex.isValid()) {
//            result.append(curIndex);
//        }
//    }

//    qDebug() << "result is" << result;
//    return result;
//}


QVector<QModelIndex> DPListView::cachedIndexes()
{
    QTime currentTime = QTime::currentTime();
    QVector<QModelIndex>visibleIndexesVector(visibleInArea(viewport()->geometry()));
    if (!visibleIndexesVector.count()) {
        return visibleIndexesVector;
    }

    int numIndexes = visibleIndexesVector.count();
    int minIndex = visibleIndexesVector.first().row();
    int maxIndex = visibleIndexesVector.first().row();
    foreach (QModelIndex index, visibleIndexesVector) {
        int indexRow = index.row();
        if (indexRow < minIndex) {
            minIndex = indexRow;
        } else if (indexRow > maxIndex) {
            maxIndex = indexRow;
        }
    }
    int numIndexesToCache = numIndexes * Globals::saveMemoryMultipler;
    //filling indexes above the visible
    int startIndex = qMax(0, minIndex - numIndexesToCache);
    for (int i = minIndex - 1; i >= startIndex; i--) {
        QModelIndex curIndex = model()->index(i, 0);
        visibleIndexesVector.prepend(curIndex);
    }
    //filling indexes under the visible
    int endIndex = qMin(model()->rowCount() - 1, maxIndex + numIndexesToCache);
    for (int i = maxIndex + 1; i <= endIndex; i++) {
        QModelIndex curIndex = model()->index(i, 0);
        visibleIndexesVector.append(curIndex);
    }
    qDebug() << "cachedIndexes() function time" << currentTime.msecsTo(QTime::currentTime());

    return visibleIndexesVector;
}

void DPListView::setAdditionalFlags(eAdditionalFlags flags)
{
    mAdditionalFlags |= flags;
}

DPListView::eAdditionalFlags DPListView::additionalFlags()
{
    return mAdditionalFlags;
}

void DPListView::resizeEvent(QResizeEvent *e)
{
    if (mAdditionalFlags & fAutoExpanding) {
        QSize newGridSize = QSize(width() - 25, width() - 25);
        setGridSize(newGridSize);
        QSize newIconSz = DynPicturesManager::iconSizeFromGridSize(newGridSize);
        setIconSize(newIconSz);

        qDebug() << "newSize is" << gridSize() << iconSize();

        emit iconSizeChanged(newIconSz);
    }
    updateEmptyPagesData();
    QListView::resizeEvent(e);
}

void DPListView::scrollContentsBy(int dx, int dy)
{
//    qDebug() << "content is scrolled by dx" << dx << "dy" << dy;
    if (dy) {
        updateEmptyPagesData();
    }
    QListView::scrollContentsBy(dx, dy);
}

void DPListView::setNewGridSize(int newSize)
{
    QSize gridSz = QSize(newSize, newSize);
    setGridSize(gridSz);
    QSize iconSz = DynPicturesManager::iconSizeFromGridSize(gridSz);
    setIconSize(iconSz);

    emit iconSizeChanged(iconSz);
}

void DPListView::reactOnSliderReleased()
{
    updateEmptyPagesData();
}

QListViewPrivate* DPListView::privPtr()
{
    return reinterpret_cast<QListViewPrivate*>(qGetPtrHelper(d_ptr));
}

void DPListView::updateEmptyPagesData()
{
    emit manipulateContentsStarted();

    QTime currentTime = QTime::currentTime();
    QVector<QModelIndex>visibleIndexesVector(visibleInArea(viewport()->geometry()));
    if (!visibleIndexesVector.count()) {
        return;
    }

    int numIndexes = visibleIndexesVector.count();
    int minIndex = visibleIndexesVector.first().row();
    int maxIndex = visibleIndexesVector.last().row();
    int requestWidth = gridSize().width() - 30;
    int requestHeight = gridSize().height() - 30;

    int numIndexesToCache = numIndexes * Globals::saveMemoryMultipler;
    //caching rect above the visible
    int startIndex = qMax(0, minIndex - numIndexesToCache);
    for (int i = startIndex; i < minIndex; i++) {
        Page *curPage = model()->index(i, 0).data(Globals::pageRole).value<Page*>();
        QImage *curPix = curPage->pix;
        if (!curPix || requestWidth > curPix->width() || requestHeight > curPix->height()) {
            DPImageRequest request(requestWidth, requestHeight, i, 0);
            emit sendRequest(request);
        }
    }

    //caching rect under the visible
    int endIndex = qMin(model()->rowCount() - 1, maxIndex + numIndexesToCache);
    for (int i = endIndex; i > maxIndex; i--) {
        Page *curPage = model()->index(i, 0).data(Globals::pageRole).value<Page*>();
        QImage *curPix = curPage->pix;
        if (!curPix || requestWidth > curPix->width() || requestHeight > curPix->height()) {
            DPImageRequest request(requestWidth, requestHeight, i, 0);
            emit sendRequest(request);
        }
    }

    for (int l = visibleIndexesVector.count() - 1; l >=0; l--) {
        QModelIndex index = visibleIndexesVector.at(l);
        int indexRow = index.row();
        if (indexRow < minIndex) {
            minIndex = indexRow;
        } else if (indexRow > maxIndex) {
            maxIndex = indexRow;
        }
        Page *curPage = index.data(Globals::pageRole).value<Page*>();
        QImage *curPix = curPage->pix;
        if (!curPix || requestWidth > curPix->width() || requestHeight > curPix->height()) {
            DPImageRequest request(requestWidth, requestHeight, index.row(), 0);
            emit sendRequest(request);
        }
    }

    qDebug() << "numIndexes" << numIndexes << "numIndexesToCache" << numIndexesToCache
             << "start above cache from" << startIndex << "endCache with" << minIndex << endl
             << "visible index from " << minIndex << "to" << maxIndex << endl
             << "start bottom cache from" << maxIndex + 1 << "end cache with" << endIndex << endl;

    qDebug() << "sendRequest time " << currentTime.msecsTo(QTime::currentTime());

    emit manipulateContentsFinished();
}

DPItemDelegate::DPItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

void DPItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    const QWidget *widget = opt.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);

    painter->setPen(0xAAAAAA);
    painter->drawRoundRect(option.rect, 15, 15);

    painter->restore();
}



void DPItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::FontRole);
    if (value.isValid() && !value.isNull()) {
        option->font = qvariant_cast<QFont>(value).resolve(option->font);
        option->fontMetrics = QFontMetrics(option->font);
    }

    value = index.data(Qt::TextAlignmentRole);
    if (value.isValid() && !value.isNull())
        option->displayAlignment = Qt::Alignment(value.toInt());

    value = index.data(Qt::ForegroundRole);
    if (value.canConvert<QBrush>())
        option->palette.setBrush(QPalette::Text, qvariant_cast<QBrush>(value));

    if (QStyleOptionViewItemV4 *v4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option)) {
        v4->index = index;
        QVariant value = index.data(Qt::CheckStateRole);
        if (value.isValid() && !value.isNull()) {
            v4->features |= QStyleOptionViewItemV2::HasCheckIndicator;
            v4->checkState = static_cast<Qt::CheckState>(value.toInt());
        }

        value = index.data(Qt::DecorationRole);
        if (value.isValid() && !value.isNull()) {
            v4->features |= QStyleOptionViewItemV2::HasDecoration;
            switch (value.type()) {
            case QVariant::Icon: {
                v4->icon = qvariant_cast<QIcon>(value);
                QIcon::Mode mode;
                if (!(option->state & QStyle::State_Enabled))
                    mode = QIcon::Disabled;
                else if (option->state & QStyle::State_Selected)
                    mode = QIcon::Selected;
                else
                    mode = QIcon::Normal;
                QIcon::State state = option->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
                v4->decorationSize = v4->icon.actualSize(option->decorationSize, mode, state);
                break;
            }
            case QVariant::Color: {
                QPixmap pixmap(option->decorationSize);
                pixmap.fill(qvariant_cast<QColor>(value));
                v4->icon = QIcon(pixmap);
                break;
            }
            case QVariant::Image: {
                QImage image = qvariant_cast<QImage>(value);
                const QListView *curView = qobject_cast<const QListView*>(v4->widget);
                v4->icon = QIcon(QPixmap::fromImage(image));
                v4->decorationSize = curView->iconSize();
                break;
            }
            case QVariant::Pixmap: {
                QPixmap pixmap = qvariant_cast<QPixmap>(value);
                v4->icon = QIcon(pixmap);
                v4->decorationSize = pixmap.size();
                break;
            }
            default:
                break;
            }
        }

        value = index.data(Qt::DisplayRole);
        if (value.isValid() && !value.isNull()) {
            v4->features |= QStyleOptionViewItemV2::HasDisplay;
            v4->text = displayText(value, v4->locale);
        }

        v4->backgroundBrush = qvariant_cast<QBrush>(index.data(Qt::BackgroundRole));
    }
}

class DPImageServicerPrivate
{
    DPImageServicerPrivate(DPImageServicer *pq)
        : abort(false)
        , q(pq)
    {

    }
    ~DPImageServicerPrivate()
    {
        mMutex.lock();
        abort = true;
        mCondition.wakeOne();
        mMutex.unlock();

        q->wait();
    }

    void addRequest(DPImageRequest req)
    {
        mMutex.lock();
        switch (req.pr) {
        case DPImageRequest::veryLow :
            requests.append(req);
            break;
        case DPImageRequest::veryHigh :
            requests.prepend(req);
            break;
        }
        mCondition.wakeOne();
        mMutex.unlock();
    }

    void run()
    {
        forever {
            DPImageRequest curReq;
            mMutex.lock();
            if (requests.isEmpty()) {
                curReq = DPImageRequest();
            } else {
                curReq = requests.at(0);
                requests.remove(0);
            }
            mMutex.unlock();

            if (abort) {
                return;
            }
            if (curReq.isValid()) {
                //                mMutex.lock();
                QImage image = q->imageForindex(curReq.pageNo);
                //                mMutex.unlock();
                if (!image.isNull() && image.size() != QSize(curReq.w, curReq.h)) {
                    image = image.scaled(curReq.w, curReq.h, Qt::KeepAspectRatio, Globals::defTRansformationMode);
                }

                emit q->sendReply(DPImageReply(image.width(), image.height(), curReq.pageNo, 0, image));
            }

            mMutex.lock();
            if (requests.isEmpty())
                mCondition.wait(&mMutex);
            mMutex.unlock();
        }
    }

private:
    QMutex mMutex;
    QWaitCondition mCondition;
    bool abort;
    DPImageServicer *q;
    QVector<DPImageRequest> requests;
    QMap<int, QVariant> mData;

    friend class DPImageServicer;
};

DPImageServicer::DPImageServicer(QObject *parent)
    : QThread(parent)
    , d(new DPImageServicerPrivate(this))
{

}

DPImageServicer::~DPImageServicer()
{
    if (d) {
        delete d;
    }
}

void DPImageServicer::addRequest(DPImageRequest req)
{
    d->addRequest(req);
}


QVariant DPImageServicer::data(int role) const
{
    return d->mData.value(role, QVariant());
}

void DPImageServicer::setData(QVariant data, int role)
{
    d->mData.insert(role, data);
}

QImage DPImageServicer::imageForindex(int index) const
{
    Q_UNUSED(index);
    return QImage();
}

void DPImageServicer::run()
{
    d->run();
}

void DPImageServicer::replyOnRequest(DPImageRequest request)
{
    addRequest(request);
}

void DPImageServicer::replyCleanImageServicerQueue()
{
    QMutexLocker(&d->mMutex);
    d->requests.clear();
}

#include "ThumbnailManager.moc"
