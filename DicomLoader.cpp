#include "DicomLoader.h"

#include <gdcmReader.h>
#include <gdcmFile.h>
#include <gdcmDataSet.h>
#include <gdcmStringFilter.h>
#include <gdcmTag.h>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

DicomLoader::DicomLoader() {}

DicomLoader::~DicomLoader() {}

bool DicomLoader::IsDicomFile(const QString& filePath)
{
    gdcm::Reader reader;
    reader.SetFileName(filePath.toStdString().c_str());
    return reader.Read();  // ✅ 파일이 DICOM인지 확인 (확장자와 무관)
}

void DicomLoader::OpenDicomDirectory(const QString& dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        qDebug() << "디렉토리가 존재하지 않습니다: " << dirPath;
        return;
    }

    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo& fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();

        // ✅ 확장자가 .dcm이거나, DICOM 파일인지 확인 후 추가
        if (fileInfo.suffix().toLower() == "dcm" || IsDicomFile(filePath)) {
            qDebug() << "DICOM 파일 추가: " << filePath;
            AddDicomFile(filePath);
        }
    }
}


void DicomLoader::AddDicomFile(const QString& filePath)
{
    // GDCM을 사용하여 DICOM 파일 읽기
    gdcm::Reader reader;
    reader.SetFileName(filePath.toStdString().c_str());
    if (!reader.Read()) return;

    // DICOM 파일에서 DataSet 추출
    gdcm::File& file = reader.GetFile();
    gdcm::DataSet& ds = file.GetDataSet();
    gdcm::StringFilter sf;
    sf.SetFile(file);

    // Group 분류를 위한 태그 정보 읽기 (Patient ID, Study ID, Series Num)
    gdcm::Tag tagPatientID(0x0010, 0x0020);
    gdcm::Tag tagStudyID(0x0020, 0x0010);
    gdcm::Tag tagSeriesNum(0x0020, 0x0011);

    QString patientID, studyID, seriesNum;
    if (ds.FindDataElement(tagPatientID))
        patientID = QString::fromStdString(sf.ToString(tagPatientID));
    if (ds.FindDataElement(tagStudyID))
        studyID = QString::fromStdString(sf.ToString(tagStudyID));
    if (ds.FindDataElement(tagSeriesNum))
        seriesNum = QString::fromStdString(sf.ToString(tagSeriesNum));

    // 같은 그룹이 있는지 확인
    vtkSmartPointer<DicomGroup> group;
    for (int i = 0; i < m_GroupList.size(); i++) {
        if (m_GroupList[i]->IsSameGroup(patientID.toStdString(), studyID.toStdString(), seriesNum.toStdString())) {
            group = m_GroupList[i];
            break;
        }
    }

    // 기존 그룹이 없다면 새 그룹 생성
    if (group == nullptr) {
        group = vtkSmartPointer<DicomGroup>::New();
        group->SetPatientID(patientID.toStdString());
        group->SetStudyID(studyID.toStdString());
        group->SetSeriesNum(seriesNum.toStdString());

        m_GroupList.push_back(group);
    }

    // 그룹에 파일 추가
    group->AddImageFile(filePath.toStdString().c_str());
}

vtkSmartPointer<DicomGroup> DicomLoader::GetDicomGroup(int idx)
{
    if (idx < 0 || idx >= static_cast<int>(m_GroupList.size())) return nullptr;
    return m_GroupList[idx];
}
