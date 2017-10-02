#include "worker.h"

worker::worker(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply *)));
    connect(manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
#ifndef QT_NO_SSL
    connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif

    path = this->readJSONFile("C:/Users/Administrator/Desktop/MonitaPiService/tag_list.json");
    id_sequence = 0;
    for (int i = 0; i < path.length(); i++) {
//            this->readJSONFile("C:/Users/Administrator/Desktop/MonitaPiService/sample 1.json");
            this->request("https://tmspremier/piwebapi/points?path="+path.at(i));
            id_sequence = 0;
            writeToDB();
    }
}

void worker::request(QString urls)
{
    QNetworkRequest request;
    QUrl url =  QUrl::fromEncoded(urls.toLocal8Bit().data());

    qDebug() << "Request URL";
    request.setUrl(url);
    manager->get(request);
}

void worker::parsing(QByteArray data)
{
    QJsonDocument JsonDoc = QJsonDocument::fromJson(data);
    QJsonObject object = JsonDoc.object();

    if (id_sequence == 0) {
        piServer.webID = object.value("WebId").toString();
        piServer.id = object.value("Id").toInt();
        piServer.tagName = object.value("Name").toString();
        piServer.pointType = object.value("PointType").toString();

        QJsonObject link = object.value("Links").toObject();
        if (!link.isEmpty()) {
            id_sequence = 1;
//            this->readJSONFile("C:/Users/Administrator/Desktop/MonitaPiService/sample 2.json");
            this->request(link.value("Value").toString());
        }
    }

    if (id_sequence == 1) {
        piServer.val.value = object.value("Value").toDouble();
        piServer.val.time = QDateTime::fromString(object.value("Timestamp").toString(), "yyyy-MM-ddTHH:mm:ssZ");
        id_sequence = 2;
    }

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

QStringList worker::readJSONFile(QString path) {
    QStringList result;
    QFile json_file(path);
    if (json_file.exists()) {
        if (json_file.open(QIODevice::ReadWrite)) {
            QByteArray readFile = json_file.readAll();
            QJsonDocument JsonDoc = QJsonDocument::fromJson(readFile);
            QJsonObject object = JsonDoc.object();

            QJsonArray array = object.value("path").toArray();
            foreach (const QJsonValue & v, array) {
                result.append(v.toObject().value("location").toString());
            }
        }
    }
    return result;
}

void worker::writeToDB() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Users/Administrator/Desktop/MonitaPiService/database.db");
    if(db.open()) {
        qDebug() << "DB Connected";
    } else {
        qDebug() << "DB Not Connected";
    }
    QSqlQuery query;
    QString str = "SELECT count(*) FROM list_titik_ukur WHERE "
            " tag = '" + piServer.tagName + "' and "
            " webid = '" + piServer.webID + "'";
    if (query.exec(str)) {
        while (query.next()) {
            if (query.value(0).toInt() == 0) {
                str = "INSERT INTO list_titik_ukur (id_tu, tag, webid) values (" +
                        QString::number(piServer.id) + ", '" +
                        piServer.tagName + "', '" +
                        piServer.webID + "')";
                if (query.exec(str)) {
                    qDebug() << "Berhasil Insert Data Baru di table list_titik_ukur";
                } else {
                    qDebug() << "Gagal Insert Data Baru di table list_titik_ukur";
                    qDebug() << query.lastError().text();
                }
            }
        }
    }

    str = "SELECT count(*) FROM data_history WHERE "
            " id_tu = " + QString::number(piServer.id) + " and "
            " timestamp = '" + QString::number(piServer.val.time.toUTC().toMSecsSinceEpoch()) + "'";
    if (query.exec(str)) {
        while (query.next()) {
            if (query.value(0).toInt() == 0) {
                str = "INSERT INTO data_history (id_tu, timestamp, value) values (" +
                        QString::number(piServer.id) + ", '" +
                        QString::number(piServer.val.time.toUTC().toMSecsSinceEpoch()) + "', " +
                        QString::number(piServer.val.value) + ")";
                if (query.exec(str)) {
                    qDebug() << "Berhasil Insert Data Baru di table data_history";
                } else {
                    qDebug() << "Gagal Insert Data Baru di table data_history";
                    qDebug() << query.lastError().text();
                }
            }
        }
    }

    db.close();
}

void worker::readFromDB() {
    QSqlQuery query;
    QString str = "INSERT INTO list_titik_ukur (tag, webid) value (" +
            piServer.tagName + ", " +
            piServer.webID + ")";
    if (query.exec(str)) {
        qDebug() << "Berhasil Insert Data Baru";
    }
}
