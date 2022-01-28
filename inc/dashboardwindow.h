#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QMainWindow>
#include <QScreen>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>

#include <QMovie>

#include "utilities.h"
#include "buywindow.h"

namespace Ui {
class DashboardWindow;
}

class DashboardWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::DashboardWindow         *ui;

    BuyWindow                   *pBuyWindow;

    QNetworkAccessManager       manager;
    QNetworkRequest             request;

    QJsonDocument               mJsonResponse;
    QJsonObject                 mJsonObject;
    QJsonValue                  mJsonValue;

    QString                     mBinanceAPIUrl = "https://api.binance.com";
    QString                     mTimestamp = "timestamp=";

    void                        checkAccountAPIStatus();

private slots:
    void                        on_QUIT_clicked();
    void                        on_REFRESH_clicked();

    void                        on_BUY_clicked();
    void                        on_BUYSELL_clicked();
    void                        on_SELL_clicked();
    void                        on_SELLBUY_clicked();


    void                        accountAPIStatusReady();

public:
    explicit                    DashboardWindow(QWidget *parent = nullptr);
                                ~DashboardWindow();
};

#endif // DASHBOARDWINDOW_H
