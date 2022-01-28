#ifndef BUYWINDOW_H
#define BUYWINDOW_H

#include <QMainWindow>
#include <QScreen>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QtWebSockets/QWebSocket>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QMovie>

#include "utilities.h"

namespace Ui {
class BuyWindow;
}

class BuyWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::BuyWindow *ui;

    QWebSocket                  mWebSocket;
    QNetworkAccessManager       manager;
    QNetworkRequest             request;

    QJsonDocument               mJsonResponse;
    QJsonObject                 mJsonObject;
    QJsonValue                  mJsonValue;
    QJsonArray                  mJsonArray;

    QString                     mBinanceWebSocketUrl = "wss://stream.binance.com:9443/ws/";
    QString                     mBinanceAPIUrl = "https://api.binance.com";
    QString                     mLimitText = "limit=";
    QString                     mSymbolText = "symbol=";
    QString                     mIntervalText = "interval=";
    QString                     mTimestampText = "timestamp=";
    QString                     mStartTimeText = "startTime=";

    QString                     mTradeSymbol;

    QString                     mCandlesInterval;
    QString                     mCandlesTimestamp;
    QString                     mCandlesDuration;

    void                        getAllCoinSymbols();
    void                        getCoinInfo(QString symbol);
    void                        getCoinCandlestickData();

private slots:
    void                        onConnected();
    void                        onTextMessageReceived(QString message);

    void                        allCoinsSymbolsReady();
    void                        coinInfoReady();
    void                        coinCandlestickDataReady();

    void                        on_TRADE_SYMBOL_currentTextChanged(const QString &arg1);
    void                        on_CANDLESTICK_TYPE_currentTextChanged(const QString &arg1);
    void                        on_CANDLESTICK_DURATION_currentTextChanged(const QString &arg1);

    void                        on_START_clicked();

    void                        on_QUIT_clicked();
    void                        on_REFRESH_clicked();

signals:
    void                        closed();

public:
    explicit                    BuyWindow(QWidget *parent = nullptr);
                                ~BuyWindow();

};

#endif // BUYWINDOW_H
