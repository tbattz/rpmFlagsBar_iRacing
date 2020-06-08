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
    sprintf(rpmOut, "<8,%03d>", 50);
    arduinoSerial->WriteData(rpmOut, strlen(rpmOut));
    // Log action
    if (newUpdate) {
        std::cout << "[Action]: Sent " << actionName << " - " << rpmOut << std::endl;
    }
}




