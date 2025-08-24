#include "capturethread.h"

#include "utilities/utilities.h"

#include <QDebug>
#include <QTime>
#include <QFile>

CaptureThread::CaptureThread(int camera, QMutex *lock):
    running(false), cameraID(camera), videoPath(""), data_lock(lock), fps_calculating(false), fps(0.0f)
{
    frame_width = frame_height = 0;
    video_saving_status = STOPPED;
    saved_video_name = "";
    video_writer = nullptr;
}

CaptureThread::CaptureThread(QString videoPath, QMutex *lock):
    running(false), cameraID(-1), videoPath(videoPath), data_lock(lock), fps_calculating(false), fps(0.0f)
{
    frame_width = frame_height = 0;
    video_saving_status = STOPPED;
    saved_video_name = "";
    video_writer = nullptr;
}

CaptureThread::~CaptureThread() {
}

void CaptureThread::run() {
    running = true;
    cv::VideoCapture cap(cameraID);
    cv::Mat tmp_frame;

    QTime timer;
    timer.start();
    frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    while(running) {
        cap >> tmp_frame;
        if (tmp_frame.empty()) {
            break;
        }

        switch(video_saving_status)
        {
        case STARTING:
            startSavingVideo(tmp_frame);
            break;
        case STARTED:
            video_writer->write(tmp_frame);
            break;
        case STOPPING:
            stopSavingVideo();
            break;
        default:
            break;
        }

        cv::cvtColor(tmp_frame, tmp_frame, cv::COLOR_BGR2RGB);

        data_lock->lock();
        frame = tmp_frame;
        data_lock->unlock();

        // 发送刚刚捕获帧
        emit frameCaptured(&frame);

        if (fps_calculating)
            calculateFPS(cap);
    }

    cap.release();
    running = false;
    quit();
}

void CaptureThread::calculateFPS(cv::VideoCapture &cap)
{
    int count_to_read = 100;
    cv::Mat tmp_frame;
    QTime timer;
    timer.start();
    for (int i = 0; i < count_to_read; i++)
    {
        cap >> tmp_frame;
    }

    int elapsed_ms = timer.elapsed();
    fps = count_to_read / (elapsed_ms / 1000.0);
    fps_calculating = false;
    emit fpsChanged(fps);
}

void CaptureThread::startSavingVideo(cv::Mat &firstFrame)
{
    saved_video_name = Utilities::newSavedVideoName();

    QString cover = Utilities::getSavedVideoPath(saved_video_name, "jpg");
    cv::imwrite(cover.toStdString(), firstFrame);

    video_writer = new cv::VideoWriter(
                Utilities::getSavedVideoPath(saved_video_name, "avi").toStdString(),
                cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                fps ? fps : 30,
                cv::Size(frame_width, frame_height));

    video_saving_status = STARTED;
}

void CaptureThread::stopSavingVideo()
{
    video_saving_status = STOPPED;

    video_writer->release();
    delete video_writer;
    video_writer = nullptr;

    emit videoSaved(saved_video_name);
}
