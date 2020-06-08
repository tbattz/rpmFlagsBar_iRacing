//
// Created by tbatt on 5/06/2020.
//

#include "StateMachine.h"



StateMachine::StateMachine(std::shared_ptr<ConfigParser> configParser, std::shared_ptr<IRData> irDataPt, std::shared_ptr<Serial> arduinoSerial) {
    // Store config parser
    this->configParser = std::move(configParser);
    // Store IRData
    this->irData = std::move(irDataPt);
    // Store Arduino File
    this->arduinoSerial = std::move(arduinoSerial);
    // Set initial states
    connState = DISCONNECTED;
    // Add Actions
    StateMachine::addActions();

    // Send inactive as default startup
    // TODO - Custom startup cycling colours
    StateMachine::sendAction(INACTIVE);

    // Start state loop
    StateMachine::stateLoop();
}

void StateMachine::addActions() {
    // The order actions are added, determines their hierarchy when looping over the map
    actions.insert({INACTIVE, std::make_shared<ContinuousAction>("INACTIVE", INACTIVE)});
    actions.insert({DISPLAY_RPM, std::make_shared<ContinuousRpmAction>("DISPLAY_RPM", DISPLAY_RPM)});
}

void StateMachine::updateActions() {
    // Update actions with the new flag state
    actions[INACTIVE]->updateAction(true);
    actions[DISPLAY_RPM]->updateAction(irData->isCarOnTrack());
}

void StateMachine::sendAction(LEDAction action) {
    // Set Current action
    currAction = action;
    // Send action
    this->actions[action]->sendAction(this->arduinoSerial);
}



void StateMachine::stateLoop() {
    while (running) {
        // Try to update data
        irData->updateData();
        // Check if we have a connection
        switch (connState) {
            case DISCONNECTED: {
                StateMachine::stateDisconnected();
                break;
            }
            case CONNECTED: {
                StateMachine::stateConnected();
                break;
            }
            default: {
                std::cout << "Invalid connection state!" << std::endl;
            }
        }
        // Sleep for a little
        std::this_thread::sleep_for (std::chrono::milliseconds(loopDelay));
    }
}

void StateMachine::stateDisconnected() {
    // Try to reconnect
    if(irData->isConnected()) {
        connState = CONNECTED;
        std::cout << "[CONNECTED]: Connected to IRacing Server" << std::endl;
        // Check for player car
        unsigned int carIdx = irData->getVarInt("PlayerCarIdx", 0);
        std::string filterStr = "DriverInfo:Drivers:CarIdx:{" + std::to_string(carIdx) + "}CarScreenName:";
        currentCar = irData->getSessionStrVal(filterStr);
        std::cout << "[CONNECTED]: Current car: " << currentCar << std::endl;
        // Check for configuration for this car
        rpmScale = configParser->getCarRpmScale(currentCar);
        std::cout << "[CONNECTED]: Rpm Scale: (" << rpmScale.minRpm << ", " << rpmScale.maxRpm << ")" << std::endl;
    } else {
        std::cout << "[DISCONNECTED]: Unable to connect to IRacing Server" << std::endl;

        // Sleep for a little
        std::this_thread::sleep_for (std::chrono::milliseconds(disconnectedDelay));
    }
}

void StateMachine::stateConnected() {
    /* Hierarchy of flags to display
     * PIT_LIMITER
     * CHECKERED
     * RED
     * YELLOW
     * GREEN
     * BLUE
     * WHITE
     * RPM */
    // Update stored flags
    StateMachine::updateGlobalFlags();
    // Update current action states
    StateMachine::updateActions();

    // Check we are still connected
    if(irData->isConnected()) {
        // Find the first action which is available in the hierarchy
        if (actions[DISPLAY_RPM]->isAvailable()) {
            StateMachine::sendAction(DISPLAY_RPM);
        } else if (actions[INACTIVE]->isAvailable()) {
            StateMachine::sendAction(INACTIVE);
        }



    } else {
        connState = DISCONNECTED;
        currentCar = ""; // Set current car to nothing
        rpmScale = RpmScale();
        // Reset flags
        globalFlags = {false, false, false, false, false, false};
        driverFlags = {false, false, false};

        // Send action
        StateMachine::sendAction(INACTIVE);

        // Log Disconnected
        std::cout << "[DISCONNECTED]: Disconnected from to IRacing Server" << std::endl;

        // Sleep for a little
        std::this_thread::sleep_for (std::chrono::milliseconds(disconnectedDelay));
    }
}

void StateMachine::updateGlobalFlags() {
    // Multiple flags can be enabled at once, flags are bitfields.
    unsigned int sessionFlag = irData->getVarInt("SessionFlags", 0);
    unsigned int engineWarnings = irData->getVarInt("EngineWarnings", 0);
    rpm = irData->getVarInt("RPM", 0);

    // Check the state of the required flags
    // Global Flags
    globalFlags.checkered = (sessionFlag & irsdk_checkered);
    globalFlags.red = (sessionFlag & irsdk_red);
    globalFlags.yellow = (sessionFlag & irsdk_yellow) || (sessionFlag & irsdk_yellowWaving) || (sessionFlag & irsdk_caution) || (sessionFlag & irsdk_cautionWaving);
    globalFlags.green = (sessionFlag & irsdk_green);
    globalFlags.blue = (sessionFlag & irsdk_blue);
    globalFlags.white = (sessionFlag & irsdk_white);
    // Driver Flags
    driverFlags.black = (sessionFlag & irsdk_black);
    driverFlags.repair = (sessionFlag & irsdk_repair);
    // Pit Limiter
    driverFlags.pitLimiter = (engineWarnings & irsdk_pitSpeedLimiter);

}


