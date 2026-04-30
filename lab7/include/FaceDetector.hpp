#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

class FaceDetector {
public:
    FaceDetector(const std::string& prototxtPath, const std::string& modelPath);
    ~FaceDetector();
    void updateFrame(const cv::Mat& frame);
    std::vector<cv::Rect> getFaces();

private:
    void workerLoop();
    cv::dnn::Net net;
    std::thread workerThread;
    std::mutex dataMutex;
    std::atomic<bool> running;
    cv::Mat currentFrame;
    bool hasNewFrame;
    std::vector<cv::Rect> detectedFaces;
};