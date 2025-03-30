#include <QApplication>
#include "model/model.h"
#include "controller/controller.h"

int main(int argc, char* argv[])
{
    QFont appFont("Segoe UI", 10);

    QApplication app(argc, argv);

    app.setStyleSheet(
        "QWidget { "
        "background-color: #1D1B20;"
        "color:#FFFFFF "
        "}"
        "QMainWindow {"
        "background: #625B71;"
        "}"
        "QTextEdit {"
        "font-size: 16px;"
        "border:none;"
        "}"
    );

    Model model;
    MainWindow mainWindow;
    MainController mainController(&model, &mainWindow);

    appFont.setStyleHint(QFont::Courier);
    QApplication::setFont(appFont);

    mainWindow.show();

    return app.exec();
}
