//
// Created by tbatt on 8/06/2020.
//

#include "ContinuousRpmAction.h"


ContinuousRpmAction::ContinuousRpmAction(const char *actionName, unsigned int actionCode) :
        ContinuousAction(actionName, actionCode) {
    this->available = false;
}

void ContinuousRpmAction::updateAction(bool flagState) {
    newUpdate = !this->available && newUpdate;
    ContinuousAction::updateAction(flagState);
}

void ContinuousRpmAction::sendAction(std::shared_ptr<Serial> arduinoSerial) {
    // Send action
    char rpmOut[7];
    sprintf(rpmOut, "<8,%03d>", rpmPer);
    arduinoSerial->WriteData(rpmOut, strlen(rpmOut));
    // Log action
    if (newUpdate) {
        std::cout << "[Action]: Sent " << actionName << " - " << rpmOut << std::endl;
    }
}

void ContinuousRpmAction::setRpmScale(RpmScale rpmScale) {
    this->rpmScale = rpmScale;
    // Calculate coefficients
    m = 100 / (float)(rpmScale.maxRpm - rpmScale.minRpm);
    c = -m * (float)rpmScale.minRpm;
}

void ContinuousRpmAction::setRpm(unsigned int newRpm) {
    // Set rpm
    this->rpm = newRpm;
    // Calculate rpm percentage
    if (rpm > rpmScale.minRpm) {
        if (rpm < rpmScale.maxRpm) {
            rpmPer = (unsigned int)( (m*(float)rpm) + c);
        } else {
            rpmPer = 100;
        }
    }
}

