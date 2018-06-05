#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <qDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QNetworkAccessManager *web = new QNetworkAccessManager(this);
    web->get(QNetworkRequest(QUrl("https://bittrex.com/api/v1.1/public/getmarketsummaries")));
    connect(web, &QNetworkAccessManager::finished, [](QNetworkReply *reply) {

       if(reply->error()) {
           qDebug() << reply->errorString();
       }
       else {

           QPair<double, double> currencyPair;
           const QString data = reply->readAll();
           const QJsonDocument document = QJsonDocument::fromJson(data.toUtf8());
           const QJsonObject jsonObject = document.object();

           if(jsonObject.value("success").toBool()) {
               const QJsonArray jsonArray = jsonObject["result"].toArray();

               foreach (const QJsonValue & value, jsonArray) {
                   const QString currency = value.toObject().value("MarketName").toString();

                   if(!QString::compare(currency, "BTC-DOGE"))
                       currencyPair.first = value.toObject().value("Last").toDouble();
                   else if(!QString::compare(currency, "USD-BTC"))
                       currencyPair.second = value.toObject().value("Last").toDouble();
               }
               qDebug() << "Dogecoin to USD:" << currencyPair.second * currencyPair.first;
               /* Dogecoin USD: 0.0035062 */
           }
           else
               qDebug() << "Failed to convert DOGE to USD.";
       }

    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
