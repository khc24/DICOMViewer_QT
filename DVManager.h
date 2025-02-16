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
   

private:
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_vtkWindow[NUM_VIEW];
};
