﻿#include "DVManager.h"
#include <windows.h>
#include "MainWindow.h"
#include "ChildView.h"
#include <QApplication>

// OpenGL 확장 함수 포인터 선언
typedef void (APIENTRY* PFNGLGENFRAMEBUFFERSPROC)(GLsizei, GLuint*);
typedef void (APIENTRY* PFNGLBINDFRAMEBUFFERPROC)(GLenum, GLuint);
typedef GLenum(APIENTRY* PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum);

// 함수 포인터 전역 변수
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;

// OpenGL 확장 함수 로드
void LoadOpenGLFunctions() {
    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");

    if (!glGenFramebuffers || !glBindFramebuffer || !glCheckFramebufferStatus) {
        qDebug() << "OpenGL 확장 함수 로드 실패!";
    }
}



DVManager::DVManager(QObject* parent)
    : QObject(parent)
{
}

DVManager::~DVManager()
{
}

DVManager* DVManager::Mgr()
{
    static DVManager instance;
    return &instance;
}

vtkSmartPointer<vtkGenericOpenGLRenderWindow> DVManager::GetVtkWindow(int viewType)
{
    if (viewType < 0 || viewType >= NUM_VIEW)
        return nullptr;

    return m_vtkWindow[viewType];
}


void DVManager::InitVtkWindow(int viewType, QVTKOpenGLNativeWidget* vtkWidget)
{
    if (viewType < 0 || viewType >= NUM_VIEW || !vtkWidget)
        return;

    if (!m_vtkWindow[viewType])
    {
        m_vtkWindow[viewType] = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    }

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(0.0, 0.0, 0.0);  // 배경색: 검정

    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

    if (viewType == VIEW_3D)
    {
        // 수정 전 vtkGenericOpenGLRenderWindow interactor 사용으로 QVTKOpenGLNativeWidget interactor과 충돌 방생하여 vtkOutputWindow 발생
        vtkWidget->interactor()->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
        vtkWidget->interactor()->SetRenderWindow(m_vtkWindow[viewType]);
        vtkWidget->interactor()->Initialize();

        camera->ParallelProjectionOff();
        camera->SetPosition(0.0, -1.0, 0.0);
        camera->SetViewUp(0.0, 0.0, 1.0);
    }
    else
    {
        // 수정 전 vtkGenericOpenGLRenderWindow interactor 사용으로 QVTKOpenGLNativeWidget interactor과 충돌 방생하여 vtkOutputWindow 발생
        vtkWidget->interactor()->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleImage>::New());
        vtkWidget->interactor()->SetRenderWindow(m_vtkWindow[viewType]);
        vtkWidget->interactor()->Initialize();

        qDebug() << "Interactor 설정됨: " << vtkWidget->interactor()->GetInteractorStyle()->GetClassName();

        
        camera->ParallelProjectionOn();
        camera->SetPosition(0.0, 0.0, -1.0);
        camera->SetViewUp(0.0, -1.0, 0.0);
    }

    renderer->SetActiveCamera(camera);
    
    m_vtkWindow[viewType]->AddRenderer(renderer);

    // 먼저, OpenGL 컨텍스트를 생성하기 위해 vtkWidget의 makeCurrent()를 호출합니다.
    vtkWidget->makeCurrent();

    // 컨텍스트가 생성된 후에 vtkGenericOpenGLRenderWindow를 QVTKOpenGLNativeWidget에 설정합니다.
    vtkWidget->setRenderWindow(m_vtkWindow[viewType]);
    //vtkWidget->interactor()->Initialize();


    // ✅ 변경 사항을 즉시 적용
    m_vtkWindow[viewType]->Render();


}


void DVManager::ResizeVtkWindow(int viewType, int width, int height)
{
    if (viewType < 0 || viewType >= NUM_VIEW)
        return;

    if (m_vtkWindow[viewType])
    {
        m_vtkWindow[viewType]->SetSize(width, height);
    }
}

// ✅ 추가: DICOM 로더 인스턴스 반환
vtkSmartPointer<DicomLoader> DVManager::GetDicomLoader()
{
    if (!m_DicomLoader) {
        m_DicomLoader = vtkSmartPointer<DicomLoader>::New();
    }

    return m_DicomLoader;
}

vtkSmartPointer<vtkRenderer> DVManager::GetRenderer(int viewType)
{
    if (viewType < 0 || viewType >= NUM_VIEW)
        return nullptr;
    if (m_vtkWindow[viewType] == nullptr)
        return nullptr;
    return m_vtkWindow[viewType]->GetRenderers()->GetFirstRenderer();
}

void DVManager::OnSelectDicomGroup(vtkSmartPointer<DicomGroup> group)
{
    // 현재 화면 초기화
    ClearVolumeDisplay();

    // 선택한 DICOM Group에서 Volume 데이터 로드
    GetDicomLoader()->LoadVolumeData(group);

    // 정보 표시 업데이트
    UpdateAnnotation();

    // Volume 데이터가 로드되었으면 렌더링 업데이트
    UpdateVolumeDisplay();

    // QT에서는 MainWindow의 중앙 위젯(ChildView)을 통해 각 2D 뷰의 스크롤바를 업데이트합니다.
    // MainWindow 클래스에 GetWndView() 함수가 구현되어 있다고 가정합니다.
    MainWindow* mainWin = qobject_cast<MainWindow*>(QApplication::activeWindow());
    if (!mainWin)
        return;
    CChildView* childView = mainWin->GetWndView();
    if (!childView)
        return;
    // VIEW_AXIAL부터 VIEW_SAGITTAL까지 각 뷰의 슬라이더 업데이트
    for (int viewType = VIEW_AXIAL; viewType <= VIEW_SAGITTAL; viewType++) {
        childView->GetDlgVtkView(viewType)->UpdateScrollBar();
    }
}

void DVManager::ClearVolumeDisplay()
{
    vtkSmartPointer<DicomLoader> loader = GetDicomLoader();
    vtkSmartPointer<VolumeData> volumeData = loader->GetVolumeData();
    if (!volumeData)
        return;

    // 3D 뷰에서 Volume 렌더링 제거
    vtkSmartPointer<vtkRenderer> renderer3D = GetRenderer(VIEW_3D);
    if (renderer3D)
        renderer3D->RemoveViewProp(volumeData->GetVolumeRendering());

    // 2D 슬라이스 뷰에서 각 슬라이스 Actor 제거
    for (int viewType = VIEW_AXIAL; viewType <= VIEW_SAGITTAL; viewType++) {
        vtkSmartPointer<vtkRenderer> renderer = GetRenderer(viewType);
        if (renderer)
            renderer->RemoveActor(volumeData->GetSliceActor(viewType));
    }
}

void DVManager::UpdateVolumeDisplay()
{
    vtkSmartPointer<DicomLoader> loader = GetDicomLoader();
    vtkSmartPointer<VolumeData> volumeData = loader->GetVolumeData();
    if (!volumeData)
    {
        qDebug() << "❌ Volume 데이터가 없습니다.";
        return;
    }
        
    qDebug() << "✅ Volume 데이터 렌더링 시작";

    // 3D 뷰에 Volume 렌더링 추가 및 카메라 재설정
    vtkSmartPointer<vtkRenderer> renderer3D = GetRenderer(VIEW_3D);
    if (renderer3D) {
        renderer3D->AddViewProp(volumeData->GetVolumeRendering());
        renderer3D->ResetCamera();
        m_vtkWindow[VIEW_3D]->Render();
    }
    else {
        qDebug() << "❌ 3D 렌더러 없음";
    }

    // 2D 슬라이스 뷰에 각 슬라이스 Actor 추가 및 카메라 재설정
    for (int viewType = VIEW_AXIAL; viewType <= VIEW_SAGITTAL; viewType++) {
        vtkSmartPointer<vtkRenderer> renderer = GetRenderer(viewType);
        if (renderer) {
            renderer->AddActor(volumeData->GetSliceActor(viewType));
            renderer->ResetCamera();
            m_vtkWindow[viewType]->Render();
        }
    }
}

void DVManager::ScrollSliceIndex(int viewType, int pos)
{
    vtkSmartPointer<VolumeData> volumeData = GetDicomLoader()->GetVolumeData();
    if (!volumeData)
        return;

    // Volume 데이터의 슬라이스 인덱스 설정
    volumeData->SetSliceIndex(viewType, pos);

    // 정보 표시 업데이트
    UpdateAnnotation();

    // 해당 VTK 윈도우를 다시 렌더링
    m_vtkWindow[viewType]->Render();
}

void DVManager::ChangeVolumeRenderMode(int renderMode)
{
    vtkSmartPointer<VolumeData> volumeData = GetDicomLoader()->GetVolumeData();
    if (!volumeData)
        return;

    // Volume 데이터의 렌더링 모드 변경 (MIP, SKIN, BONE 등)
    volumeData->SetCurrentPresetMode(renderMode);

    // 3D 뷰 다시 렌더링
    m_vtkWindow[VIEW_3D]->Render();
}

void DVManager::UpdateAnnotation()
{
    // 정보 표시 객체 생성 및 업데이트 (예: vtkCornerAnnotation 활용)
    for (int viewType = 0; viewType < NUM_VIEW; viewType++) {
        // 예시: m_Annotation[viewType]이 nullptr이면 생성
        if (!m_Annotation[viewType]) {
            m_Annotation[viewType] = vtkSmartPointer<vtkCornerAnnotation>::New();
            m_Annotation[viewType]->SetMaximumFontSize(20);
            GetRenderer(viewType)->AddViewProp(m_Annotation[viewType]);
        }
        // 2D 슬라이스 정보 업데이트
        UpdateSliceAnnotation(viewType);
    }
    // 3D 정보는 간단히 "3D"로 표시 (예시)
    m_Annotation[VIEW_3D]->SetText(2, "3D");
}

void DVManager::UpdateSliceAnnotation(int viewType)
{
    if (viewType != VIEW_AXIAL && viewType != VIEW_CORONAL && viewType != VIEW_SAGITTAL)
        return;
    vtkSmartPointer<VolumeData> volumeData = GetDicomLoader()->GetVolumeData();
    if (!volumeData)
        return;
    int ext[6];
    volumeData->GetImageData()->GetExtent(ext);

    std::string leftTopText;
    switch (viewType) {
    case VIEW_AXIAL:
        leftTopText = "Axial\n" + std::to_string(volumeData->GetSliceIndex(viewType)) + " / " + std::to_string(ext[5]);
        break;
    case VIEW_CORONAL:
        leftTopText = "Coronal\n" + std::to_string(volumeData->GetSliceIndex(viewType)) + " / " + std::to_string(ext[3]);
        break;
    case VIEW_SAGITTAL:
        leftTopText = "Sagittal\n" + std::to_string(volumeData->GetSliceIndex(viewType)) + " / " + std::to_string(ext[1]);
        break;
    }
    // 예시로 왼쪽 상단 코너(인덱스 2)에 표시
    m_Annotation[viewType]->SetText(2, leftTopText.c_str());
}