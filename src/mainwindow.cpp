#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QString appDir, QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint)
    , ui(new Ui::MainWindow)
    , appDirectory{appDir}
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
    this->setWindowTitle("Crypto Bot - Login");
    this->setStyleSheet("background-color: rgb(0, 7, 32)");

    ui->API_KEY->setStyleSheet("border: 1px solid rgb(107, 169, 255)");
    ui->SECRET_KEY->setStyleSheet("border: 1px solid rgb(107, 169, 255)");

    ui->QUIT->setIcon(QIcon(":/dashboard/style/icon/cross.png"));
    ui->QUIT->setStyleSheet("border: 0px solid rgb(0, 7, 32)");
    ui->QUIT->setIconSize(QSize(24, 24));

    ui->REFRESH->setIcon(QIcon(":/dashboard/style/icon/refresh.png"));
    ui->REFRESH->setStyleSheet("border: 0px solid rgb(0, 7, 32)");
    ui->REFRESH->setIconSize(QSize(18, 18));
    ui->REFRESH->setVisible(false);

    /**
     * Set appDirectory path to utilities variable.
     **/
    Util.setApplicationDirectory(appDirectory);

    /**
     * If we have a API_KEY and a SECRET_KEY
     * We check are these currect
     **/
    if (Util.getSettingsValue("API_KEY").size() != 0  && Util.getSettingsValue("SECRET_KEY").size() != 0)
    {
        ui->API_KEY->setVisible(false);
        ui->SECRET_KEY->setVisible(false);
        ui->LOGIN->setVisible(false);

        ui->API_KEY_LABEL->setVisible(false);
        ui->SECRET_KEY_LABEL->setVisible(false);

//        checkAccountStatus();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::accountStatusReady()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if(reply) mJsonResponse = QJsonDocument::fromJson(reply->readAll());

    qDebug() << mJsonResponse["data"].toString();

    if (mJsonResponse["data"].toString() == "Normal")
    {
        /**
         * If API_KEY and SECRET_KEY are currect
         * We close the main window
         * We open dashboard window
         **/
        pDashboardWindow = new DashboardWindow();
        pDashboardWindow->show();

        this->close();
    }
    else
    {
        ui->NOTIFICATION->setVisible(false);

        ui->API_KEY->setStyleSheet("border: 1px solid rgb(170, 0, 0)");
        ui->SECRET_KEY->setStyleSheet("border: 1px solid rgb(170, 0, 0)");

        ui->API_KEY->setVisible(true);
        ui->SECRET_KEY->setVisible(true);
        ui->LOGIN->setVisible(true);

        ui->API_KEY_LABEL->setVisible(true);
        ui->SECRET_KEY_LABEL->setVisible(true);

        ui->REFRESH->setVisible(true);
    }


}


void MainWindow::checkAccountStatus()
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());

    QString url = mBinanceAPIUrl + "/sapi/v1/account/status?";

    QString queryString = mRecvWindow + mRecvWindowValue + "&" + mTimestamp + timestamp;

    QString signature = Util.getSignature(queryString);

    queryString = queryString + signature;

    url = url + queryString;

    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-MBX-APIKEY", Util.getSettingsValue("API_KEY").toLocal8Bit());

    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, &QNetworkReply::readyRead, this, &MainWindow::accountStatusReady);

    /**
     * @brief loading animation
     */
    QMovie *movie = new QMovie(":/login/style/gif/loading.gif");
    movie->setScaledSize(QSize(50,50));
    ui->NOTIFICATION->setMinimumHeight(212);
    ui->NOTIFICATION->setMaximumHeight(212);
    ui->NOTIFICATION->setMovie(movie);
    movie->start();
}


void MainWindow::on_LOGIN_clicked()
{
    ui->API_KEY->setVisible(false);
    ui->SECRET_KEY->setVisible(false);
    ui->API_KEY_LABEL->setVisible(false);
    ui->SECRET_KEY_LABEL->setVisible(false);
    ui->LOGIN->setVisible(false);

    Util.setSettingsValue("API_KEY", ui->API_KEY->text());
    Util.setSettingsValue("SECRET_KEY", ui->SECRET_KEY->text());

    checkAccountStatus();
}




void MainWindow::on_QUIT_clicked()
{
    this->close();
}


void MainWindow::on_REFRESH_clicked()
{
    checkAccountStatus();
}

