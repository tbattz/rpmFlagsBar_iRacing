//
// Created by tbatt on 5/06/2020.
//

#ifndef RPMFLAGSBAR_IRACING_PROJ_STATEMACHINE_H
#define RPMFLAGSBAR_IRACING_PROJ_STATEMACHINE_H

// Standard Includes
#include <iostream>
#include <memory>
#include <utility>
#include <thread>
#include <chrono>

// Project Includes
#include "src/irData/IRData.h"

// IRacing SDK Includes
#include <irsdk/irsdk_defines.h>


enum ConnectionState {
    DISCONNECTED,
    CONNECTED,
};

enum ActionState {
    INACTIVE,
    DISPLAY_PIT_LIMITER,
    DISPLAY_CHECKERED_FLAG,
    DISPLAY_RED_FLAG,
    DISPLAY_YELLOW_FLAG,
    DISPLAY_GREEN_FLAG,
    DISPLAY_BLUE_FLAG,
    DISPLAY_WHITE_FLAG,
    DISPLAY_RPM,
    DISPLAY_PENALTY,
    DISPLAY_REPAIR
};

struct GlobalFlags {
    bool checkered = false;
    bool red = false;
    bool yellow = false;
    bool green = false;
    bool blue = false;
    bool white = false;
};

struct DriverFlags {
    bool repair = false;
    bool black = false;
    bool pitLimiter = false;
};


class StateMachine {
public:
    // Constructor
    StateMachine(std::shared_ptr<IRData> irDataPt);

private:
    // Data
    bool running = true;
    std::shared_ptr<IRData> irData;
    ConnectionState connState;
    ActionState actionState;
    // Sleep Timings
    unsigned int disconnectedDelay = 1000; // ms
    unsigned int inactiveDelay = 500; // ms
    // Flag states
    // Global
    GlobalFlags globalFlags;
    // Driver
    DriverFlags driverFlags;
    // RPM
    unsigned int rpm = 0;

    // Functions
    // States
    void stateLoop();
    void stateDisconnected();
    void stateConnected();
    void actionInactive();
    void checkCurrentAction();
    void updateGlobalFlags();
    // Commands
    void sendPitLimiter();
    void sendCheckeredFlag();
    void sendRedFlag();
    void sendYellowFlag();
    void sendGreenFlag();
    void sendBlueFlag();
    void sendWhiteFlag();
    void sendRPM();
    void sendInactive();


};


#endif //RPMFLAGSBAR_IRACING_PROJ_STATEMACHINE_H
