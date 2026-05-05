#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
public:
    cv::Mat process(const cv::Mat& inputFrame, ProcessingMode mode);

private:
    cv::Mat applyInvert(const cv::Mat& input);
    cv::Mat applyBlur(const cv::Mat& input);
    cv::Mat applyCanny(const cv::Mat& input);
    cv::Mat applyGlitch(const cv::Mat& input);
};