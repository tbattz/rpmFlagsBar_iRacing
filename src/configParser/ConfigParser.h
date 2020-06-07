//
// Created by tbatt on 6/06/2020.
//

#ifndef RPMFLAGSBAR_IRACING_PROJ_CONFIGPARSER_H
#define RPMFLAGSBAR_IRACING_PROJ_CONFIGPARSER_H


// Standard Includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <unordered_map>


struct RpmScale {
    unsigned int minRpm = 0;
    unsigned int maxRpm = 1;
};


class ConfigParser {
public:
    // Constructor
    ConfigParser(const char* configFilePath);

    // Functions
    std::string getCOMPort();
    RpmScale getCarRpmScale(const std::string& carName);

private:
    // Data
    std::string comPort = "";
    std::unordered_map<std::string, RpmScale> carsMap;

    // Functions
    std::vector<std::string> splitStringDelim(std::string inString, std::string delim);
    void parseData(std::string inputString);

};


#endif //RPMFLAGSBAR_IRACING_PROJ_CONFIGPARSER_H
