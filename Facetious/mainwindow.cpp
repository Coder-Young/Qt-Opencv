#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utilities/utilities.h"

#include <QWidget>
#include <QCameraInfo>
#include <QMessageBox>
#include <QImage>
#include <QApplication>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initUI();
    populateSavedList();

    data_lock = new QMutex();
    capturer = nullptr;

    connect(ui->actionCamera_Infomation, SIGNAL(triggered(bool)),
            this, SLOT(showCameraInfo()));

    connect(ui->actionOpen_Camera, SIGNAL(triggered(bool)),
            this, SLOT(openCamera()));
}

MainWindow::~MainWindow()
{
    delete ui;

    if (capturer != nullptr)
        delete capturer;
}

void MainWindow::initUI()
{
    this->resize(1000, 800);

    QGridLayout *main_layout = new QGridLayout();
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    main_layout->addWidget(imageView, 0, 0, 12, 1);

    QGridLayout *tool_layout = new QGridLayout();
    main_layout->addLayout(tool_layout, 12, 0, 1, 1);

    shutterBtn = new QPushButton(this);
    shutterBtn->setText("Take photo");
    tool_layout->addWidget(shutterBtn, 0, 0, Qt::AlignHCenter);
    tool_layout->addWidget(new QLabel(this), 0, 2);

    // list of saved videos
    saved_list = new QListView(this);
    saved_list->setViewMode(QListView::IconMode);
    saved_list->setResizeMode(QListView::Adjust);
    saved_list->setSpacing(5);
    saved_list->setWrapping(false);
    list_model = new QStandardItemModel(this);
    saved_list->setModel(list_model);
    main_layout->addWidget(saved_list, 13, 0, 4, 1);

    QWidget *widget = new QWidget();
    widget->setLayout(main_layout);
    setCentralWidget(widget);

    mainStatusLabel = new QLabel();
    mainStatusLabel->setText("Facetious is Ready");
    //ui->statusBar->addWidget(mainStatusLabel);
    ui->statusBar->addPermanentWidget(mainStatusLabel);
}

void MainWindow::showCameraInfo()
{
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    QString info = QString("Available Cameras: \n");

    foreach (const QCameraInfo &cameraInfo, cameras) {
        info += " - " + cameraInfo.deviceName() + ": ";
        info += cameraInfo.description() + "\n";
    }
    QMessageBox::information(this, "Cameras", info);
}

void MainWindow::openCamera()
{
    if (capturer != nullptr)
    {
        capturer->setRunning(false);
        disconnect(capturer, &CaptureThread::frameCaptured,
                   this, &MainWindow::updateFrame);
        //connect(capturer, SIGNAL(finished()),
         //       capturer, &CaptureThread::deleter);
    }

    int camID = 0;

    // 创建捕获线程
    capturer = new CaptureThread(camID, data_lock);
    connect(capturer, &CaptureThread::frameCaptured,
            this, &MainWindow::updateFrame);
    capturer->start();
    this->mainStatusLabel->setText(QString("Capturing Camera %1").arg(camID));
}

void MainWindow::updateFrame(cv::Mat *mat)
{
    data_lock->lock();
    currentFrame = *mat;
    data_lock->unlock();

    QImage frame(
        currentFrame.data,
        currentFrame.cols,
        currentFrame.rows,
        currentFrame.step,
        QImage::Format_RGB888);

    QPixmap image = QPixmap::fromImage(frame);

    imageScene->clear();
    imageView->resetMatrix();
    imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());
}

void MainWindow::populateSavedList()
{
    QString path = Utilities::getDataPath();

    QDir videoDir(path);
    QStringList nameFilters;
    nameFilters << "*.jpg";

    QFileInfoList videoCovers = videoDir.entryInfoList(
                nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

    foreach(QFileInfo cover, videoCovers)
    {

    }
}

void MainWindow::appendSavedPhoto(QString name)
{

}
