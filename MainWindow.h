#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialogButtonBox>
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QTabWidget>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QFormLayout>
#include <QMessageBox>
#include <QDateEdit>
#include "Database.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    int currentUserId;
    QTableWidget *projectTable;
    QTableWidget *expTable;
    QLabel *userInfoLabel;

public:
    MainWindow(int userId, QWidget *parent = nullptr) : QMainWindow(parent), currentUserId(userId) {
        setWindowTitle("Система управления портфолио ИБ");
        resize(1000, 700);

        QWidget *centralWidget = new QWidget;
        setCentralWidget(centralWidget);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // === ВЕРХНЯЯ ПАНЕЛЬ ===
        QHBoxLayout *topPanel = new QHBoxLayout();
        userInfoLabel = new QLabel("Загрузка данных...");
        userInfoLabel->setStyleSheet("font-weight: bold; color: #ddd;");
        
        QPushButton *btnProfile = new QPushButton("Мой Профиль (Личные данные)");
        QPushButton *btnExit = new QPushButton("Выход");
        btnExit->setStyleSheet("background-color: #d32f2f; color: white; border: none; padding: 5px 15px;");
        
        topPanel->addWidget(userInfoLabel);
        topPanel->addStretch();
        topPanel->addWidget(btnProfile);
        topPanel->addWidget(btnExit);
        mainLayout->addLayout(topPanel);

        // === ВКЛАДКИ (TABS) ===
        QTabWidget *tabs = new QTabWidget();
        mainLayout->addWidget(tabs);

        // --- Вкладка 1: Проекты (Портфолио) ---
        QWidget *tabProjects = new QWidget();
        QVBoxLayout *projLayout = new QVBoxLayout(tabProjects);
        
        projectTable = new QTableWidget();
        projectTable->setColumnCount(4);
        projectTable->setHorizontalHeaderLabels({"Название проекта", "Роль / Технологии", "Дата", "Описание"});
        projectTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        projectTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
        // ЗАПРЕТ РЕДАКТИРОВАНИЯ:
        projectTable->setEditTriggers(QAbstractItemView::NoEditTriggers); 
        projectTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        
        QPushButton *btnAddProj = new QPushButton("Добавить проект");
        btnAddProj->setStyleSheet("background-color: #388e3c; color: white; padding: 8px;");

        projLayout->addWidget(projectTable);
        projLayout->addWidget(btnAddProj);
        tabs->addTab(tabProjects, "Мои Проекты");

        // --- Вкладка 2: Опыт работы (Резюме) ---
        QWidget *tabExp = new QWidget();
        QVBoxLayout *expLayout = new QVBoxLayout(tabExp);

        expTable = new QTableWidget();
        expTable->setColumnCount(4);
        expTable->setHorizontalHeaderLabels({"Компания", "Должность", "Период", "Достижения / Обязанности"});
        expTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
        expTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Только чтение
        expTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        QPushButton *btnAddExp = new QPushButton("Добавить опыт работы");
        btnAddExp->setStyleSheet("background-color: #1976d2; color: white; padding: 8px;");

        expLayout->addWidget(expTable);
        expLayout->addWidget(btnAddExp);
        tabs->addTab(tabExp, "Опыт работы");

        // === ЛОГИКА ===
        loadUserData();
        loadProjects();
        loadExperience();

        connect(btnAddProj, &QPushButton::clicked, this, &MainWindow::addProjectDialog);
        connect(btnAddExp, &QPushButton::clicked, this, &MainWindow::addExperienceDialog);
        connect(btnExit, &QPushButton::clicked, this, &QMainWindow::close);
        connect(btnProfile, &QPushButton::clicked, this, &MainWindow::showSecurityInfo);
    }

private:
    void loadUserData() {
        QString email, phone;
        if(DatabaseManager::instance().getUserProfile(currentUserId, email, phone)) {
            userInfoLabel->setText("Пользователь ИБ | Контакты защищены");
        }
    }

    void showSecurityInfo() {
        QString email, phone;
        DatabaseManager::instance().getUserProfile(currentUserId, email, phone);
        QMessageBox::information(this, "Личные данные", 
            "ВНИМАНИЕ: Данные расшифрованы (AES-256) только для вас.\n\n"
            "Email: " + email + "\n"
            "Телефон: " + phone);
    }

    void loadProjects() {
        projectTable->setRowCount(0);
        QSqlQuery q = DatabaseManager::instance().getProjects(currentUserId);
        while(q.next()) {
            int row = projectTable->rowCount();
            projectTable->insertRow(row);
            projectTable->setItem(row, 0, new QTableWidgetItem(q.value(0).toString()));
            projectTable->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));
            projectTable->setItem(row, 2, new QTableWidgetItem(q.value(2).toString()));
            projectTable->setItem(row, 3, new QTableWidgetItem(q.value(3).toString()));
        }
    }

    void loadExperience() {
        expTable->setRowCount(0);
        QSqlQuery q = DatabaseManager::instance().getExperience(currentUserId);
        while(q.next()) {
            int row = expTable->rowCount();
            expTable->insertRow(row);
            expTable->setItem(row, 0, new QTableWidgetItem(q.value(0).toString()));
            expTable->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));
            expTable->setItem(row, 2, new QTableWidgetItem(q.value(2).toString()));
            // Tooltip для длинного текста
            QTableWidgetItem* descItem = new QTableWidgetItem(q.value(3).toString());
            descItem->setToolTip(q.value(3).toString());
            expTable->setItem(row, 3, descItem);
        }
    }

    // Диалог добавления проекта
    void addProjectDialog() {
        QDialog dlg(this);
        dlg.setWindowTitle("Новый проект");
        dlg.resize(400, 300);
        QFormLayout *layout = new QFormLayout(&dlg);
        
        QLineEdit *title = new QLineEdit();
        QLineEdit *role = new QLineEdit(); role->setPlaceholderText("Например: Senior Pentester, Разработчик");
        QDateEdit *date = new QDateEdit(QDate::currentDate());
        QTextEdit *desc = new QTextEdit(); desc->setPlaceholderText("Опишите стек технологий, найденные уязвимости...");

        layout->addRow("Название:", title);
        layout->addRow("Роль:", role);
        layout->addRow("Дата завершения:", date);
        layout->addRow("Описание:", desc);

        QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        layout->addRow(btns);

        if(dlg.exec() == QDialog::Accepted) {
            DatabaseManager::instance().addProject(currentUserId, title->text(), role->text(), date->text(), desc->toPlainText());
            loadProjects();
        }
    }

    // Диалог добавления опыта
    void addExperienceDialog() {
        QDialog dlg(this);
        dlg.setWindowTitle("Добавить опыт работы");
        dlg.resize(400, 400);
        QFormLayout *layout = new QFormLayout(&dlg);
        
        QLineEdit *company = new QLineEdit();
        QLineEdit *position = new QLineEdit();
        QLineEdit *years = new QLineEdit(); years->setPlaceholderText("Например: 2020-2023");
        QTextEdit *achievements = new QTextEdit(); achievements->setPlaceholderText("Ключевые достижения, обязанности...");

        layout->addRow("Компания:", company);
        layout->addRow("Должность:", position);
        layout->addRow("Период:", years);
        layout->addRow("Достижения:", achievements);

        QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        layout->addRow(btns);

        if(dlg.exec() == QDialog::Accepted) {
            DatabaseManager::instance().addExperience(currentUserId, company->text(), position->text(), years->text(), achievements->toPlainText());
            loadExperience();
        }
    }
};

#endif // MAINWINDOW_H
