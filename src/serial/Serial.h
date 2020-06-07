//
// Created by tbatt on 7/06/2020.
//

#ifndef RPMFLAGSBAR_IRACING_PROJ_SERIAL_H
#define RPMFLAGSBAR_IRACING_PROJ_SERIAL_H

// Arduino Reset Time
#define ARDUINO_WAIT_TIME 2000

// Windows Includes
#include <windows.h>

// Standard Includes
#include <cstdio>
#include <iostream>



class Serial {
    // Based on https://playground.arduino.cc/Interfacing/CPPWindows/
public:
    // Initialise connection with given COM port
    Serial(const char* comPortName);

    // Close the connection
    ~Serial();
    // Read data in a buffer, if nbCHar is greater than the maximum number of bytes available,
    // it will return only the bytes available. The function returns -1 when nothing could be read,
    // otherwise returns the number of bytes actually read.
    int ReadData(char* buffer, unsigned int nbChar);
    // Writes data from a buffer through the Serial Connection, return true on success
    bool WriteData(const char* buffer, unsigned int nbChar);
    // Check for connection
    bool IsConnected();

private:
    // Com Port
    const char* comPort = "";
    // Serial comm handle
    HANDLE hSerial;
    // Arduino Connection Status
    bool connected = false;
    // Get information about connection
    COMSTAT status;
    // Keep track of the last error
    DWORD errors;

};


#endif //RPMFLAGSBAR_IRACING_PROJ_SERIAL_H
