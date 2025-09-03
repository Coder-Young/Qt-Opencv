#include "capturethread.h"

#include "utilities/utilities.h"

#include <QDebug>
#include <QTime>
#include <QFile>
#include <QImage>
#include <QApplication>

CaptureThread::CaptureThread(int camera, QMutex *lock):
    running(false), cameraID(camera), videoPath(""), data_lock(lock), taking_photo(false)
{
    frame_width = frame_height = 0;
    loadOrnaments();
}

CaptureThread::CaptureThread(QString videoPath, QMutex *lock):
    running(false), cameraID(-1), videoPath(videoPath), data_lock(lock), taking_photo(false)
{
    frame_width = frame_height = 0;
    loadOrnaments();
}

CaptureThread::~CaptureThread() {
}

void CaptureThread::loadOrnaments()
{
    QImage image;
    image.load(":/images/glasses.jpg");
    image = image.convertToFormat(QImage::Format_RGB888);
    glasses = cv::Mat(
                 image.height(), image.width(), CV_8UC3,
                 image.bits(), image.bytesPerLine()).clone();

    image.load(":images/mustache.jpg");
    image = image.convertToFormat(QImage::Format_RGB888);
    mustache = cv::Mat(
        image.height(), image.width(), CV_8UC3,
        image.bits(), image.bytesPerLine()).clone();

    image.load(":images/mouse-nose.jpg");
    image = image.convertToFormat(QImage::Format_RGB888);
    mouse_nose = cv::Mat(
        image.height(), image.width(), CV_8UC3,
        image.bits(), image.bytesPerLine()).clone();
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
                //QString index = QString("%1").arg(k);
                //cv::putText(frame, index.toStdString(), shapes[i][k],
                //            cv::FONT_HERSHEY_SIMPLEX, 0.4, color, 2);
            }
            drawGlasses(frame, shapes[i]);
        }
    }
}

void CaptureThread::drawGlasses(cv::Mat &frame, std::vector<cv::Point2f> &marks)
{
    if (glasses.empty())
        return;

    // 设置眼镜大小适配脸型
    cv::Mat ornament;
    double distance = cv::norm(marks[45] - marks[36]) * 1.5;
    cv::resize(glasses, ornament, cv::Size(0, 0), distance / glasses.cols, distance / glasses.cols, cv::INTER_NEAREST);

    // 旋转眼镜到适配脸型
    double angle = -atan((marks[45].y - marks[36].y) / (marks[45].x - marks[36].x));
    cv::Point2f center = cv::Point(ornament.cols/2, ornament.rows/2);
    cv::Mat rotateMatrix = cv::getRotationMatrix2D(center, angle * 180 / 3.14, 1.0);

    cv::Mat rotated;
    cv::warpAffine(
        ornament, rotated, rotateMatrix, ornament.size(),
        cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));

    // paint
    center = cv::Point((marks[45].x + marks[36].x) / 2, (marks[45].y + marks[36].y) / 2);
    cv::Rect rec(center.x - rotated.cols / 2, center.y - rotated.rows / 2, rotated.cols, rotated.rows);
    frame(rec) &= rotated;
}
