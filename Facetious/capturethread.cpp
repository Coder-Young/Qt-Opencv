#include "capturethread.h"

#include "utilities/utilities.h"

#include <QDebug>
#include <QTime>
#include <QFile>
#include <vector>
#include <QApplication>

CaptureThread::CaptureThread(int camera, QMutex *lock):
    running(false), cameraID(camera), videoPath(""), data_lock(lock), taking_photo(false)
{
    frame_width = frame_height = 0;
}

CaptureThread::CaptureThread(QString videoPath, QMutex *lock):
    running(false), cameraID(-1), videoPath(videoPath), data_lock(lock), taking_photo(false)
{
    frame_width = frame_height = 0;
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

    classifier = new cv::CascadeClassifier(OPENCV_DATA_DIR "haarcascades/haarcascade_frontalface_alt.xml");
    mark_detector = cv::face::createFacemarkLBF();
    QString model_data = QApplication::instance()->applicationDirPath() + "/data/lbfmodel.yaml";
    mark_detector->loadModel(model_data.toStdString());
    while(running) {
        cap >> tmp_frame;
        if (tmp_frame.empty()) {
            break;
        }

        detectFaces(tmp_frame);

        cv::cvtColor(tmp_frame, tmp_frame, cv::COLOR_BGR2RGB);

        if (taking_photo)
        {
            takePhoto(tmp_frame);
        }

        data_lock->lock();
        frame = tmp_frame;
        data_lock->unlock();

        // 发送刚刚捕获帧
        emit frameCaptured(&frame);
    }

    cap.release();
    delete classifier;
    classifier = nullptr;
    running = false;
    quit();
}

void CaptureThread::takePhoto(cv::Mat &frame)
{
    if (frame.empty())
        return;

    QString photoName = Utilities::newSavedPhotoName();
    QString photoPath = Utilities::getSavedPhotoPath(photoName, "jpg");
    bool test = cv::imwrite(photoPath.toStdString(), frame);
    emit photoTaken(photoName);
    taking_photo = false;
}

void CaptureThread::detectFaces(cv::Mat &frame)
{
    std::vector<cv::Rect> faces;
    cv::Mat gray_frame;
    cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
    classifier->detectMultiScale(gray_frame, faces, 1.3, 5);

    cv::Scalar color = cv::Scalar(0, 0, 255);
    for (size_t i = 0; i < faces.size(); i++)
    {
        cv::rectangle(frame, faces[i], color, 1);
    }

    std::vector< std::vector<cv::Point2f> > shapes;
    if (mark_detector->fit(frame, faces, shapes))
    {
        for (unsigned long i=0; i<faces.size(); i++)
        {
            for(unsigned long k=0; k<shapes[i].size(); k++)
            {
                //cv::circle(frame, shapes[i][k], 2, color, cv::FILLED);
                QString index = QString("%1").arg(k);
                cv::putText(frame, index.toStdString(), shapes[i][k],
                            cv::FONT_HERSHEY_SIMPLEX, 0.4, color, 2);
            }
        }
    }
}
