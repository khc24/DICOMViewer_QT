#include "DicomGroupView.h"
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QDebug>

DicomGroupView::DicomGroupView(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle("DICOM Group View");

    // ✅ 트리 위젯 생성
    m_DicomGroupTree = new QTreeWidget(this);
    m_DicomGroupTree->setColumnCount(1);
    m_DicomGroupTree->setHeaderLabel("DICOM Groups");

    // ✅ 레이아웃 설정
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_DicomGroupTree);
    QWidget* container = new QWidget;
    container->setLayout(layout);
    setWidget(container);

    // ✅ 트리 항목 더블 클릭 이벤트와 슬롯 연결
    connect(m_DicomGroupTree, &QTreeWidget::itemDoubleClicked,
        this, &DicomGroupView::OnTreeItemDoubleClicked);
}

DicomGroupView::~DicomGroupView() {}

/// ✅ 창 크기 변경 시 트리 크기 자동 조정 (MFC `OnSize()` 대체)
void DicomGroupView::resizeEvent(QResizeEvent* event)
{
    QDockWidget::resizeEvent(event);
    if (m_DicomGroupTree) {
        m_DicomGroupTree->resize(event->size());
    }
}

/// ✅ 트리에서 볼륨 노드를 더블 클릭했을 때 실행되는 함수
void DicomGroupView::OnTreeItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column); // 사용되지 않는 매개변수 (경고 방지)

    if (!item) return; // 선택된 항목이 없으면 종료

    // ✅ QTreeWidgetItem에 저장된 DICOM Group 포인터 가져오기
    QVariant data = item->data(0, Qt::UserRole);
    if (!data.isValid()) return; // 저장된 데이터가 유효하지 않으면 종료

    DicomGroup* group = static_cast<DicomGroup*>(data.value<void*>());
    if (!group) return; // 포인터가 NULL이면 종료

    qDebug() << "DICOM 그룹 선택됨: " << QString::fromStdString(group->GetPatientID());

    // ✅ DICOM 그룹 선택 처리 (DVManager에서 해당 데이터 로드 및 렌더링 수행)
    DVManager::Mgr()->OnSelectDicomGroup(group);
}

/// ✅ DICOM 트리 업데이트 (MFC `UpdateDicomTree()` 변환)
void DicomGroupView::UpdateDicomTree()
{
    // ✅ 기존 아이템 삭제
    m_DicomGroupTree->clear();

    int volumeCount = 1;
    auto dicomLoader = DVManager::Mgr()->GetDicomLoader();

    for (int i = 0; i < dicomLoader->GetNumberOfGroups(); i++) {
        auto curGroup = dicomLoader->GetDicomGroup(i);
        curGroup->LoadDicomInfo();

        QString patientId = QString::fromStdString(curGroup->GetPatientID());
        QString studyId = QString::fromStdString(curGroup->GetStudyID());
        QString studyDesc = QString::fromStdString(curGroup->GetStudyDescription());
        QString seriesNum = QString::fromStdString(curGroup->GetSeriesNum());
        QString seriesDesc = QString::fromStdString(curGroup->GetSeriesDescription());

        // ✅ 트리 노드 추가
        QTreeWidgetItem* hCurPatientNode = GetDicomGroupItem("Patient_" + patientId, nullptr);
        QTreeWidgetItem* hCurStudyNode = GetDicomGroupItem("Study_" + studyId + " (" + studyDesc + ")", hCurPatientNode);
        QTreeWidgetItem* hCurSeriesNode = GetDicomGroupItem("Series_" + seriesNum + " (" + seriesDesc + ")", hCurStudyNode);

        // ✅ 볼륨 노드 추가
        QString volumeString = QString("Volume (%1 slices)").arg(curGroup->GetFileList().size());
        QTreeWidgetItem* volumeNode = new QTreeWidgetItem(hCurSeriesNode, QStringList(volumeString));
        volumeNode->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(curGroup)));
        hCurSeriesNode->addChild(volumeNode);
    }

    // ✅ 트리 전체 확장
    ExpandAllDicomGroupTree();
}

/// ✅ 기존 트리 노드를 찾거나 새로 생성 (MFC `GetDicomGroupItem()` 변환)
QTreeWidgetItem* DicomGroupView::GetDicomGroupItem(const QString& itemText, QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* targetNode = nullptr;

    if (parentItem == nullptr) {
        // ✅ 루트 노드 검색
        for (int i = 0; i < m_DicomGroupTree->topLevelItemCount(); i++) {
            if (m_DicomGroupTree->topLevelItem(i)->text(0) == itemText) {
                return m_DicomGroupTree->topLevelItem(i);
            }
        }
        targetNode = new QTreeWidgetItem(m_DicomGroupTree, QStringList(itemText));
        m_DicomGroupTree->addTopLevelItem(targetNode);
    }
    else {
        // ✅ 자식 노드 검색
        for (int i = 0; i < parentItem->childCount(); i++) {
            if (parentItem->child(i)->text(0) == itemText) {
                return parentItem->child(i);
            }
        }
        targetNode = new QTreeWidgetItem(parentItem, QStringList(itemText));
        parentItem->addChild(targetNode);
    }

    return targetNode;
}

/// ✅ 트리 전체 확장 (MFC `ExpandAllDicomGroupTree()` 변환)
void DicomGroupView::ExpandAllDicomGroupTree()
{
    for (int i = 0; i < m_DicomGroupTree->topLevelItemCount(); i++) {
        QTreeWidgetItem* patientNode = m_DicomGroupTree->topLevelItem(i);
        patientNode->setExpanded(true);

        for (int j = 0; j < patientNode->childCount(); j++) {
            QTreeWidgetItem* studyNode = patientNode->child(j);
            studyNode->setExpanded(true);

            for (int k = 0; k < studyNode->childCount(); k++) {
                QTreeWidgetItem* seriesNode = studyNode->child(k);
                seriesNode->setExpanded(true);
            }
        }
    }
}
