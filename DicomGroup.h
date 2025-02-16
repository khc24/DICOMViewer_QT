#pragma once

#include <QObject>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vector>
#include <string>

class DicomGroup : public vtkObject
{
	//Q_OBJECT  // Qt의 신호/슬롯을 사용하기 위해 추가

public:
	vtkTypeMacro(DicomGroup, vtkObject);
	static DicomGroup* New() { return new DicomGroup; }

protected:
	// 환자 정보
	std::string PatientID;
	std::string PatientName;
	std::string PatientBirthDate;
	std::string PatientSex;
	std::string PatientAge;
	std::string PatientWeight;

	// Study 정보
	std::string StudyID;
	std::string StudyDescription;

	// Series 정보
	std::string SeriesNum;
	std::string SeriesDescription;

	std::vector<std::string> m_FileList;

public:
	// Getter / Setter 메서드
	vtkGetMacro(PatientID, std::string);
	vtkSetMacro(PatientID, std::string);

	vtkGetMacro(StudyID, std::string);
	vtkSetMacro(StudyID, std::string);

	vtkGetMacro(SeriesNum, std::string);
	vtkSetMacro(SeriesNum, std::string);

	vtkGetMacro(PatientName, std::string);
	vtkGetMacro(PatientBirthDate, std::string);
	vtkGetMacro(PatientSex, std::string);
	vtkGetMacro(PatientAge, std::string);
	vtkGetMacro(PatientWeight, std::string);
	vtkGetMacro(StudyDescription, std::string);
	vtkGetMacro(SeriesDescription, std::string);

	// DICOM 파일 리스트 반환
	std::vector<std::string> GetFileList() const { return m_FileList; }

	/// PatientID, StudyID, SeriesNum을 비교하여 동일한 그룹인지 확인
	bool IsSameGroup(std::string patientID, std::string studyID, std::string seriesNum) {
		if (PatientID.compare(patientID) != 0) return false;
		if (StudyID.compare(studyID) != 0) return false;
		if (SeriesNum.compare(seriesNum) != 0) return false;

		return true;
	}

	/// 이 그룹에 DICOM 파일 추가
	void AddImageFile(const char* filePath);

	/// 추가 DICOM 태그 정보 읽기
	void LoadDicomInfo();
};
