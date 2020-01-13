//
// Created by LHospital
//

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

#define QUIT 'q'
#define SAVE_IMAGE 's'

// TODO: 模块化
// TODO: 更改注释

int main(int argc, char *argv[]) {
    // 读取数据
    std::cout << "------------------------------" << std::endl;
    std::cout << "读取图像!---------------------" << std::endl;

    cv::VideoCapture cap;
    cap.open(0);
    cv::Mat img;
    std::vector<cv::Mat> calibration_images;

    int video_status = 1;
    while(video_status != 0) {
        cap >> img;
        imshow("img", img);

        char key = cv::waitKey(1);
        switch(key) {
            case 's':
                calibration_images.push_back(img);
                break;
            case 'q':
                video_status = 0;
                break;
            default:
                break;
        }
    }
    cap.release();
    cv::destroyAllWindows();

    // 开始标定
    int imgs_num = calibration_images.size();
    if(imgs_num <= 3) {
        std::cerr << "标定图像少于三张，请重新拍摄标定数据！" << std::endl;
        // TODO: 写程序退出保护函数，防止在程序退出之后摄像头资源仍被占用
        return 0;
    }
    cv::Size imageSize(calibration_images[0].size());
    cv::Size broadSize(6, 9);
    std::vector<std::vector<cv::Point2f>> imagePoints;
    // 记录角点
    for(int i = 0; i < imgs_num; i++) {
        cv::Mat imageOrigin = calibration_images[i];
        cv::Mat imageGray;

        // 转换为灰度图
        cv::cvtColor(imageOrigin, imageGray, CV_RGB2GRAY);

        std::cout << ".......>>>>>>>>>>........." << std::endl;
        std::cout << "提取图像" << i + 1 << "的角点>>>>>>>>" << std::endl;

        // 定义角点序列
        std::vector<cv::Point2f> corners;
        if(!cv::findChessboardCorners(imageGray, broadSize, corners)) {
            std::cerr << "在图片" << i + 1 << "中没有发现足够数量的角点" << std::endl;
            continue;
        }

        // 亚像素级角点位置提取
        cv::cornerSubPix(
            imageGray,
            corners,
            cv::Size(11, 11), 
            cv::Size(-1, -1), 
            cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1)
        );
        if(int(corners.size()) != int(broadSize.width + broadSize.height)) {
            std::cerr << "在图片" << i + 1 << "中没有发现足够数量的亚像素角点" << std::endl;
            return 0;
        }

        imagePoints.push_back(corners);

        // 在屏幕上显示出图像与角点，当数量过多时可以将其关闭
        cv::Mat imageExtract = imageOrigin.clone();
        std::vector<cv::Point2f>::iterator it;
        for(it = corners.begin(); it <= corners.end(); it++) {
            cv::circle(imageExtract, *it, 5, cv::Scalar(255, 0, 255), 2);
        }
        cv::imshow("extracted Corners", imageExtract);
        cv::waitKey(0);
    }

    // 定义标定板的世界坐标：以标定板平面为xOy平面
    float scale = 1.2;
    std::vector<std::vector<cv::Point3f>> objectPoints;
    for(int n = 0; n < 10; n++) {
        std::vector<cv::Point3f> objectPoints_eacheImage;
        for(int i = 0; i < broadSize.height; i++) {
            for(int j = 0; j < broadSize.width; j++) {
                objectPoints_eacheImage.push_back(cv::Point3f(i*scale, j*scale, 0.0f));
            }
        }
        objectPoints.push_back(objectPoints_eacheImage);
    }

    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<cv::Mat> rves;
    std::vector<cv::Mat> tves;

    cv::calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rves, tves);

    std::cout << cameraMatrix << std::endl;

    //TODO: 增加畸变矫正后的图像显示
    cv::Mat imageExample = calibration_images[0];
    // cv::Size imageSize(imageExample.size());
    cv::Mat imageExample_withoutDist;
    cv::Mat map1, map2;
    cv::initUndistortRectifyMap(
        cameraMatrix, 
        distCoeffs, 
        cv::Mat(), 
        cv::Mat(), 
        imageSize, 
        CV_16SC2, 
        map1, 
        map2);
    cv::remap(imageExample, imageExample_withoutDist, map1, map2, CV_INTER_LINEAR);
    cv::imshow("Origin", imageExample);
    cv::imshow("Withour Distort", imageExample_withoutDist);
    cv::waitKey(0);

    return 1;
}
