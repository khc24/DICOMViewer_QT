#pragma once

// Qt6 �⺻ ���
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStatusBar>
#include <QPushButton>
#include <QLabel>
#include <QScrollBar>

// VTK ���� ���
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>

// GDCM ���� ���
#include <gdcmImageReader.h>
#include <gdcmImageWriter.h>

// VTK ��� �ʱ�ȭ
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingContextOpenGL2);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
//VTK_MODULE_INIT(vtkRenderingOpenGL3);