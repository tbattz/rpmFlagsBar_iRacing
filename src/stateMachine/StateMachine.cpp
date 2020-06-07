//
// Created by tbatt on 5/06/2020.
//

#include "StateMachine.h"

#include <utility>


StateMachine::StateMachine(std::shared_ptr<ConfigParser> configParser, std::shared_ptr<IRData> irDataPt, std::shared_ptr<Serial> arduinoSerial) {
    // Store config parser
    this->configParser = std::move(configParser);
    // Store IRData
    this->irData = std::move(irDataPt);
    // Store Arduino File
    this->arduinoSerial = std::move(arduinoSerial);
    // Set initial states
    connState = DISCONNECTED;
    setActionState(INACTIVE);

    // Start state loop
    StateMachine::stateLoop();
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
    }
}

void StateMachine::stateDisconnected() {
    // Try to reconnect
    irData->updateData();
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

        // Send inactive
        StateMachine::sendInactive();

        // Sleep for a little
        std::this_thread::sleep_for (std::chrono::milliseconds(disconnectedDelay));
    }
}

void StateMachine::stateConnected() {
    // Check we are still connected
    irData->updateData();
    if(irData->isConnected()) {
        switch (actionState) {
            case INACTIVE: {
                StateMachine::actionInactive();
                break;
            }
            case DISPLAY_PIT_LIMITER: {
                StateMachine::actionDisplayPitLimiter();
                break;
            }
            case DISPLAY_CHECKERED_FLAG: {
                StateMachine::actionDisplayCheckeredFlag();
                break;
            }
            case DISPLAY_RED_FLAG: {
                StateMachine::actionDisplayRedFlag();
                break;
            }
            case DISPLAY_YELLOW_FLAG: {
                StateMachine::actionDisplayYellowFlag();
                break;
            }
            case DISPLAY_GREEN_FLAG: {
                StateMachine::actionDisplayGreenFlag();
                break;
            }
            case DISPLAY_BLUE_FLAG: {
                StateMachine::actionDisplayBlueFlag();
                break;
            }
            case DISPLAY_WHITE_FLAG: {
                StateMachine::actionDisplayWhiteFlag();
                break;
            }
            case DISPLAY_RPM: {
                StateMachine::actionDisplayWhiteFlag();
                break;
            }
            default: {
                std::cout << "Invalid Action State!" << std::endl;
            }
        }
    } else {
        connState = DISCONNECTED;
        currentCar = ""; // Set current car to nothing
        rpmScale = RpmScale();
        std::cout << "[DISCONNECTED]: Disconnected from to IRacing Server" << std::endl;

        // Sleep for a little
        std::this_thread::sleep_for (std::chrono::milliseconds(disconnectedDelay));
    }
}

void StateMachine::actionInactive() {
    // Wait for car to come on track
    if(irData->isCarOnTrack()) {
        // Update flags and limiter
        StateMachine::checkCurrentAction();
    } else {
        // Delay until next check
        std::this_thread::sleep_for (std::chrono::milliseconds(inactiveDelay));
    }
}

void StateMachine::actionDisplayPitLimiter() {

}

void StateMachine::actionDisplayCheckeredFlag() {

}

void StateMachine::actionDisplayRedFlag() {

}

void StateMachine::actionDisplayYellowFlag() {

}

void StateMachine::actionDisplayGreenFlag() {

}

void StateMachine::actionDisplayBlueFlag() {

}

void StateMachine::actionDisplayWhiteFlag() {

}

void StateMachine::actionDisplayRpm() {

}

void StateMachine::checkCurrentAction() {
    if(irData->isCarOnTrack()) {
        // Update stored flags
        StateMachine::updateGlobalFlags();
        /* Hierarchy of flags to display
         * PIT_LIMITER
         * CHECKERED
         * RED
         * YELLOW
         * GREEN
         * BLUE
         * WHITE
         * RPM */
        if (driverFlags.pitLimiter) {
            // Always send Pit Limiter
            setActionState(DISPLAY_PIT_LIMITER);
            StateMachine::sendPitLimiter();
        } else if (globalFlags.checkered) {
            // Initiate Checkered Flag
            setActionState(DISPLAY_CHECKERED_FLAG);
            StateMachine::sendCheckeredFlag();
        } else if (globalFlags.red) {
            // Initiate Red Flag
            setActionState(DISPLAY_RED_FLAG);
            StateMachine::sendRedFlag();
        } else if (globalFlags.yellow) {
            // Initiate Yellow Flag
            setActionState(DISPLAY_YELLOW_FLAG);
            StateMachine::sendYellowFlag();
        } else if (globalFlags.green) {
            // Initiate Green Flag
            setActionState(DISPLAY_GREEN_FLAG);
            StateMachine::sendGreenFlag();
        } else if (globalFlags.blue) {
            // Initiate Blue Flag
            setActionState(DISPLAY_BLUE_FLAG);
            StateMachine::sendBlueFlag();
        } else if (globalFlags.white) {
            // Initiate White Flag
            setActionState(DISPLAY_WHITE_FLAG);
            StateMachine::sendWhiteFlag();
        } else {
            setActionState(DISPLAY_RPM);
            StateMachine::sendRPM();
        }
    } else {
        setActionState(INACTIVE);
        StateMachine::sendInactive();
        std::cout << "[INACTIVE]: Car not on track" << std::endl;
        // Delay until next check
        std::this_thread::sleep_for (std::chrono::milliseconds(inactiveDelay));
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

void StateMachine::setActionState(ActionState newActionState) {
    if (actionState != newActionState) {
        // New state set
        actionState = newActionState;
        std::string actionStr = "";
        if(actionState == INACTIVE) {
            actionStr = "INACTIVE";
        } else if (actionState == DISPLAY_PIT_LIMITER) {
            actionStr = "DISPLAY_PIT_LIMITER";
        } else if (actionState == DISPLAY_CHECKERED_FLAG) {
            actionStr = "DISPLAY_CHECKERED_FLAG";
        } else if (actionState == DISPLAY_RED_FLAG) {
            actionStr = "DISPLAY_RED_FLAG";
        } else if (actionState == DISPLAY_YELLOW_FLAG) {
            actionStr = "DISPLAY_YELLOW_FLAG";
        } else if (actionState == DISPLAY_GREEN_FLAG) {
            actionStr = "DISPLAY_GREEN_FLAG";
        } else if (actionState == DISPLAY_BLUE_FLAG) {
            actionStr = "DISPLAY_BLUE_FLAG";
        } else if (actionState == DISPLAY_WHITE_FLAG) {
            actionStr = "DISPLAY_WHITE_FLAG";
        } else if (actionState == DISPLAY_RPM) {
            actionStr = "DISPLAY_RPM";
        }
        std::cout << "[ActionState]: " << actionStr << std::endl;
    }
}

void StateMachine::sendPitLimiter() {
    const char* strToSend = "<1>";
    this->arduinoSerial->WriteData(strToSend, strlen(strToSend));
}

void StateMachine::sendCheckeredFlag() {
    const char* strToSend = "<2>";
    this->arduinoSerial->WriteData(strToSend, strlen(strToSend));
}

void StateMachine::sendRedFlag() {
    const char* strToSend = "<3>";
    this->arduinoSerial->WriteData(strToSend, strlen(strToSend));
}

void StateMachine::sendYellowFlag() {
    const char* strToSend = "<4>";
    this->arduinoSerial->WriteData(strToSend, strlen(strToSend));
}

void StateMachine::sendGreenFlag() {
    const char* strToSend = "<5>";
    this->arduinoSerial->WriteData(strToSend, strlen(strToSend));
}

void StateMachine::sendBlueFlag() {
    const char* strToSend = "<6>";
    this->arduinoSerial->WriteData(strToSend, strlen(strToSend));
}

void StateMachine::sendWhiteFlag() {
    const char* strToSend = "<7>";
    this->arduinoSerial->WriteData(strToSend, strlen(strToSend));
}

void StateMachine::sendRPM() {
    char rpmOut[7];
    sprintf(rpmOut, "<8,%03d>", 50);
    this->arduinoSerial->WriteData(rpmOut, strlen(rpmOut));
}

void StateMachine::sendInactive() {
    const char* strToSend = "<0>";
    this->arduinoSerial->WriteData(strToSend, strlen(strToSend));
}
