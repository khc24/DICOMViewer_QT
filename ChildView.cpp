#include "ChildView.h"
#include <QDebug>  // ✅ qDebug() 사용을 위한 헤더 추가


CChildView::CChildView(QWidget* parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);  // 🔹 배경 자동 채우기 비활성화

    // ✅ QGridLayout을 부모 없이 생성
    layout = new QGridLayout(this);  // 🔹 바로 CChildView에 설정
    layout->setSpacing(0);  // 🔹 내부 간격 제거
    layout->setContentsMargins(0, 0, 0, 0);  // 🔹 여백 제거
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);

    // ✅ 4개의 VTK View를 생성하고 배경색 적용
    QColor colors[4] = { Qt::red, Qt::green, Qt::blue, Qt::yellow };
    for (int i = 0; i < 4; i++) {
        m_dlgVtkView[i] = new CDlgVtkView(i, this);
        //m_dlgVtkView[i]->setBackgroundColor(colors[i]);
        m_dlgVtkView[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        layout->addWidget(m_dlgVtkView[i], i / 2, i % 2);  // 🔹 2x2 그리드 배치
    }

    setLayout(layout);  // ✅ 바로 `CChildView`에 설정 (mainLayout 제거)
}


CChildView::~CChildView()
{
    // Qt에서는 QObject 기반의 객체는 자동으로 삭제되므로 별도 delete 필요 없음.
}

