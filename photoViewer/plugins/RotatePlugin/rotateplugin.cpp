#include "rotateplugin.h"

QString RotatePlugin::name()
{
    return "Rotate";
}

/*
void RotatePlugin::edit(const cv::Mat &input, cv::Mat &output)
{
    double angle = 45.0;
    double scale = 1.0;

    cv::Point2f center = cv::Point(input.cols / 2, input.rows / 2);
    cv::Mat rotateMatrix = cv::getRotationMatrix2D(center, angle, scale);

    cv::Mat result;
    cv::warpAffine(input, result,
                   rotateMatrix, input.size(),
                   cv::INTER_LINEAR, cv::BORDER_CONSTANT);
    output = result;
}
*/

void RotatePlugin::edit(const cv::Mat &input, cv::Mat &output)
{

    cv::Point2f triangleA[3];
    cv::Point2f triangleB[3];

    triangleA[0] = cv::Point2f(0 , 0);
    triangleA[1] = cv::Point2f(1 , 0);
    triangleA[2] = cv::Point2f(0 , 1);

    triangleB[0] = cv::Point2f(0, 0);
    triangleB[1] = cv::Point2f(1, 0);
    triangleB[2] = cv::Point2f(1, 1);

    cv::Mat affineMatrix = cv::getAffineTransform(triangleA, triangleB);
    cv::Mat result;
    cv::warpAffine(
        input, result,
        affineMatrix, input.size(), // output image size, same as input
        cv::INTER_CUBIC, // Interpolation method
        //cv::BORDER_CONSTANT // Extrapolation method
        //cv::BORDER_WRAP // Extrapolation method
        cv::BORDER_TRANSPARENT
    );

    output = result;
}
