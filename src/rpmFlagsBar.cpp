//
// Created by tbatt on 5/06/2020.
//

// Standard Includes
#include <iostream>
#include <memory>


// Project Includes
#include "src/irData/IRData.h"
#include "src/stateMachine/StateMachine.h"
#include <src/configParser/ConfigParser.h>


int main(int argc, char* argv[]) {
    std::cout << "Starting RpmFlagsBar Version 0.1" << std::endl;

    // Read Json Config File
    // Load file
    ConfigParser cp("../../config/config.cfg");

    // Establish a connection to the arduino
    std::shared_ptr<Serial> arduinoSerial = std::make_shared<Serial>(cp.getCOMPort().c_str());

    // Create IRData
    std::shared_ptr<IRData> irData;
    if(argc == 1 || strcmp(argv[1], "sim") == 0) {
        irData = std::make_shared<IRData>();
    } else if (strcmp(argv[1], "file") == 0) {
        const char* filepath = R"(C:\Users\tbatt\Documents\iRacing\telemetry\trucks silverado_daytona oval 2020-06-06 21-34-23.ibt)";
        irData = std::make_shared<IRData>(filepath);
    }

    // Create state machine
    std::shared_ptr<ConfigParser> cpPt = std::make_shared<ConfigParser>(cp);
    std::shared_ptr<IRData> irDataPt = std::shared_ptr<IRData>(irData);
    StateMachine stateMachine = StateMachine(cpPt, irDataPt, arduinoSerial);

}