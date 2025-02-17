#pragma once

#include <QDockWidget>
#include <QTreeWidget>
#include "DVManager.h"

class DicomGroupView : public QDockWidget
{
    Q_OBJECT

public:
    explicit DicomGroupView(QWidget* parent = nullptr);
    virtual ~DicomGroupView();

    /// ✅ DICOM 트리 업데이트
    void UpdateDicomTree();

    /// ✅ 특정 이름의 트리 항목을 찾거나 없으면 새로 생성
    QTreeWidgetItem* GetDicomGroupItem(const QString& itemText, QTreeWidgetItem* parentItem);

    /// ✅ 트리 전체 펼치기
    void ExpandAllDicomGroupTree();

protected:
    /// ✅ Qt 이벤트 처리 (MFC `OnSize()` 대체)
    void resizeEvent(QResizeEvent* event) override;

private slots:
    /// ✅ 트리에서 볼륨 노드를 더블 클릭했을 때 실행될 슬롯
    void OnTreeItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    /// ✅ DICOM 그룹 트리 (외부 접근 불가능하도록 `private` 지정)
    QTreeWidget* m_DicomGroupTree;
};
