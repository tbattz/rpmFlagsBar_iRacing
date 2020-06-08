//
// Created by tbatt on 8/06/2020.
//

#ifndef RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSRPMACTION_H
#define RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSRPMACTION_H

// Project Includes
#include "ContinuousAction.h"


class ContinuousRpmAction : public ContinuousAction {
public:
    // Constructor
    ContinuousRpmAction(const char* actionName, unsigned int actionCode);

    // Functions
    void updateAction(bool flagState);
    void sendAction(std::shared_ptr<Serial> arduinoSerial);

private:
    bool newUpdate = true;  // Used to only log at the start of sending rpm, to avoid spamming output
};


#endif //RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSRPMACTION_H
