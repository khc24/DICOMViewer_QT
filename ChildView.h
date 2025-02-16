#pragma once

#include <QWidget>
#include <QGridLayout>
#include "DlgVtkView.h"
#include <QShowEvent>


class CChildView : public QWidget
{
    Q_OBJECT

public:
    explicit CChildView(QWidget* parent = nullptr);
    ~CChildView();



//protected:
    //void resizeEvent(QResizeEvent* event) override;  // 🔹 창 크기 변경 이벤트

private:
    CDlgVtkView* m_dlgVtkView[4];  // 🔹 4개의 VTK 뷰 저장
    QGridLayout* layout;  // 🔹 2x2 레이아웃 관리
};
