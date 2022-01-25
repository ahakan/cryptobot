#include "dashboardwindow.h"
#include "ui_dashboardwindow.h"

DashboardWindow::DashboardWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    ui(new Ui::DashboardWindow)
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
    this->setWindowTitle("Crypto Bot - Dashboard");
    this->setStyleSheet("background-color: rgb(0, 7, 32)");

    ui->BUY->setVisible(false);
    ui->SELL->setVisible(false);
    ui->BUYSELL->setVisible(false);
    ui->SELLBUY->setVisible(false);

    ui->BUY->setIcon(QIcon(":/dashboard/style/icon/bitcoin.png"));
    ui->BUY->setIconSize(QSize(28, 28));
    ui->BUY->setStyleSheet("border: 0px solid rgb(0, 7, 32)");

    ui->SELL->setIcon(QIcon(":/dashboard/style/icon/coin.png"));
    ui->SELL->setIconSize(QSize(28, 28));
    ui->SELL->setStyleSheet("border: 0px solid rgb(0, 7, 32)");

    ui->BUYSELL->setIcon(QIcon(":/dashboard/style/icon/bitcoin.png"));
    ui->BUYSELL->setIconSize(QSize(28, 28));
    ui->BUYSELL->setStyleSheet("border: 0px solid rgb(0, 7, 32)");

    ui->SELLBUY->setIcon(QIcon(":/dashboard/style/icon/coin.png"));
    ui->SELLBUY->setIconSize(QSize(28, 28));
    ui->SELLBUY->setStyleSheet("border: 0px solid rgb(0, 7, 32)");

    ui->QUIT->setIcon(QIcon(":/dashboard/style/icon/cross.png"));
    ui->QUIT->setStyleSheet("border: 0px solid rgb(0, 7, 32)");
    ui->QUIT->setIconSize(QSize(24, 24));

    checkAccountAPIStatus();
}

DashboardWindow::~DashboardWindow()
{
    delete ui;
}

void DashboardWindow::accountAPIStatusReady()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if(reply) mJsonResponse = QJsonDocument::fromJson(reply->readAll());

    mJsonObject = mJsonResponse.object();

    mJsonValue = mJsonObject["data"];

    qDebug() << mJsonValue["isLocked"].toBool();

    if (mJsonValue["isLocked"].toBool() == true)
    {
        /**
         * If User account islocked
         * We close the menu
         * We show the notification
         **/
        ui->BUY->setVisible(false);
        ui->SELL->setVisible(false);
        ui->BUYSELL->setVisible(false);
        ui->SELLBUY->setVisible(false);

    }
    else
    {
        ui->NOTIFICATION->setVisible(false);

        ui->BUY->setVisible(true);
        ui->SELL->setVisible(true);
        ui->BUYSELL->setVisible(true);
        ui->SELLBUY->setVisible(true);
    }


}


void DashboardWindow::checkAccountAPIStatus()
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());

    QString url = mBinanceAPIUrl + "/sapi/v1/account/apiTradingStatus?";

    QString queryString = mTimestamp + timestamp;

    QString signature = Util.getSignature(queryString);

    queryString = queryString + signature;

    url = url + queryString;

    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-MBX-APIKEY", Util.getSettingsValue("API_KEY").toLocal8Bit());

    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, &QNetworkReply::readyRead, this, &DashboardWindow::accountAPIStatusReady);

    /**
     * @brief loading animation
     */
    QMovie *movie = new QMovie(":/login/style/gif/loading.gif");
    movie->setScaledSize(QSize(50,50));
    ui->NOTIFICATION->setMinimumHeight(298);
    ui->NOTIFICATION->setMaximumHeight(298);
    ui->NOTIFICATION->setMovie(movie);
    movie->start();
}

void DashboardWindow::on_QUIT_clicked()
{
    this->close();
}


void DashboardWindow::on_BUY_clicked()
{

}


void DashboardWindow::on_BUYSELL_clicked()
{

}


void DashboardWindow::on_SELL_clicked()
{

}


void DashboardWindow::on_SELLBUY_clicked()
{

}

