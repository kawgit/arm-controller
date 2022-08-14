#include <iostream>
#include <string>
#include <thread>
#include "serial.h"
#include "timer.h"

using namespace std;

const char EOT = '~';

void print_incoming(Serial* sp) {

    cout << "\033[1;32m";
    cout << "Arduino< ";

    const int buf_size = 256;

	while (sp->is_connected())
	{
        string buf = "";
        int index = 0;

		while (true) {
            char incoming_byte = 0;
            if (sp->read(&incoming_byte, 1) > 0) {
                if (incoming_byte == EOT) {
                    break;
                }
                buf += incoming_byte;
            }
        }

        cout << "\033[0m";
        cout << "\nArduino> " << buf << endl;
        cout << "\033[1;32m";
        cout << "\nArduino< ";
	}
}

int main() {
    Serial* sp = new Serial("\\\\.\\COM3");
	if (sp->is_connected()) {
		cout << "We're connected!" << endl;
	}

	thread print_thread(&print_incoming, ref(sp));
    print_thread.detach();

    while (sp->is_connected()) {
        string message = "";
        getline(cin, message);
        message += EOT;
        sp->write((const char*)(&message[0]), message.size());
    }
}