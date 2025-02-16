﻿#include "DicomGroup.h"

#include <gdcmReader.h>
#include <gdcmFile.h>
#include <gdcmDataSet.h>
#include <gdcmStringFilter.h>
#include <gdcmTag.h>

//DicomGroup::DicomGroup() {}
//
//DicomGroup::~DicomGroup() {}

void DicomGroup::AddImageFile(const char* filePath)
{
    if (filePath == nullptr) return;  // ✅ NULL → nullptr 변환
    m_FileList.push_back(std::string(filePath));  // ✅ std::string 사용
}

void DicomGroup::LoadDicomInfo()
{
    if (m_FileList.empty()) return;  // ✅ size() == 0 → empty() 사용

    // GDCM을 이용한 DICOM 파일 읽기
    gdcm::Reader reader;
    reader.SetFileName(m_FileList[0].c_str());
    if (!reader.Read()) return;

    // DICOM 파일에서 DataSet 추출
    gdcm::File& file = reader.GetFile();
    gdcm::DataSet& ds = file.GetDataSet();
    gdcm::StringFilter sf;
    sf.SetFile(file);

    // DICOM 태그 정의
    gdcm::Tag tagPatientName(0x0010, 0x0010);
    gdcm::Tag tagPatientBirthDate(0x0010, 0x0030);
    gdcm::Tag tagPatientSex(0x0010, 0x0040);
    gdcm::Tag tagPatientAge(0x0010, 0x1010);
    gdcm::Tag tagPatientWeight(0x0010, 0x1030);
    gdcm::Tag tagStudyDescription(0x0008, 0x1030);
    gdcm::Tag tagSeriesDescription(0x0008, 0x103e);

    // ✅ 태그 값이 존재할 경우 데이터 저장
    if (ds.FindDataElement(tagPatientName))
        PatientName = sf.ToString(tagPatientName);
    if (ds.FindDataElement(tagPatientBirthDate))
        PatientBirthDate = sf.ToString(tagPatientBirthDate);
    if (ds.FindDataElement(tagPatientSex))
        PatientSex = sf.ToString(tagPatientSex);
    if (ds.FindDataElement(tagPatientAge))
        PatientAge = sf.ToString(tagPatientAge);
    if (ds.FindDataElement(tagPatientWeight))
        PatientWeight = sf.ToString(tagPatientWeight);
    if (ds.FindDataElement(tagStudyDescription))
        StudyDescription = sf.ToString(tagStudyDescription);
    if (ds.FindDataElement(tagSeriesDescription))
        SeriesDescription = sf.ToString(tagSeriesDescription);
}
