#pragma once

#include <QObject>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRendererCollection.h>
#include <vtkCornerAnnotation.h>
#include <vtkGenericOpenGLRenderWindow.h> 

#include "DicomLoader.h"

class DVManager : public QObject
{
    Q_OBJECT

private:
    explicit DVManager(QObject* parent = nullptr);
    ~DVManager();

public:
    static DVManager* Mgr();

    static const int NUM_VIEW = 4;
    enum ViewType { VIEW_AXIAL, VIEW_CORONAL, VIEW_SAGITTAL, VIEW_3D };

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> GetVtkWindow(int viewType);
    void InitVtkWindow(int viewType, QVTKOpenGLNativeWidget* vtkWidget);
    void ResizeVtkWindow(int viewType, int width, int height);

    vtkSmartPointer<DicomLoader> GetDicomLoader();

   /// View 타입에 따른 첫 번째 Renderer 반환
    vtkSmartPointer<vtkRenderer> GetRenderer(int viewType);

    /// DICOM Group 선택 시, Volume 데이터 로드 및 렌더링 업데이트
    void OnSelectDicomGroup(vtkSmartPointer<DicomGroup> group);

    /// 현재 그려진 슬라이스와 Volume 제거
    void ClearVolumeDisplay();

    /// Volume 데이터 렌더링 업데이트 (3D와 2D 슬라이스 각각)
    void UpdateVolumeDisplay();

    void ScrollSliceIndex(int viewType, int pos);

    // 새로 추가: Volume 렌더링 모드 변경 함수 선언
    void ChangeVolumeRenderMode(int renderMode);

    // 정보 표시 업데이트 관련 함수들 (UpdateAnnotation, UpdateSliceAnnotation)
    void UpdateAnnotation();
    void UpdateSliceAnnotation(int viewType);
   

private:
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_vtkWindow[NUM_VIEW];
    vtkSmartPointer<DicomLoader> m_DicomLoader;
    /// 정보 표시
    vtkSmartPointer<vtkCornerAnnotation> m_Annotation[NUM_VIEW];

};
