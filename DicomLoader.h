#pragma once

#include <vector>
#include <QString>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>

#include "DicomGroup.h"
#include "VolumeData.h"

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

    /// 현재 선택된 DICOM Group
    vtkSmartPointer<DicomGroup> m_CurrentGroup;

    /// 현재 선택된 Volume 데이터
    vtkSmartPointer<VolumeData> m_VolumeData;

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

    /// 현재 선택된 DICOM Group
    vtkSmartPointer<DicomGroup> GetCurrentGroup() const { return m_CurrentGroup; }

    /// Volume 데이터
    vtkSmartPointer<VolumeData> GetVolumeData() const { return m_VolumeData; }

    /// DICOM 그룹에서 Volume 데이터 로드
    void LoadVolumeData(vtkSmartPointer<DicomGroup> dicomGroup);


};