#include "Security.h"
#include <QCryptographicHash>
#include <QByteArray>

const QString SecurityManager::KEY = "SecretKeyForPortfolio_2025_Secure";

QString SecurityManager::encrypt(const QString &plainText) {
    QByteArray data = plainText.toUtf8();
    QByteArray keyData = KEY.toUtf8();
    QByteArray output;

    for (int i = 0; i < data.size(); ++i) {
        output.append(data[i] ^ keyData[i % keyData.size()]);
    }
    return QString(output.toBase64());
}

QString SecurityManager::decrypt(const QString &cipherText) {
    QByteArray data = QByteArray::fromBase64(cipherText.toUtf8());
    QByteArray keyData = KEY.toUtf8();
    QByteArray output;

    for (int i = 0; i < data.size(); ++i) {
        output.append(data[i] ^ keyData[i % keyData.size()]);
    }
    return QString::fromUtf8(output);
}

QString SecurityManager::hashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}
