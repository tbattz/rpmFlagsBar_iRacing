//
// Created by tbatt on 7/06/2020.
//

#include "Serial.h"

Serial::Serial(const char* comPortName) {
    // Store com port
    this->comPort = comPortName;
    // Try to connect through the com port
    this->hSerial = CreateFile(comPortName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    // Check if the connection was successful
    if(this->hSerial == INVALID_HANDLE_VALUE) {
        // Display full error
        unsigned int error = GetLastError();
        if(error == ERROR_FILE_NOT_FOUND) {
            printf("[ERROR]: Handle as not attached. Reason: %s not available.\n", comPortName);
        } else {
            printf("[ERROR]: CreateFile - Code %i!!!\n", error);
        }
    } else {
        // Once connected, try to set comm parameters
        DCB dcbSerialParams = {0};

        // Try to get the current params
        if(!GetCommState(this->hSerial, &dcbSerialParams)) {
            // If impossible, show error
            printf("[ERROR]: Failed to get current serial parameters!\n");
        } else {
            // Define connection parameters
            dcbSerialParams.BaudRate = 250000;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            // Setting DTR to Control_Enable ensures that Arduino is properly reset on establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            // Set parameters and check they're applied
            if(!SetCommState(hSerial, &dcbSerialParams)) {
                printf("[ALERT]: Could not set Serial Port Parameters!\n");
            } else {
                // Now connected
                this->connected = true;
                printf("[CONNECTED]: Established connection to Arduino on %s\n", comPortName);
                // FLush any remaining characters in the buffers
                PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                // Wait 2 s will Arduino resets
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
}

Serial::~Serial() {
    // Check if we are connected for attempting to disconnect
    if(this->connected) {
        // No longer connected
        this->connected = false;
        // Close the serial handler
        CloseHandle(this->hSerial);
    }
}

int Serial::ReadData(char *buffer, unsigned int nbChar) {
    // Number of bytes we'll read
    DWORD bytesRead;
    // Number of bytes we'll actually ask to read
    unsigned int toRead;

    // Use the ClearCommError function to get the current status of the Serial Port
    if(this->status.cbInQue > 0) {
        // Check there is enough data to read the required number of characters, otherwise we'll read only the available
        // characters to prevent locking the application.
        if(this->status.cbInQue > nbChar) {
            toRead = nbChar;
        } else {
            toRead = this->status.cbInQue;
        }

        // Try to read the required number of chars, and return the number of bytes read
        if(ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL)) {
            return bytesRead;
        }
    }
    // Nothing read or error detected
    return 0;
}

bool Serial::WriteData(const char *buffer, unsigned int nbChar) {
    if (IsConnected()) {
        DWORD bytesSend;

        // Try to write to the buffer on the serial port
        if (!WriteFile(this->hSerial, (void *) buffer, nbChar, &bytesSend, 0)) {
            // Get error and return false
            ClearCommError(this->hSerial, &this->errors, &this->status);

            return false;
        } else {
            return true;
        }
    } else {
        printf("[ERROR]: Tried to send data while not connected to %s!", comPort);
        return false;
    }
}

bool Serial::IsConnected() {
    return this->connected;
}
