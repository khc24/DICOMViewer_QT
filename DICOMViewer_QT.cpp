// DICOMViewer_QT.cpp : 애플리케이션의 진입점을 정의합니다.
//

#include "pch.h"
#include "DICOMViewer_QT.h"
#include <QApplication>
#include "ChildView.h" 
#include "MainWindow.h"
#include <vtkSmartPointer.h>
#include <vtkOutputWindow.h>
#include <vtkFileOutputWindow.h>
#include <vtkObjectFactory.h>
using namespace std;

void DisableVTKOutput()
{
    vtkSmartPointer<vtkFileOutputWindow> fileOutput = vtkSmartPointer<vtkFileOutputWindow>::New();
    fileOutput->SetFileName("vtk_output_log.txt");  // 로그 파일로 출력 (또는 무시 가능)
    vtkOutputWindow::SetInstance(fileOutput);
}

void CheckVTKModules()
{
    vtkObject* obj = vtkObjectFactory::CreateInstance("vtkSmartVolumeMapper");
    if (obj)
    {
        qDebug() << "✅ RenderingVolumeOpenGL2 모듈이 정상적으로 로드되었습니다.";
        obj->Delete();
    }
    else
    {
        qDebug() << "❌ RenderingVolumeOpenGL2 모듈이 누락되었습니다! CMake 설정을 확인하세요.";
    }
}


int main(int argc, char* argv[])
{
    // 최신 OpenGL (예: 4.6 Core Profile) 사용
    /*QSurfaceFormat format;
    format.setVersion(4, 6);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);*/



    QApplication app(argc, argv);

    CheckVTKModules();
    // DisableVTKOutput();  // 필요시 VTK 로그를 파일로 출력

    MainWindow window;
    window.resize(800, 600);
    window.show();

    return app.exec();
}
