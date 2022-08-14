#pragma once

#define ARDUINO_WAIT_TIME 2000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

class Serial {
    private:
        //Serial comm handler
        HANDLE handle;
        //Connection status
        bool connected;
        //Get various information about the connection
        COMSTAT status;
        //Keep track of last error
        DWORD errors;

    public:
        Serial(string portName);
        ~Serial();
        int read(char *buffer, unsigned int num_bytes);
        bool write(const char *buffer, unsigned int num_bytes);
        bool is_connected();
        int available();


};
