#include "NetWorker.h"

class NetWorker::Private
{
public:
    Private(NetWorker *q) :
        manager(new QNetworkAccessManager(q)){}
    QNetworkAccessManager *manager;
};

NetWorker::NetWorker(QObject *parent) :
    QObject(parent),
    d(new NetWorker::Private(this))
{
    connect(d->manager, &QNetworkAccessManager::finished, this, &NetWorker::finished);
}

NetWorker *NetWorker::instance()
{
    static NetWorker netWorker;
    return &netWorker;
}

NetWorker::~NetWorker()
{
    delete d;
    d = 0;
}

//QNetworkReply *NetWorker::get(const QString &url)
//{
//    return d->manager->get(QNetworkRequest(QUrl(url)));
//}
QNetworkReply * NetWorker::get(const QString &url)
{
    return d->manager->get(QNetworkRequest(QUrl(url)));
}
