#include "DicomLoader.h"

#include <gdcmReader.h>
#include <gdcmFile.h>
#include <gdcmDataSet.h>
#include <gdcmStringFilter.h>
#include <gdcmTag.h>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include <vtkGDCMImageReader.h>
#include <vtkStringArray.h>
#include <gdcmIPPSorter.h>
#include "DVManager.h"
#include "DicomGroup.h"

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

void DicomLoader::LoadVolumeData(vtkSmartPointer<DicomGroup> dicomGroup)
{
    // DICOM 그룹 검사
    if (dicomGroup == NULL) return;

    qDebug() << "📂 DICOM 그룹 선택됨: " << QString::fromStdString(dicomGroup->GetPatientID());

    // 현재 선택된 그룹 저장
    m_CurrentGroup = dicomGroup;

    // 선택된 그룹의 파일 개수 검사
    int numFiles = (int)dicomGroup->GetFileList().size();
    if (numFiles == 0) {
        qDebug() << "❌ 파일이 없습니다.";
        return;
    }
    
    qDebug() << "📂 파일 개수: " << numFiles;

    // DICOM 파일 정렬 (Image Position (Patient) 태그 기준)
    gdcm::IPPSorter ippSorter;
    bool bSortSuccess = ippSorter.Sort(dicomGroup->GetFileList());

    // 정렬이 성공하면 정렬된 파일 목록을 실패하면 기존 파일 목록을 로드
    std::vector<std::string> sortedFileNames;
    if (bSortSuccess) sortedFileNames = ippSorter.GetFilenames();
    else sortedFileNames = dicomGroup->GetFileList();

    // vtkStringArray 타입으로 변환
    vtkSmartPointer<vtkStringArray> fileArray = vtkSmartPointer<vtkStringArray>::New();
    for (int i = 0; i < (int)sortedFileNames.size(); i++)
        fileArray->InsertNextValue(sortedFileNames[i].c_str());

    // GDCM Image Reader를 이용하여 DICOM 이미지 로딩
    vtkSmartPointer<vtkGDCMImageReader> dcmReader =
        vtkSmartPointer<vtkGDCMImageReader>::New();
    // 이미지를 아래에서 위로 읽음
    dcmReader->FileLowerLeftOn();
    // 파일 목록이 1개 이상
    if (numFiles > 1) dcmReader->SetFileNames(fileArray);
    // 파일 목록이 1개 (Mosaic Image일 가능성 있음)
    else dcmReader->SetFileName(fileArray->GetValue(0).c_str());
    // 이미지 로더 업데이트
    dcmReader->Update();

    // Volume Data 새로 생성
    m_VolumeData = vtkSmartPointer<VolumeData>::New();
    m_VolumeData->SetImageData(dcmReader->GetOutput());
    m_VolumeData->SetOrientation(dcmReader->GetDirectionCosines());

    // IPP 정렬 성공 시, IPPSorter에서 계산된 z-spacing으로 업데이트
    if (bSortSuccess) {
        double spacing[3];
        m_VolumeData->GetImageData()->GetSpacing(spacing);
        spacing[2] = ippSorter.GetZSpacing();
        m_VolumeData->GetImageData()->SetSpacing(spacing);
    }

    // Volume 렌더링 준비
    m_VolumeData->ReadyForVolumeRendering();

    // Slice 렌더링 준비
    m_VolumeData->ReadyForSliceRendering();
}