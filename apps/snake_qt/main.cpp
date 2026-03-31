#include "snake_window.hpp"

#include <QApplication>
#include <QFont>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Snake"));
    QApplication::setApplicationDisplayName(QStringLiteral("Snake"));
    QApplication::setOrganizationName(QStringLiteral("mb.cpp-app-template"));

    QFont f = QApplication::font();
    f.setStyleStrategy(QFont::PreferAntialias);
    QApplication::setFont(f);

    SnakeWindow w;
    w.show();
    return app.exec();
}
