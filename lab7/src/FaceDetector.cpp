#include "FaceDetector.hpp"
#include <iostream>

FaceDetector::FaceDetector(const std::string& prototxtPath, const std::string& modelPath)
    : running(true), hasNewFrame(false) {
    try {
        net = cv::dnn::readNetFromCaffe(prototxtPath, modelPath);
    } catch (const cv::Exception& e) {
        std::cerr << "Помилка завантаження нейромережі: " << e.what() << std::endl;
    }
    workerThread = std::thread(&FaceDetector::workerLoop, this);
}

FaceDetector::~FaceDetector() {
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(dataMutex);
    if (!frame.empty()) {
        currentFrame = frame.clone();
        hasNewFrame = true;
    }
}

std::vector<cv::Rect> FaceDetector::getFaces() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return detectedFaces;
}

void FaceDetector::workerLoop() {
    while (running) {
        cv::Mat frameToProcess;
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            if (hasNewFrame) {
                frameToProcess = currentFrame.clone();
                hasNewFrame = false;
            }
        }

        if (!frameToProcess.empty() && !net.empty()) {
            cv::Mat blob = cv::dnn::blobFromImage(frameToProcess, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));
            net.setInput(blob);
            cv::Mat detection = net.forward();

            cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
            std::vector<cv::Rect> faces;

            for (int i = 0; i < detectionMat.rows; i++) {
                float confidence = detectionMat.at<float>(i, 2);
                if (confidence > 0.5) { // Впевненість > 50%
                    int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frameToProcess.cols);
                    int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frameToProcess.rows);
                    int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frameToProcess.cols);
                    int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frameToProcess.rows);

                    faces.push_back(cv::Rect(cv::Point(xLeftBottom, yLeftBottom), cv::Point(xRightTop, yRightTop)));
                }
            }

            {
                std::lock_guard<std::mutex> lock(dataMutex);
                detectedFaces = faces;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}