#include "MainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QComboBox>
#include "DVManager.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // 중앙 뷰 (ChildView)를 설정
    m_childView = new CChildView(this);
    setCentralWidget(m_childView);

    // 도킹식 DICOM 그룹 뷰 생성 및 왼쪽에 배치
    m_dicomGroupView = new DicomGroupView(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_dicomGroupView);

    // 메뉴, 툴바, 상태 표시줄 생성
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    // DICOM 그룹 창 토글 액션 (체크 가능한 액션)
    m_actionToggleDicomGroupView = new QAction(tr("DICOM 그룹"), this);
    m_actionToggleDicomGroupView->setCheckable(true);
    m_actionToggleDicomGroupView->setChecked(true);
    connect(m_actionToggleDicomGroupView, &QAction::triggered, this, &MainWindow::toggleDicomGroupView);

    // DICOM 폴더 열기 액션
    m_actionOpenDicomFolder = new QAction(tr("DICOM 폴더 열기"), this);
    connect(m_actionOpenDicomFolder, &QAction::triggered, this, &MainWindow::openDicomFolder);
}

void MainWindow::createMenus()
{
    // 보기 메뉴에 DICOM 그룹 토글 액션 추가
    QMenu* viewMenu = menuBar()->addMenu(tr("보기"));
    viewMenu->addAction(m_actionToggleDicomGroupView);

    // 파일 메뉴에 폴더 열기 액션 추가
    QMenu* fileMenu = menuBar()->addMenu(tr("파일"));
    fileMenu->addAction(m_actionOpenDicomFolder);
}

void MainWindow::createToolBar()
{
    QToolBar* toolBar = addToolBar(tr("파일"));
    toolBar->addAction(m_actionOpenDicomFolder);

    // 볼륨 렌더 모드 콤보박스 추가 (MFC의 콤보 박스와 유사)
    m_comboVolumeRenderMode = new QComboBox(this);
    m_comboVolumeRenderMode->addItem(tr("MIP"));
    m_comboVolumeRenderMode->addItem(tr("Skin"));
    m_comboVolumeRenderMode->addItem(tr("Bone"));
    connect(m_comboVolumeRenderMode, QOverload<int>::of(&QComboBox::activated),
        this, &MainWindow::changeVolumeRenderMode);
    toolBar->addWidget(m_comboVolumeRenderMode);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::toggleDicomGroupView()
{
    // 도킹창의 표시/숨김을 토글
    bool visible = m_actionToggleDicomGroupView->isChecked();
    m_dicomGroupView->setVisible(visible);
}

void MainWindow::openDicomFolder()
{
    // 폴더 선택 다이얼로그 표시
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("DICOM 폴더 선택"), QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folderPath.isEmpty())
    {
        // DVManager를 통해 DICOM 폴더 열기 및 DICOM 그룹 트리 업데이트
        DVManager::Mgr()->GetDicomLoader()->OpenDicomDirectory(folderPath);
        m_dicomGroupView->UpdateDicomTree();
    }
}

void MainWindow::changeVolumeRenderMode(int index)
{
    // DVManager를 통해 볼륨 렌더 모드 변경 (해당 함수는 DVManager에 구현되어야 함)
    DVManager::Mgr()->ChangeVolumeRenderMode(index);
}
