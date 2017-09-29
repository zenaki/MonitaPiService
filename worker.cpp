#include "worker.h"

worker::worker(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply *)));
    connect(manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
#ifndef QT_NO_SSL
    connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif

//    db = new QSqlDatabase();
//    db->addDatabase("QSQLITE");
//    db->setDatabaseName("C:/Users/Administrator/Desktop/MonitaPiService/Test");
//    if (!db->open()) {
//        return;
//    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Users/Administrator/Desktop/MonitaPiService/database.db");
    if(db.open()) {
        qDebug() << "Connected";
    } else {
        qDebug() << "Not Connected";
    }

    QSqlQuery query;
//    query.prepare("CREATE TABLE test (id INTEGER PRIMARY KEY);");
    if (query.exec("CREATE TABLE test (id INTEGER PRIMARY KEY)")) {
        qDebug() << "Berhasil Create Table Baru";
    }

//    QSqlQuery query("SELECT * from data_history");
//     if (query.lastError().isValid()) qDebug() << query.lastError().text();

    id_sequence = 0;

//    this->request("http://m2prime.aissat.com/RestMessages.svc/get_return_messages.json/?access_id=150103286&password=ZRM3B9SSDI&start_utc=2017-03-23%2000:00:00");
//    this->request("https://tmspremier/piwebapi/points?path=\\\\TMSPREMIER\\SINUSOID");
    this->readJSONFile("C:/Users/Administrator/Desktop/MonitaPiService/sample 1.json");
    id_sequence = 0;
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

    if (id_sequence == 0) {
        piServer.webID = object.value("WebId").toString();
        piServer.id = object.value("Id").toInt();
        piServer.tagName = object.value("Name").toString();
        piServer.pointType = object.value("PointType").toString();

        QJsonObject link = object.value("Links").toObject();
        if (!link.isEmpty()) {
            id_sequence = 1;
//            this->request(link.value("Value").toString());
            this->readJSONFile("C:/Users/Administrator/Desktop/MonitaPiService/sample 2.json");
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

void worker::readJSONFile(QString path) {
    QFile visual_json_file(path);
    if (visual_json_file.exists()) {
        if (visual_json_file.open(QIODevice::ReadWrite)) {
            QByteArray readFile = visual_json_file.readAll();
            this->parsing(readFile);
        }
    }
}
