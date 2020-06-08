//
// Created by tbatt on 8/06/2020.
//

#ifndef RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSRPMACTION_H
#define RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSRPMACTION_H

// Project Includes
#include <src/configParser/ConfigParser.h>
#include "ContinuousAction.h"


class ContinuousRpmAction : public ContinuousAction {
public:
    // Constructor
    ContinuousRpmAction(const char* actionName, unsigned int actionCode);

    // Functions
    void updateAction(bool flagState);
    void sendAction(std::shared_ptr<Serial> arduinoSerial);
    void setRpmScale(RpmScale rpmScale);
    void setRpm(unsigned int newRpm);

private:
    bool newUpdate = true;  // Used to only log at the start of sending rpm, to avoid spamming output
    RpmScale rpmScale;
    unsigned int rpm = 0;
    unsigned int rpmPer = 0;    // Rpm as a percentage of the LED bar
    float m = 0;  // Gradient of linear interpolation
    float c = 0;  // Intercept of linear interpolation
};


#endif //RPMFLAGSBAR_IRACING_PROJ_CONTINUOUSRPMACTION_H
