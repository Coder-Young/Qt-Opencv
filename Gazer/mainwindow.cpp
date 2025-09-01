#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utilities/utilities.h"

#include <QWidget>
#include <QCameraInfo>
#include <QMessageBox>
#include <QImage>
#include <QCameraViewfinder>
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

    connect(ui->actioncalcFPS, SIGNAL(triggered(bool)),
            this, SLOT(calculateFPS()));

    connect(recordBtn, SIGNAL(clicked(bool)),
            this, SLOT(recordingStartStop()));

    connect(monitorCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(updateMonitorStatus(int)));
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

    monitorCheckBox = new QCheckBox(this);
    monitorCheckBox->setText("Monitor On/Off");
    tool_layout->addWidget(monitorCheckBox, 0, 0);

    recordBtn = new QPushButton(this);
    recordBtn->setText("Record");
    tool_layout->addWidget(recordBtn, 0, 1, Qt::AlignHCenter);
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
    mainStatusLabel->setText("Gazer is Ready");
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
        disconnect(capturer, &CaptureThread::fpsChanged,
                   this, &MainWindow::updateFPS);
        disconnect(capturer, &CaptureThread::videoSaved,
                   this, &MainWindow::appendSavedVideo);
        //connect(capturer, SIGNAL(finished()),
         //       capturer, &CaptureThread::deleter);
    }

    int camID = 0;

    // 创建捕获线程
    capturer = new CaptureThread(camID, data_lock);
    connect(capturer, &CaptureThread::frameCaptured,
            this, &MainWindow::updateFrame);
    connect(capturer, &CaptureThread::fpsChanged,
            this, &MainWindow::updateFPS);
    connect(capturer, &CaptureThread::videoSaved,
            this, &MainWindow::appendSavedVideo);
    capturer->start();

    monitorCheckBox->setCheckState(Qt::Unchecked);
    recordBtn->setEnabled(true);
    this->mainStatusLabel->setText(QString("Capturing Camera %1").arg(camID));
}

void MainWindow::calculateFPS()
{
    if (capturer != nullptr)
    {
        capturer->startCalcFPS(true);
    }
}

void MainWindow::updateFPS(float fps)
{
    this->mainStatusLabel->setText(QString("FPS %1").arg(fps));
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

void MainWindow::recordingStartStop()
{
    QString text = recordBtn->text();
    if (text == "Record" && capturer != nullptr)
    {
        capturer->setVideoSavingStatus(CaptureThread::STARTING);
        recordBtn->setText("Stop Recording");
    }
    else if (text == "Stop Recording" && capturer != nullptr)
    {
        capturer->setVideoSavingStatus(CaptureThread::STOPPING);
        recordBtn->setText("Record");
    }
}

void MainWindow::appendSavedVideo(QString name)
{
    QString coverPath = Utilities::getSavedVideoPath(name, "jpg");
    QPixmap pixmap;
    bool loaded = pixmap.load(coverPath);
    if (!loaded)
    {
        qDebug() << "video cover lost";
        return;
    }

    QStandardItem *item = new QStandardItem();
    item->setEditable(false);
    list_model->appendRow(item);

    QModelIndex index = list_model->indexFromItem(item);
    list_model->setData(index, QPixmap(coverPath).scaledToHeight(145), Qt::DecorationRole);
    list_model->setData(index, name, Qt::DisplayRole);
    saved_list->scrollTo(index);
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
        appendSavedVideo(cover.baseName());
    }
}

void MainWindow::updateMonitorStatus(int status)
{
    if(capturer == nullptr) {
        return;
    }

    if(status) {
        capturer->setMotionDetectingStatus(true);
        recordBtn->setEnabled(false);
    } else {
        capturer->setMotionDetectingStatus(false);
        recordBtn->setEnabled(true);
    }
}
