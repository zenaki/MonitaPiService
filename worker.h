#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QAuthenticator>

class worker : public QObject
{
    Q_OBJECT
public:
    explicit worker(QObject *parent = 0);

    void request(QString urls);
    void parsing(QByteArray data);

    QNetworkAccessManager *manager;

public slots:
    void replyFinished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
};

#endif // WORKER_H
