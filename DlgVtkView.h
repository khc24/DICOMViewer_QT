﻿#pragma once
#include <QWidget>
#include <QScrollBar>
#include <QVBoxLayout>
#include "DVManager.h"

class CDlgVtkView : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgVtkView(int viewType, QWidget* parent = nullptr);
    ~CDlgVtkView();

    // View Type 설정 및 반환
    int getViewType() const { return m_ViewType; }
    void setViewType(int val) { m_ViewType = val; }
    void setBackgroundColor(const QColor& color);



private:
    int m_ViewType;  // 이 Widget의 View Type
    QScrollBar* m_ScrollBar;  // Qt의 Scroll Bar 객체
    QVTKOpenGLNativeWidget* m_VtkWidget;
    
    protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
//protected:
//    void resizeEvent(QResizeEvent* event) override;  // 🔹 창 크기 변경 이벤트
};
