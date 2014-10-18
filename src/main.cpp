#include <QApplication>

#include "maindialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainDialog ui;
    ui.show();

    return app.exec();
}
