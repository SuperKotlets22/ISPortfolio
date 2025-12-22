#include "Security.h"
#include <QCryptographicHash>
#include <QByteArray>
#include <QMessageAuthenticationCode>

const QByteArray KEY_HEX = QByteArray::fromHex("9F735E4B2C8D1A6F0E3B5D8C9A2F4E7D1B6C8A0E3F5D7B9A2C4E6F8D1B3A5C7E");
const QByteArray IV_HEX  = QByteArray::fromHex("5A3C1B8D9E2F4A7C6B0D8E1F3A5C7B9D");

QString SecurityManager::encrypt(const QString &plainText) {
    if (plainText.isEmpty()) return "";

    QByteArray data = plainText.toUtf8();
    
    QByteArray output;
    QByteArray key = KEY_HEX;
    QByteArray iv = IV_HEX;
    
    QByteArray block = iv;
    for(int i = 0; i < data.size(); ++i) {
        char byte = data[i] ^ key[i % key.size()] ^ block[i % block.size()];
        output.append(byte);
        block[i % block.size()] = byte; 
    }

    return QString(output.toBase64());
}

QString SecurityManager::decrypt(const QString &cipherText) {
    if (cipherText.isEmpty()) return "";

    QByteArray data = QByteArray::fromBase64(cipherText.toUtf8());
    QByteArray output;
    QByteArray key = KEY_HEX;
    QByteArray iv = IV_HEX;

    QByteArray block = iv;
    for(int i = 0; i < data.size(); ++i) {
        char encryptedByte = data[i];
        char byte = encryptedByte ^ key[i % key.size()] ^ block[i % block.size()];
        output.append(byte);
        block[i % block.size()] = encryptedByte;
    }

    return QString::fromUtf8(output);
}

QString SecurityManager::hashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}