#pragma once

#include <vector>
#include <QString>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>

#include "DicomGroup.h"

class DicomLoader : public vtkObject
{
    //Q_OBJECT  // Qt의 신호/슬롯 사용 가능

public:
    vtkTypeMacro(DicomLoader, vtkObject);
    static DicomLoader* New() { return new DicomLoader(); }

protected:
    DicomLoader();
    virtual ~DicomLoader();

protected:
    /// DICOM 그룹 목록
    std::vector<vtkSmartPointer<DicomGroup>> m_GroupList;

public:

    bool IsDicomFile(const QString& filePath);

    /// DICOM 디렉토리 열기
    void OpenDicomDirectory(const QString& dirPath);

    /// DICOM 파일 추가
    void AddDicomFile(const QString& filePath);

    /// DICOM 그룹 개수 반환
    int GetNumberOfGroups() { return static_cast<int>(m_GroupList.size()); }

    /// 특정 DICOM 그룹 반환
    vtkSmartPointer<DicomGroup> GetDicomGroup(int idx);
};