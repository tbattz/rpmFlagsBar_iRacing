//
// Created by tbatt on 8/06/2020.
//

#ifndef RPMFLAGSBAR_IRACING_PROJ_TIMEDACTION_H
#define RPMFLAGSBAR_IRACING_PROJ_TIMEDACTION_H

// Standard Includes
#include <chrono>

// Project Includes
#include "IAction.h"


class TimedAction : public IAction {
public:
    // Constructor
    TimedAction(const char* actionName, unsigned int actionCode, unsigned int durationMs);

    // Functions
    void updateAction(bool flagState);
    void sendAction(std::shared_ptr<Serial> arduinoSerial);

private:
    // Data
    unsigned int actionCode;
    bool lastFlagState = true;        // Keeps track of the start of a flag
    unsigned int durationMs;    // Duration of action in ms
    std::chrono::steady_clock::time_point expiryTime = std::chrono::steady_clock::now();

};


#endif //RPMFLAGSBAR_IRACING_PROJ_TIMEDACTION_H
