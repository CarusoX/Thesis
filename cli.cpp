#include "cli.hpp"

CLI::CLI() {}

CLI::~CLI() {
    for (const auto& [id, state] : progressStates) {
        if (state.active) {
            finishProgress(id);
        }
    }
}

void CLI::startProgress(const std::string& id, const std::string& label, size_t total) {
    ProgressState state;
    state.active = true;
    state.total = total;
    state.current = 0;
    state.label = label;
    state.startTime = std::chrono::steady_clock::now();
    state.lastUpdate = state.startTime;
    
    progressStates[id] = state;
    std::cout << "\n"; // Start on a new line
    updateProgress(id, 0);
}

void CLI::updateProgress(const std::string& id, size_t current) {
    auto it = progressStates.find(id);
    if (it == progressStates.end() || !it->second.active) return;

    auto now = std::chrono::steady_clock::now();
    if (now - it->second.lastUpdate < MIN_UPDATE_INTERVAL) {
        return; // Skip update if not enough time has passed
    }
    it->second.lastUpdate = now;
    it->second.current = current;

    updateAllProgress();
}

void CLI::updateAllProgress() {
    // Count active progress bars
    size_t activeCount = 0;
    for (const auto& [id, state] : progressStates) {
        if (state.active) activeCount++;
    }
    
    if (activeCount == 0) return;

    // Move cursor up for each active progress bar
    moveCursorUp(activeCount);
    
    // Update each progress bar
    for (const auto& [id, state] : progressStates) {
        if (!state.active) continue;

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - state.startTime);
        
        // Calculate estimated time remaining
        double progress = static_cast<double>(state.current) / state.total;
        auto estimatedTotal = std::chrono::seconds(
            static_cast<long long>(elapsed.count() / progress)
        );
        auto remaining = estimatedTotal - elapsed;

        // Clear the current line
        clearLine();
        
        // Print progress bar and info
        std::cout << state.label << " "
                  << formatProgressBar(state.current, state.total) << " "
                  << std::fixed << std::setprecision(1)
                  << (progress * 100) << "% "
                  << "(" << state.current << "/" << state.total << ") "
                  << "Elapsed: " << formatTime(elapsed) << " "
                  << "Remaining: " << formatTime(remaining)
                  << std::endl;
    }
}

void CLI::finishProgress(const std::string& id) {
    auto it = progressStates.find(id);
    if (it == progressStates.end() || !it->second.active) return;
    
    auto& state = it->second;
    
    // Count active progress bars before finishing this one
    size_t activeCount = 0;
    for (const auto& [pid, pstate] : progressStates) {
        if (pstate.active) activeCount++;
    }
    
    if (activeCount > 0) {
        moveCursorUp(activeCount);
    }
    
    clearLine();
    auto total = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - state.startTime);
    
    std::cout << state.label << " "
              << formatProgressBar(state.total, state.total) << " "
              << "100% "
              << "(" << state.total << "/" << state.total << ") "
              << "Completed in " << formatTime(total)
              << std::endl;
    
    state.active = false;
    
    // Update remaining progress bars if any
    if (activeCount > 1) {
        updateAllProgress();
    }
}

void CLI::printStatus(const std::string& message) {
    size_t activeCount = 0;
    for (const auto& [id, state] : progressStates) {
        if (state.active) activeCount++;
    }
    
    if (activeCount > 0) {
        moveCursorUp(activeCount);
    }
    std::cout << "ℹ️  " << message << std::endl;
    if (activeCount > 0) {
        updateAllProgress();
    }
}

void CLI::printError(const std::string& message) {
    size_t activeCount = 0;
    for (const auto& [id, state] : progressStates) {
        if (state.active) activeCount++;
    }
    
    if (activeCount > 0) {
        moveCursorUp(activeCount);
    }
    std::cout << "❌ " << message << std::endl;
    if (activeCount > 0) {
        updateAllProgress();
    }
}

void CLI::printSuccess(const std::string& message) {
    size_t activeCount = 0;
    for (const auto& [id, state] : progressStates) {
        if (state.active) activeCount++;
    }
    
    if (activeCount > 0) {
        moveCursorUp(activeCount);
    }
    std::cout << "✅ " << message << std::endl;
    if (activeCount > 0) {
        updateAllProgress();
    }
}

void CLI::checkTimeDiff(double currentTime, double prevTime) {
    if (std::abs(currentTime - prevTime) > 1.0) { // Example threshold
        printError("Big time difference detected: " + 
                  std::to_string(currentTime - prevTime) + " seconds");
    }
}

void CLI::clearLine() {
    std::cout << "\r\033[K"; // Carriage return + clear to end of line
}

void CLI::moveCursorUp(int lines) {
    if (lines > 0) {
        std::cout << "\033[" << lines << "A";
    }
}

void CLI::saveCursor() {
    std::cout << "\033[s";
}

void CLI::restoreCursor() {
    std::cout << "\033[u";
}

std::string CLI::formatTime(std::chrono::seconds seconds) {
    auto h = std::chrono::duration_cast<std::chrono::hours>(seconds);
    auto m = std::chrono::duration_cast<std::chrono::minutes>(seconds % std::chrono::hours(1));
    auto s = seconds % std::chrono::minutes(1);
    
    std::stringstream ss;
    if (h.count() > 0) {
        ss << h.count() << "h ";
    }
    if (m.count() > 0 || h.count() > 0) {
        ss << m.count() << "m ";
    }
    ss << s.count() << "s";
    return ss.str();
}

std::string CLI::formatProgressBar(size_t current, size_t total, size_t width) {
    std::string bar;
    bar.reserve(width + 2);
    
    size_t filled = (current * width) / total;
    bar.push_back('[');
    bar.append(filled, '=');
    if (filled < width) {
        bar.push_back('>');
        bar.append(width - filled - 1, ' ');
    }
    bar.push_back(']');
    
    return bar;
} 