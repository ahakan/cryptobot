#include "utilities.h"

Utilities::Utilities()
{
}

void Utilities::setApplicationDirectory(QString dir)
{
    appDirectory = dir;
}

void Utilities::setSettingsValue(QString key, QString value)
{
    settings = new QSettings(appDirectory + "/crypto.plist", QSettings::NativeFormat);

    settings->setValue(key, value);
}

QString Utilities::getSettingsValue(QString key)
{
    settings = new QSettings(appDirectory + "/crypto.plist", QSettings::NativeFormat);

    return settings->value(key).toString();
}

QString Utilities::encryptWithHMAC(const char* key, const char* data)
{
    unsigned char *result;
    static char res_hexstring[64];
    int result_len = 32;
    QString signature;

    result = HMAC(EVP_sha256(), key, strlen((char *)key), const_cast<unsigned char *>(reinterpret_cast<const unsigned char*>(data)), strlen((char *)data), NULL, NULL);

    for (int i = 0; i < result_len; i++) {
        sprintf(&(res_hexstring[i * 2]), "%02x", result[i]);
    }

    for (int i = 0; i < 64; i++) {
        signature += res_hexstring[i];
    }

    return signature;
}

QString Utilities::getSignature(QString query)
{
    QString signature = encryptWithHMAC(getSettingsValue("SECRET_KEY").toStdString().c_str(), query.toStdString().c_str());
    return "&signature=" + signature;
}


Utilities Util = Utilities();

