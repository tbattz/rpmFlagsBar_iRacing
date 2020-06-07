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
            std::cout << "[CONFIG] COM Port: " << comPort << std::endl;
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

void ConfigParser::parseData(std::string inputString) {


}

std::string ConfigParser::getCOMPort() {
    return R"(\\.\)" + comPort;
}



