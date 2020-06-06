//
// Created by tbatt on 5/06/2020.
//

// Standard Includes
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <streambuf>


// Project Includes
#include "src/irData/IRData.h"
#include "src/stateMachine/StateMachine.h"
#include <src/configParser/ConfigParser.h>


int main(int argc, char* argv[]) {
    std::cout << "Starting RpmFlagsBar Version 0.1" << std::endl;

    // Read Json Config File
    // Load file
    ConfigParser cp("../../config/config.cfg");


    // Connect to Arduino on COM Port
    std::ofstream arduino;
    arduino.open(cp.gettCOMPort());
    arduino << "Hello World from C++!" << std::endl;
    arduino.close();


    // Create IRData
    std::shared_ptr<IRData> irData;
    if(argc == 1 || strcmp(argv[1], "sim") == 0) {
        irData = std::make_shared<IRData>();
    } else if (strcmp(argv[1], "file") == 0) {
        const char* filepath = R"(C:\Users\tbatt\Documents\iRacing\telemetry\dallaraf3_brandshatch indy 2020-04-09 21-32-00.ibt)";
        irData = std::make_shared<IRData>(filepath);
    }

    // Create state machine
    std::shared_ptr<IRData> irDataPt = std::shared_ptr<IRData>(irData);
    StateMachine stateMachine = StateMachine(irDataPt);


}