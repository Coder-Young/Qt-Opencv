#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QString>
#include <QThread>
#include <QMutex>

#include "opencv2/opencv.hpp"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    CaptureThread(QString videoPath, QMutex *lock);
    ~CaptureThread();

    void setRunning(bool run) { running = run; }

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);

private: //播放视频
    bool running;
    int cameraID;
    QString videoPath;
    QMutex *data_lock;
    cv::Mat frame;

    int frame_width, frame_height;
};
#endif // CAPTURETHREAD_H
