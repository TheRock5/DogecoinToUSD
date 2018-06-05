/* 
 * MIT License
 *
 * Copyright (c) 2018 TheRock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
