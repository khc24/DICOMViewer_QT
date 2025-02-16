#pragma once

#include <QObject>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vector>
#include <string>

class DicomGroup : public vtkObject
{
	//Q_OBJECT  // Qt�� ��ȣ/������ ����ϱ� ���� �߰�

public:
	vtkTypeMacro(DicomGroup, vtkObject);
	static DicomGroup* New() { return new DicomGroup; }

protected:
	// ȯ�� ����
	std::string PatientID;
	std::string PatientName;
	std::string PatientBirthDate;
	std::string PatientSex;
	std::string PatientAge;
	std::string PatientWeight;

	// Study ����
	std::string StudyID;
	std::string StudyDescription;

	// Series ����
	std::string SeriesNum;
	std::string SeriesDescription;

	std::vector<std::string> m_FileList;

public:
	// Getter / Setter �޼���
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

	// DICOM ���� ����Ʈ ��ȯ
	std::vector<std::string> GetFileList() const { return m_FileList; }

	/// PatientID, StudyID, SeriesNum�� ���Ͽ� ������ �׷����� Ȯ��
	bool IsSameGroup(std::string patientID, std::string studyID, std::string seriesNum) {
		if (PatientID.compare(patientID) != 0) return false;
		if (StudyID.compare(studyID) != 0) return false;
		if (SeriesNum.compare(seriesNum) != 0) return false;

		return true;
	}

	/// �� �׷쿡 DICOM ���� �߰�
	void AddImageFile(const char* filePath);

	/// �߰� DICOM �±� ���� �б�
	void LoadDicomInfo();
};
