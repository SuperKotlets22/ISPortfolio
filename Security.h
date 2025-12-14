#ifndef SECURITY_H
#define SECURITY_H

#include <QString>
#include <QByteArray>

class SecurityManager {
public:
    static QString encrypt(const QString &plainText);
    static QString decrypt(const QString &cipherText);
    static QString hashPassword(const QString &password);

private:
    static const QString KEY; // Секретный ключ (зашит в код)
};

#endif // SECURITY_H
