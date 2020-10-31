#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "NetWorker.h"

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QJsonParseError>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

enum RemoteRequest {
    FetchWeatherInfo,
    FetchWeatherIcon
};

class MainWindow::Private
{
public:
    Private(MainWindow *q) :
        mainWindow(q)
    {
        netWorker = NetWorker::instance();
    }

    void fetchWeather(const QString &cityName)
    {
        QNetworkReply *reply = netWorker->get(QString("http://api.openweathermap.org/data/2.5/weather?q=%1&mode=json&units=metric&lang=zh_cn").arg(cityName));
        replyMap.insert(reply, FetchWeatherInfo);
    }

    void fetchIcon(const QString &iconName)
    {
        QNetworkReply *reply = netWorker->get(QString("http://api.openweathermap.org/img/w/%1.png").arg(iconName));
        replyMap.insert(reply, FetchWeatherIcon);
    }

    NetWorker *netWorker;
    MainWindow *mainWindow;
    QMap<QNetworkReply *, RemoteRequest> replyMap;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d(new MainWindow::Private(this))
{
    ui->setupUi(this);

    QComboBox *cityList = new QComboBox(this);
    cityList->setEditable(true);

    QFile f("city.txt");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        cityList->addItem(lineStr, QLatin1String((lineStr + ",cn").toLatin1().data()));
    }
    f.close();

    QLabel *cityLabel = new QLabel(tr("City: "), this);
    QPushButton *refreshButton = new QPushButton(tr("Refresh"), this);
    QHBoxLayout *cityListLayout = new QHBoxLayout;
    cityListLayout->setDirection(QBoxLayout::LeftToRight);
    cityListLayout->addWidget(cityLabel);
    cityListLayout->addWidget(cityList);
    cityListLayout->addWidget(refreshButton);

    QVBoxLayout *weatherLayout = new QVBoxLayout;
    weatherLayout->setDirection(QBoxLayout::TopToBottom);
    QLabel *cityNameLabel = new QLabel(this);
    weatherLayout->addWidget(cityNameLabel);
    QLabel *dateTimeLabel = new QLabel(this);
    weatherLayout->addWidget(dateTimeLabel);

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addLayout(cityListLayout);
    mainLayout->addLayout(weatherLayout);
    setCentralWidget(mainWidget);
    resize(320, 120);
    setWindowTitle(tr("Weather"));

    QLabel *labelDesc = new QLabel(this);
    QLabel *labelIcon = new QLabel(this);
//    weatherDetailLayout->addWidget(new QLabel(detail->desc(), this));
//    weatherDetailLayout->addWidget(new QLabel(this));
    QHBoxLayout *weatherDetailLayout = new QHBoxLayout;
    weatherDetailLayout->setDirection(QBoxLayout::LeftToRight);
    weatherDetailLayout->addWidget(labelDesc);
    weatherDetailLayout->addWidget(labelIcon);
    weatherLayout->addLayout(weatherDetailLayout);

    connect(d->netWorker, &NetWorker::finished, [=] (QNetworkReply *reply)
    {
        RemoteRequest request = d->replyMap.value(reply);
        switch (request) {
        case FetchWeatherInfo:
        {
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error == QJsonParseError::NoError) {
                if (!(jsonDocument.isNull() || jsonDocument.isEmpty()) && jsonDocument.isObject()) {
                    QVariantMap data = jsonDocument.toVariant().toMap();
                    WeatherInfo weather;
                    weather.setCityName(data[QLatin1String("name")].toString());
                    cityNameLabel->setText(weather.cityName());
                    QDateTime dateTime;
                    dateTime.setTime_t(data[QLatin1String("dt")].toLongLong());
                    weather.setDateTime(dateTime);
                    QVariantMap main = data[QLatin1String("main")].toMap();
                    weather.setTemperature(main[QLatin1String("temp")].toFloat());
                    weather.setPressure(main[QLatin1String("pressure")].toFloat());
                    weather.setHumidity(main[QLatin1String("humidity")].toFloat());
                    QVariantList detailList = data[QLatin1String("weather")].toList();
                    QList<WeatherDetail *> details;
                    foreach (QVariant w, detailList) {
                        QVariantMap wm = w.toMap();
                        WeatherDetail *detail = new WeatherDetail;
                        detail->setDesc(wm[QLatin1String("description")].toString());
                        detail->setIcon(wm[QLatin1String("icon")].toString());
                        details.append(detail);

                        labelDesc->setText(detail->desc());

                        d->fetchIcon(detail->icon());
                    }
                    weather.setDetails(details);

                    cityNameLabel->setText(weather.cityName());
                    dateTimeLabel->setText(weather.dateTime().toString(Qt::DefaultLocaleLongDate));
                }
            } else {
                QMessageBox::critical(this, tr("Error"), error.errorString());
            }
            break;
        }
        case FetchWeatherIcon:
        {
//            QHBoxLayout *weatherDetailLayout = (QHBoxLayout *)weatherLayout->itemAt(2)->layout();
//            QLabel *iconLabel = (QLabel *)weatherDetailLayout->itemAt(1)->widget();
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
//            iconLabel->setPixmap(pixmap);
            labelIcon->setPixmap(pixmap);
            break;
        }
        }

        reply->deleteLater();
    });
    connect(refreshButton, &QPushButton::clicked, [=] () {
        d->fetchWeather(cityList->currentText().split(',').at(0));
    });
}

MainWindow::~MainWindow()
{
    delete ui;

    delete d;
    d = 0;
}

QDebug operator <<(QDebug dbg, const WeatherDetail &w)
{
    dbg.nospace() << "("
                   << "Description: " << w.desc() << "; "
                   << "Icon: " << w.icon()
                   << ")";
     return dbg.space();
}


QDebug operator <<(QDebug dbg, const WeatherInfo &w)
{
    dbg.nospace() << "("
                   << "id: " << w.id() << "; "
                   << "City name: " << w.cityName() << "; "
                   << "Date time: " << w.dateTime().toString(Qt::DefaultLocaleLongDate) << ": " << endl
                   << "Temperature: " << w.temperature() << ", "
                   << "Pressure: " << w.pressure() << ", "
                   << "Humidity: " << w.humidity() << endl
                   << "Details: [";
     foreach (WeatherDetail *detail, w.details()) {
         dbg.nospace() << "( Description: " << detail->desc() << ", "
                       << "Icon: " << detail->icon() << "), ";
     }
     dbg.nospace() << "] )";
     return dbg.space();
}

class WeatherDetail::Private
{
public:
    QString m_desc;
    QString m_icon;
};

WeatherDetail::WeatherDetail() :
    d(new WeatherDetail::Private)
{

}

WeatherDetail::~WeatherDetail()
{
    delete d;
    d = NULL;
}

QString WeatherDetail::desc() const
{
    return d->m_desc;
}

void WeatherDetail::setDesc(const QString &desc)
{
    d->m_desc = desc;
}

QString WeatherDetail::icon() const
{
    return d->m_icon;
}

void WeatherDetail::setIcon(const QString &icon)
{
    d->m_icon = icon;
}

class WeatherInfo::Private
{
public:
    QString m_cityName;
    quint32 m_id;
    QDateTime m_dateTime;
    float m_temperature;
    float m_humidity;
    float m_pressure;
    QList<WeatherDetail*> m_details;
};

WeatherInfo::WeatherInfo():
    d(new WeatherInfo::Private)
{

}

WeatherInfo::~WeatherInfo()
{
    delete d;
    d = NULL;
}

QString WeatherInfo::cityName() const
{
    return d->m_cityName;
}

void WeatherInfo::setCityName(const QString &cityName)
{
    d->m_cityName = cityName;
}

quint32 WeatherInfo::id() const
{
    return d->m_id;
}

void WeatherInfo::setId(quint32 id)
{
    d->m_id = id;
}

QDateTime WeatherInfo::dateTime() const
{
    return d->m_dateTime;
}

void WeatherInfo::setDateTime(const QDateTime &dateTime)
{
    d->m_dateTime = dateTime;
}

float WeatherInfo::temperature() const
{
    return d->m_temperature;
}

void WeatherInfo::setTemperature(float temperature)
{
    d->m_temperature = temperature;
}

float WeatherInfo::humidity() const
{
    return d->m_humidity;
}

void WeatherInfo::setHumidity(float humidity)
{
    d->m_humidity = humidity;
}

float WeatherInfo::pressure() const
{
    return d->m_pressure;
}

void WeatherInfo::setPressure(float pressure)
{
    d->m_pressure = pressure;
}

QList<WeatherDetail *> WeatherInfo::details() const
{
    return d->m_details;
}

void WeatherInfo::setDetails(const QList<WeatherDetail *> details)
{
    d->m_details = details;
}
