//
// Created by tbatt on 8/06/2020.
//

#include "TimedAction.h"

TimedAction::TimedAction(const char *actionName, unsigned int actionCode, unsigned int durationMs) : IAction(actionName) {
    this->actionCode = actionCode;
    this->durationMs = durationMs;
    this->available = false;
}

void TimedAction::updateAction(bool flagState) {
    // If flag is active in irData
    if (flagState) {
        // Check if flag is already active
        if (isAvailable()) {
            // Check if current action has expired
            auto now = std::chrono::steady_clock::now();
            auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - expiryTime).count();
            this->available = timeDiff <= 0;
            if(!isAvailable()) {
                std::cout << "[Action]: " << actionName << " has expired." << std::endl;
            }
        } else {
            // Check if this is the start of a new flag
            if(!lastFlagState) {
                // Start new flag
                this->available = true;
                // Set expiry time
                expiryTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(durationMs);
            } else {
                // Flag is active but must have expired
                this->available = false;
            }
        }
    } else {
        // Turn flag off
        this->available = false;
    }
    lastFlagState = flagState;
}

void TimedAction::sendAction(std::shared_ptr<Serial> arduinoSerial) {
    // Send action
    std::string outStr = std::string("<") + std::to_string(actionCode) + std::string(">");
    const char* strToSend = outStr.c_str();
    arduinoSerial->WriteData(strToSend, strlen(strToSend));
    // Log action
    std::cout << "[Action]: Sent " << actionName << " - " << strToSend << std::endl;
}
