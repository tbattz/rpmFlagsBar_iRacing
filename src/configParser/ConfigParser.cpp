//
// Created by tbatt on 6/06/2020.
//




#include "ConfigParser.h"


ConfigParser::ConfigParser(const char *configFilePath) {
    // Open file
    std::string line;
    std::ifstream fs;
    fs.open(configFilePath);

    if(!fs.is_open()) {
        printf("Config file %s does not exist!\n", configFilePath);
        std::exit(EXIT_FAILURE);
    }

    // Parse line by line
    while(std::getline(fs, line)) {
        // Split by :
        std::vector<std::string> splitString = ConfigParser::splitStringDelim(line, ":");
        // Check for com port
        if (splitString.size() > 1 && (splitString[0] == "comPort")) {
            comPort = splitString[1];
            std::cout << "[CONFIG]: COM Port: " << comPort << std::endl;
        } else if (splitString.size() > 1 && splitString[0] == "cars") {
            for(unsigned int i=1; i < splitString.size(); i++) {
                // Split substring by commas
                std::vector<std::string> carSplitString = ConfigParser::splitStringDelim(splitString[i], ",");
                std::string carName = carSplitString[0];
                RpmScale rpmScale = {stoul(carSplitString[1]), stoul(carSplitString[2])};
                carsMap.insert({carName, rpmScale});
                std::cout << "[CONFIG]: Found Car: " << carName << " - Rpm Range (" << rpmScale.minRpm << ", " << rpmScale.maxRpm << ")" << std::endl;
            }
        }
    }
}

std::vector<std::string> ConfigParser::splitStringDelim(std::string inString, std::string delim) {
    /* Returns a vector of strings split by a deliminator. */
    unsigned int pos1 = 0;
    unsigned int pos2 = inString.find(delim,pos1);
    std::vector<std::string> outVec;
    while (pos2 != std::string::npos) {
        // Found delim
        outVec.push_back(inString.substr(pos1,pos2-pos1));
        pos1 = pos2 + 1;
        pos2 = inString.find(delim,pos1);
        if (pos2 > inString.length()) {
            // Find . for extension
            pos2 = inString.find(".",pos1);
            outVec.push_back(inString.substr(pos1,pos2-pos1));
            break;
        }
    }

    return outVec;
}

std::string ConfigParser::getCOMPort() {
    return R"(\\.\)" + comPort;
}

RpmScale ConfigParser::getCarRpmScale(const std::string& carName) {
    // If car is not found, returns default rpm scale.
    std::unordered_map<std::string, RpmScale>::const_iterator mapIt = carsMap.find(carName);
    if(mapIt == carsMap.end()) {
        std::cout << "[Config]: Could not find RpmScale for car " << carName << std::endl;
        RpmScale defaultRpmScale = {};
        return defaultRpmScale;
    } else {
        return mapIt->second;
    }
}




