#include "Security.h"
#include <QCryptographicHash>
#include <QByteArray>

// Ключ шифрования (32 символа для эмуляции 256 бит)
const QString SecurityManager::KEY = "SecretKeyForPortfolio_2025_Secure";

// Реализация простого XOR-шифрования с ключом + Base64
// Для курсовой это демонстрирует принцип "хранения в зашифрованном виде"
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

// Хэширование пароля (SHA-256)
QString SecurityManager::hashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}
