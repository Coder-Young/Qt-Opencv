#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QCheckBox>
#include <QPushButton>
#include <QListView>
#include <QLabel>
#include <QMutex>
#include <QCamera>
#include <QCameraViewfinderSettings>
#include <QListView>
#include <QStandardItemModel>
#include "opencv2/opencv.hpp"
#include "capturethread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QCameraViewfinder;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initUI();
    void populateSavedList();

private slots:
    void showCameraInfo();
    void openCamera();
    void updateFrame(cv::Mat*);
    void appendSavedPhoto(QString name);

private:
    Ui::MainWindow *ui;

private:
    QMenu *fileMenu;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QPushButton *shutterBtn;

    QListView *saved_list;

    QLabel *mainStatusLabel;

    cv::Mat currentFrame;

    QMutex *data_lock;
    CaptureThread *capturer;

    QStandardItemModel *list_model;
};
#endif // MAINWINDOW_H
