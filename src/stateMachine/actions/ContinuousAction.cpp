//
// Created by tbatt on 8/06/2020.
//


#include "ContinuousAction.h"


ContinuousAction::ContinuousAction(const char *actionName, unsigned int actionCode) : IAction(actionName) {
    this->actionCode = actionCode;
    this->available = true;
}

void ContinuousAction::updateAction(bool flagState) {
    this->available = flagState;
}

void ContinuousAction::sendAction(std::shared_ptr<Serial> arduinoSerial) {
    // Send action
    std::string outStr = std::string("<") + std::to_string(actionCode) + std::string(">");
    const char* strToSend = outStr.c_str();
    arduinoSerial->WriteData(strToSend, strlen(strToSend));
    // Log action
    std::cout << "[Action]: Sent " << actionName << " - " << strToSend << std::endl;
}
