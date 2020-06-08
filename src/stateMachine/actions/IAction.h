//
// Created by tbatt on 8/06/2020.
//

#ifndef RPMFLAGSBAR_IRACING_PROJ_IACTION_H
#define RPMFLAGSBAR_IRACING_PROJ_IACTION_H


// Standard Includes
#include <memory>

// Project Includes
#include <src/serial/Serial.h>


class IAction {
public:
    // Constructor
    IAction(const char* actionName);

    // Functions
    virtual void updateAction(bool flagState) = 0;
    virtual void sendAction(std::shared_ptr<Serial> arduinoSerial) = 0;

    bool isAvailable();

protected:
    // Data
    const char* actionName;
    bool available = false;  // True if the flags for this action are active

};


#endif //RPMFLAGSBAR_IRACING_PROJ_IACTION_H
