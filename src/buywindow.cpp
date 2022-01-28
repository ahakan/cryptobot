#include "buywindow.h"
#include "ui_buywindow.h"

#include <QTimer>

BuyWindow::BuyWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    ui(new Ui::BuyWindow)
{
    ui->setupUi(this);

    /**
     * @brief We moved window to center
     */
    move(screen()->geometry().center() - frameGeometry().center());
    setAttribute(Qt::WA_TranslucentBackground);

    /**
     * We set window title and window style sheet
     **/
    this->setWindowTitle("Crypto Bot - Buy Dashboard");
    this->setStyleSheet("background-color: rgb(0, 7, 32)");

    ui->APPNAME->setStyleSheet("color: rgb(218, 223, 225); font-size:28px; font-weight: 700; letter-spacing: 0.1em;");
    ui->CONFIGURATION_LABEL->setStyleSheet("color: rgb(218, 223, 225); font-size:24px; font-weight: 700; letter-spacing: 0.1em;");
    ui->INFORMATION_LABEL->setStyleSheet("color: rgb(218, 223, 225); font-size:24px; font-weight: 700; letter-spacing: 0.1em;");

    ui->TRADE_SYMBOL->setStyleSheet("QWidget:item:selected{background-color: rgb(85, 85, 127);}QWidget:item:checked{color: rgb(211, 211, 211);background-color: rgb(85, 85, 127);}");
    ui->TRADE_SYMBOL->setMinimumSize(QSize(0,40));

    ui->TRADE_SYMBOL_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");

    ui->TRADE_QUANTITY->setStyleSheet("border: 1px solid rgb(45, 69, 117)");
    ui->TRADE_QUANTITY->setMinimumSize(QSize(0,40));

    ui->TRADE_QUANTITY_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");

    ui->TRADE_AVERAGE->setStyleSheet("border: 1px solid rgb(45, 69, 117)");
    ui->TRADE_AVERAGE->setMinimumSize(QSize(0,40));

    ui->TRADE_AVERAGE_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");

    ui->CANDLESTICK_TYPE->setStyleSheet("QWidget:item:selected{background-color: rgb(85, 85, 127);}QWidget:item:checked{color: rgb(211, 211, 211);background-color: rgb(85, 85, 127);}");
    ui->CANDLESTICK_TYPE->setMinimumSize(QSize(0,40));
    ui->CANDLESTICK_TYPE->addItems({"1m", "3m", "5m", "15m", "30m", "1h", "2h", "4h", "6h", "8h", "12h", "1d", "3d", "1w", "1M"});

    ui->CANDLESTICK_DURATION->setStyleSheet("QWidget:item:selected{background-color: rgb(85, 85, 127);}QWidget:item:checked{color: rgb(211, 211, 211);background-color: rgb(85, 85, 127);}");
    ui->CANDLESTICK_DURATION->setMinimumSize(QSize(0,40));
    ui->CANDLESTICK_DURATION->addItems({"1h", "4h", "12h", "1d", "3d", "7d", "14d", "30d"});

    ui->CANDLESTICK_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");

    ui->START->setMinimumSize(QSize(0,40));

    ui->QUIT->setIcon(QIcon(":/dashboard/style/icon/cross.png"));
    ui->QUIT->setStyleSheet("border: 0px solid rgb(0, 7, 32)");
    ui->QUIT->setIconSize(QSize(24, 24));

    ui->REFRESH->setIcon(QIcon(":/dashboard/style/icon/refresh.png"));
    ui->REFRESH->setStyleSheet("border: 0px solid rgb(0, 7, 32)");
    ui->REFRESH->setIconSize(QSize(18, 18));
    ui->REFRESH->setVisible(false);


    /**
     * @brief information table ui configuration
     */

    ui->LAST_LIVE_PRICE->setStyleSheet("color: rgb(218, 223, 225); border: 1px solid rgb(45, 69, 117)");
    ui->LAST_LIVE_PRICE->setMinimumSize(QSize(0,40));
    ui->LAST_LIVE_PRICE->setEnabled(false);

    ui->LAST_LIVE_PRICE_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");

    ui->LAST_CANDLE_PRICE->setStyleSheet("color: rgb(218, 223, 225); border: 1px solid rgb(45, 69, 117)");
    ui->LAST_CANDLE_PRICE->setMinimumSize(QSize(0,40));
    ui->LAST_CANDLE_PRICE->setEnabled(false);

    ui->LAST_CANDLE_PRICE_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");

    ui->CANDLES_AVERAGE->setStyleSheet("color: rgb(218, 223, 225); border: 1px solid rgb(45, 69, 117)");
    ui->CANDLES_AVERAGE->setMinimumSize(QSize(0,40));
    ui->CANDLES_AVERAGE->setEnabled(false);

    ui->CANDLES_AVERAGE_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");

    ui->MAX_CANDLE_PRICE->setStyleSheet("color: rgb(218, 223, 225); border: 1px solid rgb(45, 69, 117)");
    ui->MAX_CANDLE_PRICE->setMinimumSize(QSize(0,40));
    ui->MAX_CANDLE_PRICE->setEnabled(false);

    ui->MAX_CANDLE_PRICE_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");

    ui->MIN_CANDLE_PRICE->setStyleSheet("color: rgb(218, 223, 225); border: 1px solid rgb(45, 69, 117)");
    ui->MIN_CANDLE_PRICE->setMinimumSize(QSize(0,40));
    ui->MIN_CANDLE_PRICE->setEnabled(false);

    ui->MIN_CANDLE_PRICE_LABEL->setStyleSheet("color: rgb(218, 223, 225);font-size:20px;");


    /**
     * @brief get all coin symbols
     */

    QTimer a;
    a.singleShot(100, this, &BuyWindow::getAllCoinSymbols);
}


void BuyWindow::onTextMessageReceived(QString message)
{
    mJsonResponse = QJsonDocument::fromJson(message.toUtf8());

    mJsonObject = mJsonResponse.object();

    mJsonValue = mJsonObject["k"];

    mJsonObject = mJsonValue.toObject();

    qDebug() << mJsonObject;

    ui->LAST_LIVE_PRICE->setText(mJsonObject["c"].toString());
}


void BuyWindow::allCoinsSymbolsReady()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QByteArray allJson;

    if(reply) allJson = allJson + reply->readAll();

//    qDebug() << allJson;

    mJsonResponse = QJsonDocument::fromJson(allJson);

    mJsonObject = mJsonResponse.object();

    mJsonValue = mJsonObject["symbols"];

    mJsonArray = mJsonValue.toArray();

    foreach( auto &a, mJsonArray)
    {
        mJsonObject = a.toObject();
//        qDebug() << mJsonObject["symbol"];

        ui->TRADE_SYMBOL->addItem(mJsonObject["symbol"].toString());
    }
}


void BuyWindow::coinInfoReady()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if(reply) mJsonResponse = QJsonDocument::fromJson(reply->readAll());

    mJsonObject = mJsonResponse.object();

    mJsonValue = mJsonObject["symbols"];

    mJsonArray = mJsonValue.toArray();

    foreach( auto &a, mJsonArray)
    {
        mJsonObject = a.toObject();
//        qDebug() << mJsonObject["filters"];
        mJsonArray = mJsonObject["filters"].toArray();
        foreach( auto &a, mJsonArray)
        {
            mJsonObject = a.toObject();
            if(mJsonObject["filterType"].toString() == "LOT_SIZE")
            {
                QString a = mJsonObject["minQty"].toString();
                double qty = a.toDouble();
                ui->TRADE_QUANTITY_LABEL->setText("Trade Quantity / Min: " + QString::number(qty, 'f', 4));
//                qDebug() << mJsonObject;
            }
        }
    }

}


void BuyWindow::coinCandlestickDataReady()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

//    if(reply) qDebug() << reply->readAll();
}


void BuyWindow::getAllCoinSymbols()
{
    QString url = mBinanceAPIUrl + "/api/v3/exchangeInfo";

    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, &QNetworkReply::finished, this, &BuyWindow::allCoinsSymbolsReady);
}


void BuyWindow::getCoinInfo(QString symbol)
{
    QString url = mBinanceAPIUrl + "/api/v3/exchangeInfo?symbol="+symbol;

    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, &QNetworkReply::finished, this, &BuyWindow::coinInfoReady);
}


void BuyWindow::getCoinCandlestickData()
{

    QString url = mBinanceAPIUrl + "/api/v3/klines?";

    QString queryString = mSymbolText + mTradeSymbol + "&";
    queryString = queryString + mIntervalText + mCandlesInterval + "&";
    queryString = queryString + mLimitText + "500" + "&";
    queryString = queryString + mStartTimeText + mCandlesTimestamp;

    url = url + queryString;

    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, &QNetworkReply::readyRead, this, &BuyWindow::coinCandlestickDataReady);

}


void BuyWindow::onConnected()
{
    connect(&mWebSocket, &QWebSocket::textMessageReceived,
            this, &BuyWindow::onTextMessageReceived);
}


void BuyWindow::on_TRADE_SYMBOL_currentTextChanged(const QString &arg1)
{
    mTradeSymbol = arg1;
    getCoinInfo(mTradeSymbol);
}


void BuyWindow::on_CANDLESTICK_TYPE_currentTextChanged(const QString &arg1)
{
    mCandlesInterval = arg1;
}


void BuyWindow::on_CANDLESTICK_DURATION_currentTextChanged(const QString &arg1)
{
    mCandlesDuration = arg1;

    QDateTime time = QDateTime::currentDateTime();
    int seconds = 0;

    if (arg1 == "1h")
    {
        seconds = 1*60*60;
        mCandlesDuration = "1 Hour";
    }
    else if (arg1 == "4h")
    {
        seconds = 4*60*60;
        mCandlesDuration = "4 Hours";
    }
    else if (arg1 == "12h")
    {
        seconds = 12*60*60;
        mCandlesDuration = "12 Hours";
    }
    else if (arg1 == "1d")
    {
        seconds = 1*24*60*60;
        mCandlesDuration = "1 Day";
    }
    else if (arg1 == "3d")
    {
        seconds = 3*24*60*60;
        mCandlesDuration = "3 Days";
    }
    else if (arg1 == "7d")
    {
        seconds = 7*24*60*60;
        mCandlesDuration = "7 Days";
    }
    else if (arg1 == "14d")
    {
        seconds = 14*24*60*60;
        mCandlesDuration = "14 Days";
    }
    else if (arg1 == "30d")
    {
        seconds = 30*24*60*60;
        mCandlesDuration = "30 Days";
    }
    time = time.addSecs(-seconds);

//    qDebug() << time;

    mCandlesTimestamp = QString::number(time.toMSecsSinceEpoch());

//    qDebug() << mCandlesTimestamp;
}


void BuyWindow::on_START_clicked()
{
    /**
     *  @brief initializing websocket
     **/
    mBinanceWebSocketUrl = mBinanceWebSocketUrl + mTradeSymbol.toLower();
    mBinanceWebSocketUrl = mBinanceWebSocketUrl + "@kline_" + mCandlesInterval;

    qDebug() << mBinanceWebSocketUrl;

    connect(&mWebSocket, &QWebSocket::connected, this, &BuyWindow::onConnected);
//    connect(&mWebSocket, &QWebSocket::disconnected, this, &BuyWindow::closed);

    mWebSocket.open(QUrl(mBinanceWebSocketUrl));

    /**
     * @brief get coin candlesticks data
     */
    getCoinCandlestickData();

    ui->DURATION_LABEL->setStyleSheet("color: rgb(218, 223, 225); font-size:12px;");
    ui->DURATION_LABEL->setText("(Last "+mCandlesDuration+" Data)");


}


void BuyWindow::on_QUIT_clicked()
{
    this->close();
}


void BuyWindow::on_REFRESH_clicked()
{

}


BuyWindow::~BuyWindow()
{
    delete ui;
}
