#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QStackedWidget>
#include <QRegularExpression> // Для проверки телефона
#include "Database.h"
#include "MainWindow.h"

class LoginWindow : public QWidget {
    Q_OBJECT
    QLineEdit *loginEdit, *passEdit;
    QLineEdit *regLogin, *regPass, *regEmail, *regPhone;
    QStackedWidget *stack;

public:
    LoginWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Вход в систему Portfolio");
        resize(400, 450);
        
        // --- ДИЗАЙН (Логотип или заголовок) ---
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);
        stack = new QStackedWidget;
        layout->addWidget(stack);

        // === СТРАНИЦА ВХОДА ===
        QWidget *loginPage = new QWidget;
        QVBoxLayout *loginLayout = new QVBoxLayout(loginPage);
        loginLayout->setContentsMargins(40,40,40,40);
        loginLayout->setSpacing(15);
        
        QLabel *title = new QLabel("ВХОД");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 24px; font-weight: bold; color: #61dafb;");
        
        loginEdit = new QLineEdit; loginEdit->setPlaceholderText("Логин");
        passEdit = new QLineEdit; passEdit->setPlaceholderText("Пароль"); passEdit->setEchoMode(QLineEdit::Password);
        
        QPushButton *btnLogin = new QPushButton("Войти");
        btnLogin->setCursor(Qt::PointingHandCursor);
        QPushButton *btnGoReg = new QPushButton("Нет аккаунта? Регистрация");
        btnGoReg->setFlat(true);
        btnGoReg->setCursor(Qt::PointingHandCursor);

        loginLayout->addStretch();
        loginLayout->addWidget(title);
        loginLayout->addWidget(loginEdit);
        loginLayout->addWidget(passEdit);
        loginLayout->addWidget(btnLogin);
        loginLayout->addWidget(btnGoReg);
        loginLayout->addStretch();
        stack->addWidget(loginPage);

        // === СТРАНИЦА РЕГИСТРАЦИИ ===
        QWidget *regPage = new QWidget;
        QVBoxLayout *regLayout = new QVBoxLayout(regPage);
        regLayout->setContentsMargins(40,20,40,20);
        
        QLabel *regTitle = new QLabel("РЕГИСТРАЦИЯ");
        regTitle->setAlignment(Qt::AlignCenter);
        regTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #61dafb;");

        regLogin = new QLineEdit; regLogin->setPlaceholderText("Придумайте логин");
        regPass = new QLineEdit; regPass->setPlaceholderText("Пароль"); regPass->setEchoMode(QLineEdit::Password);
        regEmail = new QLineEdit; regEmail->setPlaceholderText("Email (для связи)");
        regPhone = new QLineEdit; regPhone->setPlaceholderText("Телефон (+7...)");

        QPushButton *btnRegister = new QPushButton("Создать аккаунт");
        QPushButton *btnBack = new QPushButton("Назад");
        btnBack->setFlat(true);

        regLayout->addStretch();
        regLayout->addWidget(regTitle);
        regLayout->addWidget(regLogin);
        regLayout->addWidget(regPass);
        regLayout->addWidget(regEmail);
        regLayout->addWidget(regPhone);
        regLayout->addWidget(btnRegister);
        regLayout->addWidget(btnBack);
        regLayout->addStretch();
        stack->addWidget(regPage);

        // Коннекты
        connect(btnLogin, &QPushButton::clicked, this, &LoginWindow::onLogin);
        connect(btnGoReg, &QPushButton::clicked, [this](){ stack->setCurrentIndex(1); });
        connect(btnBack, &QPushButton::clicked, [this](){ stack->setCurrentIndex(0); });
        connect(btnRegister, &QPushButton::clicked, this, &LoginWindow::onRegister);
    }

private slots:
    void onLogin() {
        int userId = -1;
        if (DatabaseManager::instance().loginUser(loginEdit->text(), passEdit->text(), userId)) {
            MainWindow *mw = new MainWindow(userId);
            mw->show();
            this->close();
        } else {
            QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
        }
    }

    void onRegister() {
        // 1.1 ПРОВЕРКА ТЕЛЕФОНА
        QString phone = regPhone->text();
        QRegularExpression phoneRegex("^\\+7\\d{10}$"); // Начинается с +7, потом 10 цифр
        QRegularExpressionMatch match = phoneRegex.match(phone);

        if (!match.hasMatch()) {
            QMessageBox::warning(this, "Ошибка формата", "Номер телефона должен начинаться с +7 и содержать 10 цифр.\nПример: +79991234567");
            return;
        }

        if (regLogin->text().isEmpty() || regPass->text().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Заполните все поля");
            return;
        }

        if (DatabaseManager::instance().registerUser(regLogin->text(), regPass->text(), regEmail->text(), phone)) {
            QMessageBox::information(this, "Успех", "Аккаунт создан! Данные зашифрованы.");
            stack->setCurrentIndex(0);
        } else {
            QMessageBox::critical(this, "Ошибка", "Ошибка регистрации (возможно, логин занят)");
        }
    }
};

#endif // LOGINWINDOW_H
