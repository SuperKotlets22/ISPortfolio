#ifndef EMPLOYERWINDOW_H
#define EMPLOYERWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QMessageBox>
#include "Database.h"

class EmployerWindow : public QMainWindow {
    Q_OBJECT
    QListWidget *userList;
    QTextBrowser *resumeView;

public:
    explicit EmployerWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Кабинет Работодателя");
        QWidget *central = new QWidget;
        setCentralWidget(central);
        QVBoxLayout *main = new QVBoxLayout(central);

        // Шапка
        QHBoxLayout *top = new QHBoxLayout();
        QLabel *logo = new QLabel("Поиск Кандидатов");
        logo->setStyleSheet("font-size: 20px; font-weight: bold; color: #bb86fc;");
        QPushButton *btnExit = new QPushButton("Выйти");
        btnExit->setStyleSheet("background-color: #cf6679; color: black;");
        top->addWidget(logo); top->addStretch(); top->addWidget(btnExit);
        main->addLayout(top);

        QSplitter *split = new QSplitter();
        split->setHandleWidth(1); // Тонкий разделитель
        split->setStyleSheet("QSplitter::handle { background-color: #444; }");
        main->addWidget(split);

        // Левая панель
        QWidget *left = new QWidget;
        QVBoxLayout *ll = new QVBoxLayout(left);
        ll->setContentsMargins(0,0,10,0);
        ll->addWidget(new QLabel("Список специалистов:"));
        userList = new QListWidget();
        QPushButton *btnRef = new QPushButton("Обновить список");
        ll->addWidget(userList); ll->addWidget(btnRef);
        split->addWidget(left);

        // Правая панель (Резюме)
        QWidget *right = new QWidget;
        QVBoxLayout *rl = new QVBoxLayout(right);
        rl->setContentsMargins(10,0,0,0);
        
        resumeView = new QTextBrowser();
        resumeView->setOpenExternalLinks(false);
        // ТЕМНЫЙ ФОН ДЛЯ РЕЗЮМЕ
        resumeView->setStyleSheet("background-color: #1e1e1e; border: 1px solid #333; padding: 20px;");
        
        QPushButton *btnContact = new QPushButton("Показать зашифрованные контакты");
        btnContact->setStyleSheet("background-color: #03dac6; color: black; font-weight: bold; padding: 12px;");
        
        rl->addWidget(resumeView);
        rl->addWidget(btnContact);
        split->addWidget(right);
        split->setStretchFactor(1, 3);

        loadList();

        connect(btnExit, &QPushButton::clicked, [this](){ emit logoutRequested(); this->close(); });
        connect(btnRef, &QPushButton::clicked, this, &EmployerWindow::loadList);
        connect(userList, &QListWidget::itemClicked, this, &EmployerWindow::loadResume);
        connect(btnContact, &QPushButton::clicked, this, &EmployerWindow::showContacts);
    }

signals:
    void logoutRequested();

private:
    void loadList() {
        userList->clear();
        auto list = DatabaseManager::instance().getAllSpecialists();
        for(auto &p : list) {
            QListWidgetItem *it = new QListWidgetItem(p.second);
            it->setData(Qt::UserRole, p.first);
            userList->addItem(it);
        }
    }

    void loadResume(QListWidgetItem *item) {
        int uid = item->data(Qt::UserRole).toInt();
        UserProfile p;
        bool hasProfile = DatabaseManager::instance().getProfile(uid, p);

        // ГЕНЕРАЦИЯ ТЕМНОГО HTML
        QString html = "<html><body style='font-family: sans-serif; color: #e0e0e0; background-color: #1e1e1e;'>";
        
        html += "<div style='border-bottom: 2px solid #bb86fc; padding-bottom: 10px; margin-bottom: 20px;'>";
        html += "<h1 style='color: #ffffff; margin:0;'>" + (hasProfile ? p.surname + " " + p.name + " " + p.patronymic : "Без имени") + "</h1>";
        html += "<h2 style='color: #bb86fc; margin:5px 0;'>" + (hasProfile ? p.jobTitle : "Должность не указана") + "</h2>";
        html += "</div>";
        
        if(hasProfile) {
            html += "<table style='width: 100%; color: #ccc;'>";
            html += "<tr><td><b>Зарплата:</b> " + QString::number(p.salary) + " руб.</td><td><b>График:</b> " + p.schedule + "</td></tr>";
            html += "<tr><td><b>Город:</b> " + p.city + "</td><td><b>Переезд:</b> " + (p.readyToMove ? "Да" : "Нет") + "</td></tr>";
            html += "<tr><td><b>Дата рождения:</b> " + p.birthDate.toString("dd.MM.yyyy") + "</td><td><b>Пол:</b> " + p.gender + "</td></tr>";
            html += "</table><hr style='border: 1px solid #444;'>";
        }

        // Опыт
        html += "<h3 style='color: #03dac6;'>Опыт работы</h3>";
        QSqlQuery qExp = DatabaseManager::instance().getExperience(uid);
        bool hasExp = false;
        while(qExp.next()) {
            hasExp = true;
            QString period = qExp.value(2).toDate().toString("MM.yyyy") + " — " + qExp.value(3).toDate().toString("MM.yyyy");
            html += "<div style='background-color: #252525; padding: 10px; border-radius: 5px; margin-bottom: 10px;'>";
            html += "<div style='font-weight: bold; font-size: 16px; color: #fff;'>" + qExp.value(1).toString() + "</div>"; 
            html += "<div style='color: #888; font-size: 13px;'>" + qExp.value(0).toString() + " | " + period + "</div>";
            html += "<div style='margin-top: 5px; color: #ddd;'>" + qExp.value(4).toString() + "</div>";
            html += "</div>";
        }
        if(!hasExp) html += "<p style='color: #666;'>Нет данных.</p>";

        // Образование
        html += "<h3 style='color: #03dac6;'>Образование</h3>";
        QSqlQuery qEdu = DatabaseManager::instance().getEducation(uid);
        while(qEdu.next()) {
            html += "<div style='margin-bottom: 10px;'>";
            html += "<b>" + qEdu.value(3).toString() + "</b> — <span style='color:#fff;'>" + qEdu.value(0).toString() + "</span><br>";
            html += "<span style='color:#aaa;'>" + qEdu.value(1).toString() + ", " + qEdu.value(2).toString() + "</span>";
            html += "</div>";
        }

        html += "</body></html>";
        resumeView->setHtml(html);
    }

    void showContacts() {
        if(userList->selectedItems().isEmpty()) return;
        int uid = userList->selectedItems().first()->data(Qt::UserRole).toInt();
        QString e, p;
        if(DatabaseManager::instance().getUserContacts(uid, e, p)) {
            QMessageBox::information(this, "Контакты (Расшифровано)", "Email: " + e + "\nТелефон: " + p);
        }
    }
};

#endif // EMPLOYERWINDOW_H