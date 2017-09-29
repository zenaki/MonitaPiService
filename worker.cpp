#include "worker.h"

worker::worker(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply *)));
    connect(manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
#ifndef QT_NO_SSL
    connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif
//    this->request("http://m2prime.aissat.com/RestMessages.svc/get_return_messages.json/?access_id=150103286&password=ZRM3B9SSDI&start_utc=2017-03-23%2000:00:00");
    this->request("https://tmspremier/piwebapi/");
}

void worker::request(QString urls)
{
    QNetworkRequest request;
    //    urls = "http://m2prime.aissat.com/RestMessages.svc/get_return_messages.json/?access_id=150103286&password=ZRM3B9SSDI&start_utc=2017-03-23%2000:00:00";
    //    urls = "http://m2prime.aissat.com/RestMessages.svc/get_return_messages.json/?access_id=150103286&password=ZRM3B9SSDI&start_utc=2017-03-27 03:43:02&end_utc=2017-03-27 04:43:02&mobile_id=01020268SKY7559";
    QUrl url =  QUrl::fromEncoded(urls.toLocal8Bit().data());

    qDebug() << "Request URL";
    request.setUrl(url);
    manager->get(request);
}

void worker::parsing(QByteArray data)
{
    QJsonDocument JsonDoc = QJsonDocument::fromJson(data);
    QJsonObject object = JsonDoc.object();

    qDebug() << "Finish";
}

void worker::replyFinished(QNetworkReply *reply)
{
    qDebug() << "Get Reply";
    QByteArray data;
    data = reply->readAll();
    qDebug() << data;
    this->parsing(data);
}

void worker::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    qDebug() << "Authentication";
    authenticator->setUser("administrator");
    authenticator->setPassword("P@$$w0rd");
}

#ifndef QT_NO_SSL
void worker::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    qDebug() << "SSL Error";
    reply->ignoreSslErrors();
}
#endif
