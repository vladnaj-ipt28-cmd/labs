#pragma once

enum class ProcessingMode {
    NORMAL = 0,
    INVERT,
    BLUR,
    CANNY,
    GLITCH
};

class KeyProcessor {
private:
    ProcessingMode currentMode;
    bool shouldExit;

public:
    KeyProcessor();
    
    void handleKey(int key);
    
    ProcessingMode getMode() const;
    
    bool isExitRequested() const;
};