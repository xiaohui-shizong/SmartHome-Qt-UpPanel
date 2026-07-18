#include "loginwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWidget w;
    w.setWindowTitle("智能管家登录界面");
    w.show();
    return a.exec();
}
