#include <iostream>
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>

#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "FaceDetector.hpp"

std::string getModeName(ProcessingMode mode) {
    switch (mode) {
        case ProcessingMode::NORMAL: return "NORMAL (0)";
        case ProcessingMode::INVERT: return "INVERT (1)";
        case ProcessingMode::BLUR:   return "BLUR (2)";
        case ProcessingMode::CANNY:  return "CANNY (3)";
        case ProcessingMode::GLITCH: return "GLITCH (4)";
        case ProcessingMode::FACE:   return "FACE DETECT (F)"; // Додали назву режиму
        default: return "UNKNOWN";
    }
}

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Помилка: Не вдалося підключитися до камери!" << std::endl;
        return -1;
    }

    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;
    FaceDetector faceDetector("deploy.prototxt", "res10_300x300_ssd_iter_140000.caffemodel");

    const std::string windowName = "Lab 7: OpenCV & Multithreading";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    auto prevTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    double currentFps = 0.0;

    while (!keyProcessor.isExitRequested()) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;

        cv::Mat processedFrame = frameProcessor.process(frame, keyProcessor.getMode());

        if (keyProcessor.getMode() == ProcessingMode::FACE) {
            faceDetector.updateFrame(frame);
            
            std::vector<cv::Rect> faces = faceDetector.getFaces();
            
            for (const auto& face : faces) {
                cv::rectangle(processedFrame, face, cv::Scalar(0, 255, 0), 3);
            }
        }

        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - prevTime;
        
        if (elapsed.count() >= 1.0) { 
            currentFps = frameCount / elapsed.count();
            frameCount = 0;
            prevTime = currentTime;
        }

        std::string fpsText = "FPS: " + std::to_string((int)currentFps);
        std::string modeText = "Mode: " + getModeName(keyProcessor.getMode());
        
        cv::putText(processedFrame, fpsText, cv::Point(15, 35), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 3);
        cv::putText(processedFrame, fpsText, cv::Point(15, 35), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        
        cv::putText(processedFrame, modeText, cv::Point(15, 75), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 3);
        cv::putText(processedFrame, modeText, cv::Point(15, 75), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 255), 2);

        cv::imshow(windowName, processedFrame);

        int key = cv::waitKey(1);
        keyProcessor.handleKey(key);
    }

    cv::destroyAllWindows();
    return 0;
}