#include "serial.h"
#include <cassert>
#include <string>
#include <iostream>

using namespace std;

Serial::Serial(string portName) {
    wstring w_port_name;
    w_port_name.assign(portName.begin(), portName.end());

    handle = CreateFileW((LPCWSTR)&(w_port_name[0]),
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        if(GetLastError() == ERROR_FILE_NOT_FOUND) {
            cout << "ERROR CONNECTING TO COM: " << portName << " not available." << endl;
        }
        else {
            cout << "ERROR CONNECTING TO COM" << endl;
        }

        return;
    }

    connected = true;

    //If connected we try to set the comm parameters
    DCB dcbSerialParams = {0};

    if (!GetCommState(handle, &dcbSerialParams)) {
        cout << "failed to get current serial parameters!" << endl;
    }
    else {
        //Define serial connection parameters for the arduino board
        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        //Setting the DTR to Control_Enable ensures that the Arduino is properly
        //reset upon establishing a connection
        dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

        //Set the parameters and check for their proper application
        if(!SetCommState(handle, &dcbSerialParams)) {
            cout << "ALERT: Could not set Serial Port parameters" << endl;
        }
        else {
            //Flush any remaining characters in the buffers 
            PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
            //We wait 2s as the arduino board will be reseting
            Sleep(ARDUINO_WAIT_TIME);
        }
    }

}

Serial::~Serial() {
    CloseHandle(handle);
}

int Serial::read(char *buffer, unsigned int num_bytes) {

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(handle, &errors, &status);

    DWORD toRead = min(available(), num_bytes);
    DWORD bytesRead = 0;

    ReadFile(handle, buffer, toRead, &bytesRead, NULL);
    
    return bytesRead;
}


bool Serial::write(const char *buffer, unsigned int num_bytes) {
    DWORD bytesSend;
    if (!WriteFile(handle, (void *)buffer, num_bytes, &bytesSend, 0)) {
        ClearCommError(handle, &errors, &status);
        return false;
    }
    return true;
}

int Serial::available() {
    return status.cbInQue;
}

bool Serial::is_connected() {
    return connected;
}