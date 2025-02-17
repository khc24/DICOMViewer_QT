#include "DlgVtkView.h"
#include <QVBoxLayout>
#include <QKeyEvent>


CDlgVtkView::CDlgVtkView(int viewType, QWidget* parent)
    : QWidget(parent), m_ViewType(viewType)
{
    setAutoFillBackground(false);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_VtkWidget = new QVTKOpenGLNativeWidget(this);
    layout->addWidget(m_VtkWidget);

    // 2D View일 경우 Scroll bar 추가
    if (m_ViewType == DVManager::VIEW_AXIAL ||
        m_ViewType == DVManager::VIEW_CORONAL ||
        m_ViewType == DVManager::VIEW_SAGITTAL)
    {
        m_ScrollBar = new QScrollBar(Qt::Horizontal, this);
        layout->addWidget(m_ScrollBar);

        connect(m_ScrollBar, &QScrollBar::valueChanged, this, &CDlgVtkView::onScrollBarValueChanged);
    }

    setLayout(layout);

    // VTK 윈도우 초기화
    DVManager::Mgr()->InitVtkWindow(m_ViewType, m_VtkWidget);

}


CDlgVtkView::~CDlgVtkView()
{
    // Qt에서는 QObject 기반 객체는 자동으로 삭제되므로 delete 필요 없음.
}


void CDlgVtkView::setBackgroundColor(const QColor& color)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, color);
    setPalette(pal);
    setAutoFillBackground(true);
}


void CDlgVtkView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    int vtkHeight = height();
    if (m_ViewType == DVManager::VIEW_AXIAL ||
        m_ViewType == DVManager::VIEW_CORONAL ||
        m_ViewType == DVManager::VIEW_SAGITTAL)
    {
        m_ScrollBar->setGeometry(0, height() - 15, width(), 15);
        vtkHeight = std::max(1, vtkHeight - 15);  // 최소 높이 1 보장
    }

    m_VtkWidget->resize(std::max(1, width()), std::max(1, vtkHeight));
}


void CDlgVtkView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return || event->key() == Qt::Key_Escape)
    {
        event->ignore();  // ✅ Enter, Esc 키 무시하여 창이 닫히지 않도록 설정
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

void CDlgVtkView::onScrollBarValueChanged(int value)
{
    // DVManager의 ScrollSliceIndex 함수를 호출하여 슬라이스 인덱스를 업데이트합니다.
    DVManager::Mgr()->ScrollSliceIndex(m_ViewType, value);
}

void CDlgVtkView::UpdateScrollBar()
{
    qDebug() << "UpdateScrollBar : 1";
    if (!m_ScrollBar)
        return;

    qDebug() << "UpdateScrollBar : 2";
    // Volume 데이터 가져오기
    vtkSmartPointer<VolumeData> volumeData = DVManager::Mgr()->GetDicomLoader()->GetVolumeData();
    if (!volumeData)
        return;

    qDebug() << "UpdateScrollBar : 3";
    int ext[6];
    volumeData->GetImageData()->GetExtent(ext);

    // 뷰 타입에 따라 범위 설정 (예: VIEW_AXIAL이면 ext[4] ~ ext[5])
    if (m_ViewType == DVManager::VIEW_AXIAL)
        m_ScrollBar->setRange(ext[4], ext[5]);
    else if (m_ViewType == DVManager::VIEW_CORONAL)
        m_ScrollBar->setRange(ext[2], ext[3]);
    else if (m_ViewType == DVManager::VIEW_SAGITTAL)
        m_ScrollBar->setRange(ext[0], ext[1]);


    qDebug() << "m_ViewType = " << m_ViewType << "volumeData->GetSliceIndex(m_ViewType)=" <<
        volumeData->GetSliceIndex(m_ViewType);
    // 현재 슬라이스 인덱스로 설정
    m_ScrollBar->setValue(volumeData->GetSliceIndex(m_ViewType));
}
