#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScreen>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>

#include <QMovie>

#include "dashboardwindow.h"

#include "utilities.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow              *ui;

    DashboardWindow             *pDashboardWindow;

    QNetworkAccessManager       manager;
    QNetworkRequest             request;

    QJsonDocument               mJsonResponse;
    QJsonObject                 mJsonObject;

    QString                     mBinanceAPIUrl = "https://api.binance.com";
    QString                     mTimestamp = "timestamp=";
    QString                     mRecvWindow = "recvWindow=";
    QString                     mRecvWindowValue = "5000";

    QString                     appDirectory;

private:
    void                        checkAccountStatus();

private slots:
    void                        on_LOGIN_clicked();
    void                        on_QUIT_clicked();
    void                        on_REFRESH_clicked();

    void                        accountStatusReady();

public:
                                MainWindow(QString appDir, QWidget *parent = nullptr);
                                ~MainWindow();

};
#endif // MAINWINDOW_H
