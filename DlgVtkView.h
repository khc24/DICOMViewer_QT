#pragma once
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

    /// Volume 데이터가 로드되면 각 Slice별 인덱스 범위에 따라 스크롤바 업데이트
    void UpdateScrollBar();


private:
    int m_ViewType;  // 이 Widget의 View Type
    QScrollBar* m_ScrollBar;  // Qt의 Scroll Bar 객체
    QVTKOpenGLNativeWidget* m_VtkWidget;
    

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onScrollBarValueChanged(int value);

};
