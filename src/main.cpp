#include <QApplication>

#include "gui/styles/GlobalStyles.cpp"
#include "gui/MainWindow/MainWindow.hpp"

int main(int argc, char *argv[])
{
    constexpr int DISPLAY_WIDTH = 1200;
    constexpr int DISPLAY_HEIGHT = 1000;
    const QString APP_NAME = "Audio Analyser";

    QApplication app(argc, argv);

    app.setApplicationName(APP_NAME);

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
    darkPalette.setColor(QPalette::Highlight, QColor(142, 45, 197));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    app.setPalette(darkPalette);

    MainWindow mainWindow;
    mainWindow.setWindowTitle(APP_NAME);
    mainWindow.show();
    mainWindow.resize(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    return app.exec();
}