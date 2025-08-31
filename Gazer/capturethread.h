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
    enum VideoSavingStatus
    {
        STARTING,
        STARTED,
        STOPPING,
        STOPPED
    };

public:
    CaptureThread(int camera, QMutex *lock);
    CaptureThread(QString videoPath, QMutex *lock);
    ~CaptureThread();

    void setRunning(bool run) { running = run; }
    void startCalcFPS(bool calculate) { fps_calculating = calculate; }
    void setVideoSavingStatus(VideoSavingStatus status) {video_saving_status = status; }
    void setMotionDetectingStatus(bool status)
    {
        motion_detecting_status = status;
        motion_detected = false;
        if (video_saving_status != STOPPED)
            video_saving_status = STOPPING;
    }

private:
    void calculateFPS(cv::VideoCapture &cap);
    void startSavingVideo(cv::Mat &firstFrame);
    void stopSavingVideo();
    void motionDetect(cv::Mat &frame);

protected:
    void run() override;

signals:
    void frameCaptured(cv::Mat *data);
    void fpsChanged(float fps);
    void videoSaved(QString name);

private: //播放视频
    bool running;
    int cameraID;
    QString videoPath;
    QMutex *data_lock;
    cv::Mat frame;

private: //计算fps
    bool fps_calculating;
    float fps;

private: //保存视频
    int frame_width, frame_height;
    VideoSavingStatus video_saving_status;
    QString saved_video_name;
    cv::VideoWriter *video_writer;

private: //运动检测
    bool motion_detecting_status;
    bool motion_detected;
    cv::Ptr<cv::BackgroundSubtractorMOG2> segmentor;
};
#endif // CAPTURETHREAD_H
