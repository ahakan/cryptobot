#ifndef UTILITIES_H
#define UTILITIES_H

#include <QtCore/QCoreApplication>
#include <QSettings>

#include <openssl/hmac.h>

class Utilities
{
private:
    QSettings                   *settings;

    QString                     appDirectory;

public:
                                Utilities();

    void                        setApplicationDirectory(QString dir);
    void                        setSettingsValue(QString key, QString value);
    QString                     getSettingsValue(QString key);

    QString                     encryptWithHMAC(const char* key, const char* data);
    QString                     getSignature(QString query);
};

extern Utilities Util;

#endif // UTILITIES_H


