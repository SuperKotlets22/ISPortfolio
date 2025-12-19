#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QRegularExpression>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include "Database.h"
#include "SpecialistWindow.h"
#include "EmployerWindow.h"

class LoginWindow : public QWidget {
    Q_OBJECT
    
    QLineEdit *loginEdit, *passEdit;
    QLineEdit *regLogin, *regPass, *regEmail, *regPhone;
    QLabel *statusLabel;
    QStackedWidget *stack;
    QPushButton *btnRoleSpec;
    QPushButton *btnRoleEmpl;
    QString selectedRole = "specialist";

public:
    LoginWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Portfolio System");
        resize(1200, 800); 

        // Градиент фона (Темно-синий в черный)
        this->setStyleSheet("LoginWindow { background: qradialgradient(cx:0.5, cy:0.5, radius: 1, fx:0.5, fy:0.5, stop:0 #0f0c29, stop:1 #000000); }");

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addStretch(); 

        // КАРТОЧКА
        QFrame *card = new QFrame();
        card->setObjectName("AuthCard"); 
        card->setFixedWidth(460); 
        card->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum); 
        
        card->setStyleSheet(
            // 1. Фон карточки - 
            "QFrame#AuthCard { "
            "    background-color: #09090b; " 
            "    border-radius: 24px; "
            "    border: 1px solid #27272a; " 
            "}"
            
            // 2. Поля ввода 
            "QLineEdit { "
            "    background-color: #27272a; " 
            "    border: none; "              
            "    border-radius: 12px; "       
            "    padding: 16px; "             
            "    color: #ececf1; "            
            "    font-size: 15px; "
            "    margin-bottom: 16px; "
            "}"
            
            // При наведении - чуть светлее
            "QLineEdit:hover { "
            "    background-color: #3f3f46; "
            "}"
            
            // При клике - еще светлее + акцент
            "QLineEdit:focus { "
            "    background-color: #3f3f46; "
            "    border: 1px solid #7f5af0; " /* Тонкая фиолетовая обводка */
            "    color: #ffffff; "
            "}"
            
            "QLabel { background: transparent; border: none; }"
        );

        // Тень
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(60); 
        shadow->setColor(QColor(0,0,0, 200)); 
        shadow->setOffset(0, 15);
        card->setGraphicsEffect(shadow);

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(50, 50, 50, 50); 
        cardLayout->setSpacing(0); 

        stack = new QStackedWidget;
        stack->setStyleSheet("background: transparent; border: none;");
        stack->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        cardLayout->addWidget(stack);
        
        mainLayout->addWidget(card, 0, Qt::AlignCenter);
        mainLayout->addStretch();

        setupLoginPage();
        setupRegisterPage();
    }

    void clearInputs() {
        loginEdit->clear(); passEdit->clear();
        regLogin->clear(); regPass->clear(); regEmail->clear(); regPhone->clear();
        statusLabel->clear(); statusLabel->hide();
        btnRoleSpec->setChecked(true); selectedRole = "specialist"; updateRoleButtons();
    }

private:
    void setupLoginPage() {
        QWidget *page = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(5);
        layout->setAlignment(Qt::AlignTop); 

        QLabel *title = new QLabel("Вход");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 30px; font-weight: bold; color: white; margin-bottom: 25px; letter-spacing: 1px;");

        statusLabel = new QLabel("");
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setWordWrap(true);
        statusLabel->hide();

        loginEdit = new QLineEdit; loginEdit->setPlaceholderText("Логин");
        passEdit = new QLineEdit; passEdit->setPlaceholderText("Пароль"); passEdit->setEchoMode(QLineEdit::Password);
        
        QPushButton *btnLogin = new QPushButton("Войти");
        btnLogin->setCursor(Qt::PointingHandCursor);
        btnLogin->setStyleSheet(
            "QPushButton { "
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #7f5af0, stop:1 #6246ea); "
            "    color: white; border: none; padding: 15px; border-radius: 12px; font-weight: bold; font-size: 16px; margin-top: 10px;"
            "}"
            "QPushButton:hover { background: #5a3bc4; }"
        );
        
        QPushButton *btnGoReg = new QPushButton("Нет аккаунта? Зарегистрироваться");
        btnGoReg->setObjectName("TextBtn");
        btnGoReg->setCursor(Qt::PointingHandCursor);
        btnGoReg->setStyleSheet("QPushButton { background: transparent; color: #949BA4; border: none; margin-top: 10px; font-size: 13px; } QPushButton:hover { color: #ffffff; }");

        layout->addWidget(title);
        layout->addWidget(statusLabel);
        layout->addWidget(loginEdit);
        layout->addWidget(passEdit);
        layout->addWidget(btnLogin);
        layout->addWidget(btnGoReg);
        
        stack->addWidget(page);

        connect(btnLogin, &QPushButton::clicked, this, &LoginWindow::onLogin);
        connect(btnGoReg, &QPushButton::clicked, [this](){ clearInputs(); stack->setCurrentIndex(1); });
    }

    void setupRegisterPage() {
        QWidget *page = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(5);
        layout->setAlignment(Qt::AlignTop);

        QLabel *title = new QLabel("Регистрация");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 26px; font-weight: bold; color: white; margin-bottom: 20px;");

        QHBoxLayout *roleLayout = new QHBoxLayout();
        roleLayout->setSpacing(10);
        btnRoleSpec = new QPushButton("Специалист"); btnRoleSpec->setCheckable(true); btnRoleSpec->setChecked(true);
        btnRoleEmpl = new QPushButton("Работодатель"); btnRoleEmpl->setCheckable(true);
        roleLayout->addWidget(btnRoleSpec); roleLayout->addWidget(btnRoleEmpl);
        updateRoleButtons();

        QButtonGroup *group = new QButtonGroup(this); group->addButton(btnRoleSpec); group->addButton(btnRoleEmpl);
        connect(btnRoleSpec, &QPushButton::toggled, [this](bool c){ if(c) { selectedRole="specialist"; updateRoleButtons(); } });
        connect(btnRoleEmpl, &QPushButton::toggled, [this](bool c){ if(c) { selectedRole="employer"; updateRoleButtons(); } });

        QWidget* spacer = new QWidget(); spacer->setFixedHeight(15); layout->addWidget(spacer);

        regLogin = new QLineEdit; regLogin->setPlaceholderText("Придумайте логин");
        regPass = new QLineEdit; regPass->setPlaceholderText("Пароль"); regPass->setEchoMode(QLineEdit::Password);
        regEmail = new QLineEdit; regEmail->setPlaceholderText("Email");
        regPhone = new QLineEdit; regPhone->setPlaceholderText("Телефон (+7...)");

        QPushButton *btnReg = new QPushButton("Создать аккаунт");
        btnReg->setStyleSheet(
            "QPushButton { "
            "    background-color: #2cb67d; "
            "    color: white; border: none; padding: 15px; border-radius: 12px; font-weight: bold; font-size: 16px; margin-top: 10px;"
            "}"
            "QPushButton:hover { background-color: #239263; }"
        );
        btnReg->setCursor(Qt::PointingHandCursor);

        QPushButton *btnBack = new QPushButton("Назад");
        btnBack->setObjectName("TextBtn");
        btnBack->setStyleSheet("QPushButton { background: transparent; color: #949BA4; border: none; margin-top: 10px; } QPushButton:hover { color: white; }");
        btnBack->setCursor(Qt::PointingHandCursor);

        layout->addWidget(title);
        layout->addLayout(roleLayout);
        layout->addWidget(spacer);
        layout->addWidget(regLogin);
        layout->addWidget(regPass);
        layout->addWidget(regEmail);
        layout->addWidget(regPhone);
        layout->addWidget(btnReg);
        layout->addWidget(btnBack);

        stack->addWidget(page);
        connect(btnBack, &QPushButton::clicked, [this](){ clearInputs(); stack->setCurrentIndex(0); });
        connect(btnReg, &QPushButton::clicked, this, &LoginWindow::onRegister);
    }

    void updateRoleButtons() {
        QString base = "QPushButton { padding: 12px; border-radius: 8px; border: 1px solid #333; background: #2b2d31; color: #949BA4; font-weight: bold;} QPushButton:hover { border-color: #7f5af0; color: white; }";
        QString active = "QPushButton { background-color: #7f5af0; color: white; border: none; padding: 12px; border-radius: 8px; font-weight: bold; }";
        
        if (selectedRole == "specialist") { btnRoleSpec->setStyleSheet(active); btnRoleEmpl->setStyleSheet(base); }
        else { btnRoleSpec->setStyleSheet(base); btnRoleEmpl->setStyleSheet(active); }
    }
    
    // МЕТОДЫ onLogin и onRegister (Такие же)
    
private slots:
    void onLogin() {
        int userId = -1;
        QString role;
        if (DatabaseManager::instance().loginUser(loginEdit->text(), passEdit->text(), userId, role)) {
            statusLabel->hide();
            auto logoutHandler = [this](){ clearInputs(); stack->setCurrentIndex(0); this->showMaximized(); };
            if (role == "specialist") {
                SpecialistWindow *w = new SpecialistWindow(userId);
                connect(w, &SpecialistWindow::logoutRequested, this, logoutHandler);
                w->showMaximized();
            } else {
                EmployerWindow *w = new EmployerWindow();
                connect(w, &EmployerWindow::logoutRequested, this, logoutHandler);
                w->showMaximized();
            }
            this->hide();
        } else {
            statusLabel->setText("Неверный логин или пароль");
            statusLabel->setStyleSheet("color: #ff5050; margin-bottom: 10px; background: transparent; border: none; font-weight: bold;");
            statusLabel->show();
        }
    }

    void onRegister() {
        QString phone = regPhone->text();
        QRegularExpression re("^\\+7\\d{10}$");
        if (regLogin->text().isEmpty() || regPass->text().isEmpty()) return;
        if (!re.match(phone).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Телефон должен быть в формате +79990000000");
            return;
        }
        if (DatabaseManager::instance().registerUser(regLogin->text(), regPass->text(), regEmail->text(), phone, selectedRole)) {
            clearInputs();
            stack->setCurrentIndex(0);
            loginEdit->setText(regLogin->text());
            statusLabel->setText("Аккаунт создан! Введите пароль.");
            statusLabel->setStyleSheet("color: #2cb67d; margin-bottom: 10px; background: transparent; border: none; font-weight: bold;");
            statusLabel->show();
        } else {
            QMessageBox::warning(this, "Ошибка", "Логин занят");
        }
    }
};

#endif // LOGINWINDOW_H