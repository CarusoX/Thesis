#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <map>

class CLI {
public:
    CLI();
    ~CLI();

    // Progress bar
    void startProgress(const std::string& id, const std::string& label, size_t total);
    void updateProgress(const std::string& id, size_t current);
    void finishProgress(const std::string& id);

    // Status messages
    void printStatus(const std::string& message);
    void printError(const std::string& message);
    void printSuccess(const std::string& message);

    // Time difference warning
    void checkTimeDiff(double currentTime, double prevTime);

private:
    struct ProgressState {
        bool active;
        size_t total;
        size_t current;
        std::string label;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastUpdate;
    };

    std::map<std::string, ProgressState> progressStates;
    static constexpr std::chrono::milliseconds MIN_UPDATE_INTERVAL{1000}; // 1s minimum between updates

    // Console manipulation
    void clearLine();
    void moveCursorUp(int lines);
    void saveCursor();
    void restoreCursor();
    void updateAllProgress();
    
    // Formatting
    std::string formatTime(std::chrono::seconds seconds);
    std::string formatProgressBar(size_t current, size_t total, size_t width = 50);
}; 