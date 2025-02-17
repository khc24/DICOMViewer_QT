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


private:
    CDlgVtkView* m_dlgVtkView[4];  // 🔹 4개의 VTK 뷰 저장
    QGridLayout* layout;  // 🔹 2x2 레이아웃 관리

public:
    CDlgVtkView* GetDlgVtkView(int viewType);
};
