//
// Created by tbatt on 8/06/2020.
//

#ifndef RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSACTION_H
#define RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSACTION_H

// Standard Includes
#include <string>

// Project Includes
#include "IAction.h"


class ContinuousAction : public IAction {
public:
    // Constructor
    ContinuousAction(const char* actionName, unsigned int actionCode);

    // Functions
    void updateAction(bool flagState);
    void sendAction(std::shared_ptr<Serial> arduinoSerial);

protected:
    // Data
    unsigned int actionCode;
};


#endif //RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSACTION_H
