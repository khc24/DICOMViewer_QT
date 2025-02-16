#include "MainWindow.h"
#include "QPalette"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{

    //// 🔹 배경색 설정 (디버깅용)
    //QPalette pal = palette();
    //pal.setColor(QPalette::Window, Qt::red);  // ✅ Qt 6에서는 Background 대신 Window 사용
    //setPalette(pal);
    //setAutoFillBackground(true);

    CChildView* childView = new CChildView(this);
    setCentralWidget(childView);  // 🔹 `CChildView`를 QMainWindow의 중앙 위젯으로 설정
}

MainWindow::~MainWindow()
{
}
