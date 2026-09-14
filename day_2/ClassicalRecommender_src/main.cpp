#include <QApplication>
#include "mainwindow.h"
#include "styles.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 간단한 스타일 (GUI 완성도 향상용)
    /*pp.setStyleSheet(R"(
        QWidget { font-size: 14px; }
        QLabel#title { font-size: 22px; font-weight: bold; margin: 12px; }
        QFrame#card { background: #f5f3ef; border-radius: 8px; }
        QPushButton {
            padding: 8px 14px; border-radius: 6px;
            border: 1px solid #c9c3b8; background: #ffffff;
        }
        QPushButton:hover { background: #efeae0; }
        QPushButton:checked { background: #7a5cff; color: white; border: none; }
        QListWidget { border: 1px solid #d8d2c6; border-radius: 6px; padding: 4px; }
    )");*/
    app.setStyleSheet(kAppStyle);

    MainWindow w;
    w.show();
    return app.exec();
}
