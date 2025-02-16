#pragma once

#include <QMainWindow>
#include "ChildView.h"
#include "DicomGroupView.h"
#include <QComboBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();

private slots:
    void toggleDicomGroupView();
    void openDicomFolder();
    void changeVolumeRenderMode(int index);

private:
    CChildView* m_childView;
    DicomGroupView* m_dicomGroupView;

    QAction* m_actionToggleDicomGroupView;
    QAction* m_actionOpenDicomFolder;
    QComboBox* m_comboVolumeRenderMode;
};
