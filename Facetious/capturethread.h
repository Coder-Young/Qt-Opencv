#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QString>
#include <QThread>
#include <QMutex>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/face/facemark.hpp"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(int camera, QMutex *lock);
    CaptureThread(QString videoPath, QMutex *lock);
    ~CaptureThread();

    void setRunning(bool run) { running = run; }
    void takePhoto() { taking_photo = true; }

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);
    void photoTaken(QString name);

private:
    void takePhoto(cv::Mat &frame);
    void detectFaces(cv::Mat &frame);
    void loadOrnaments();
    void drawGlasses(cv::Mat &frame, std::vector<cv::Point2f> &marks);

private: //播放视频
    bool running;
    int cameraID;
    QString videoPath;
    QMutex *data_lock;
    cv::Mat frame;

    int frame_width, frame_height;

private:
    bool taking_photo;

private:
    cv::CascadeClassifier *classifier;
    cv::Ptr<cv::face::Facemark> mark_detector;

private:
    cv::Mat glasses;
    cv::Mat mustache;
    cv::Mat mouse_nose;
};
#endif // CAPTURETHREAD_H
