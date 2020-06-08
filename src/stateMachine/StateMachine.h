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
#include <fstream>
#include <utility>
#include <map>

// IRacing SDK Includes
#include <irsdk/irsdk_defines.h>

// Project Includes
#include "src/irData/IRData.h"
#include <src/serial/Serial.h>
#include <src/configParser/ConfigParser.h>
#include <src/stateMachine/actions/IAction.h>
#include <src/stateMachine/actions/ContinuousAction.h>
#include <src/stateMachine/actions/ContinuousRpmAction.h>




enum ConnectionState {
    DISCONNECTED,
    CONNECTED,
};

enum LEDAction {
    INACTIVE = 0,
    DISPLAY_PIT_LIMITER = 1,
    DISPLAY_CHECKERED_FLAG = 2,
    DISPLAY_RED_FLAG = 3,
    DISPLAY_YELLOW_FLAG = 4,
    DISPLAY_GREEN_FLAG = 5,
    DISPLAY_BLUE_FLAG = 6,
    DISPLAY_WHITE_FLAG = 7,
    DISPLAY_RPM = 8
};

enum ActionType {
    CONTINUOUS = 0,
    FLASH_CONTINUOUS = 1,
    FLASH_X_SECONDS = 2
};

struct GlobalFlags2 {
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
    StateMachine(std::shared_ptr<ConfigParser> configParser, std::shared_ptr<IRData> irDataPt, std::shared_ptr<Serial> arduinoSerial);

private:
    // Data
    bool running = true;
    std::shared_ptr<ConfigParser> configParser;
    std::shared_ptr<IRData> irData;
    std::shared_ptr<Serial> arduinoSerial;
    // Sleep Timings
    unsigned int loopDelay = 10; //ms
    unsigned int disconnectedDelay = 1000; // ms
    unsigned int inactiveDelay = 500; // ms

    // States
    ConnectionState connState;
    std::map<LEDAction, std::shared_ptr<IAction>> actions;
    LEDAction currAction = INACTIVE;
    // Flag states
    // Global
    GlobalFlags2 globalFlags;
    // Driver
    DriverFlags driverFlags;
    // RPM
    unsigned int rpm = 0;
    RpmScale rpmScale = {};
    // Car
    std::string currentCar = "";

    // Functions
    void addActions();
    void updateActions();
    void sendAction(LEDAction action);
    void stateLoop();
    void stateDisconnected();
    void stateConnected();
    void updateGlobalFlags();


};


#endif //RPMFLAGSBAR_IRACING_PROJ_STATEMACHINE_H
