#include "CameraProvider.hpp"
#include <iostream>

CameraProvider::CameraProvider(int deviceId) {
    cap.open(deviceId);
    if (!cap.isOpened()) {
        std::cerr << "Помилка: Не вдалося відкрити камеру з ID " << deviceId << std::endl;
    }
}

CameraProvider::~CameraProvider() {
    if (cap.isOpened()) {
        cap.release();
    }
}

bool CameraProvider::isOpened() const {
    return cap.isOpened();
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    if (cap.isOpened()) {
        cap >> frame;
    }
    return frame;
}