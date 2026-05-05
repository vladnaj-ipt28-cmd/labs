#include "FrameProcessor.hpp"
#include <vector>

cv::Mat FrameProcessor::process(const cv::Mat& inputFrame, ProcessingMode mode) {
    if (inputFrame.empty()) return inputFrame;

    switch (mode) {
        case ProcessingMode::NORMAL:
            return inputFrame.clone();
        case ProcessingMode::INVERT:
            return applyInvert(inputFrame);
        case ProcessingMode::BLUR:
            return applyBlur(inputFrame);
        case ProcessingMode::CANNY:
            return applyCanny(inputFrame);
        case ProcessingMode::GLITCH:
            return applyGlitch(inputFrame);
        default:
            return inputFrame.clone();
    }
}

cv::Mat FrameProcessor::applyInvert(const cv::Mat& input) {
    cv::Mat result;
    cv::bitwise_not(input, result);
    return result;
}

cv::Mat FrameProcessor::applyBlur(const cv::Mat& input) {
    cv::Mat result;
    cv::GaussianBlur(input, result, cv::Size(15, 15), 0);
    return result;
}

cv::Mat FrameProcessor::applyCanny(const cv::Mat& input) {
    cv::Mat gray, edges;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 50, 150);
    
    cv::Mat result;
    cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
    return result;
}

cv::Mat FrameProcessor::applyGlitch(const cv::Mat& input) {
    cv::Mat result = input.clone();
    std::vector<cv::Mat> channels;
    cv::split(result, channels);

    int shift = 15;
    
    cv::Mat redShifted = cv::Mat::zeros(channels[2].size(), channels[2].type());
    channels[2](cv::Rect(0, 0, result.cols - shift, result.rows)).copyTo(redShifted(cv::Rect(shift, 0, result.cols - shift, result.rows)));
    channels[2] = redShifted;

    cv::Mat blueShifted = cv::Mat::zeros(channels[0].size(), channels[0].type());
    channels[0](cv::Rect(shift, 0, result.cols - shift, result.rows)).copyTo(blueShifted(cv::Rect(0, 0, result.cols - shift, result.rows)));
    channels[0] = blueShifted;

    cv::merge(channels, result);
    return result;
}