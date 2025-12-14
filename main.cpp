#include <QApplication>
#include "LoginWindow.h"
#include "Database.h"
#include <QStyleFactory>

// Функция установки стилей (CSS для десктопа)
void setStyle(QApplication &app) {
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);

    // Дополнительная тонкая настройка через QSS
    app.setStyleSheet(
        "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"
        "QLineEdit { border: 1px solid #555; border-radius: 4px; padding: 5px; background: #333; color: white; selection-background-color: #2a82da; }"
        "QLineEdit:focus { border: 1px solid #2a82da; }"
        "QPushButton { background-color: #3f3f3f; border: 1px solid #555; border-radius: 4px; padding: 6px; color: white; }"
        "QPushButton:hover { background-color: #4d4d4d; border: 1px solid #2a82da; }"
        "QPushButton:pressed { background-color: #2a82da; }"
        "QTableWidget { gridline-color: #444; alternate-background-color: #353535; }"
        "QHeaderView::section { background-color: #353535; padding: 4px; border: 1px solid #444; color: #ddd; }"
        "QTabWidget::pane { border: 1px solid #444; }"
        "QTabBar::tab { background: #353535; color: #aaa; padding: 8px 15px; border-top-left-radius: 4px; border-top-right-radius: 4px; }"
        "QTabBar::tab:selected { background: #505050; color: white; border-bottom: 2px solid #2a82da; }"
    );
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Применяем тему
    setStyle(app);

    if (!DatabaseManager::instance().connectToDb()) {
        QMessageBox::critical(nullptr, "Ошибка БД", "Не удалось подключиться к базе данных Docker. Проверьте docker-compose.");
        return -1;
    }
    
    DatabaseManager::instance().initDb();

    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}
