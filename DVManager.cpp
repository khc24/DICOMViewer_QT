#include "DVManager.h"
#include <windows.h>


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
        camera->ParallelProjectionOff();
        camera->SetPosition(0.0, -1.0, 0.0);
        camera->SetViewUp(0.0, 0.0, 1.0);
    }
    else
    {
        // 수정 전 vtkGenericOpenGLRenderWindow interactor 사용으로 QVTKOpenGLNativeWidget interactor과 충돌 방생하여 vtkOutputWindow 발생
        vtkWidget->interactor()->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleImage>::New());
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
