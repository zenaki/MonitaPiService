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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDateTime>
#include <QFile>

struct piDataValue {
    QDateTime time;
    double value;
};

struct piData {
    int id;
    QString tagName;
    QString pointType;
    QString webID;

    struct piDataValue val;
};

class worker : public QObject
{
    Q_OBJECT
public:
    explicit worker(QObject *parent = 0);

    void request(QString urls);
    void parsing(QByteArray data);
    void readJSONFile(QString path);

    QNetworkAccessManager *manager;

    struct piData piServer;

    int id_sequence;
//    QSqlDatabase *db;

public slots:
    void replyFinished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
};

#endif // WORKER_H
