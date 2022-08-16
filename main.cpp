#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "serial.h"
#include "timer.h"
#include "controller.h"

using namespace std;

string peek_token(string line) {
    return line.substr(0, line.find(' '));
}

string pop_token(string& line) {
    string token = peek_token(line);
    if (token.size() != line.size()) line = line.substr(token.size() + 1);
    else line = "";
    return token;
}

vector<string> split_to_tokens(string line) {
    vector<string> tokens;
    while (line.size() != 0) {
        tokens.push_back(pop_token(line));
        cout << tokens[tokens.size() - 1] << endl;
    }
    return tokens;
}

const char EOT = '~';
Serial* sp;

void send_to_arduino(string message) {
    message += ' ';
    message += EOT;
    sp->write((const char*)(&message[0]), message.size());
}

void send_to_arduino(State& state) {
    send_to_arduino("m1" + to_string(state.m1) + "m2" + to_string(state.m2) + "m3" + to_string(state.m3));
}

void apply(State state) {
    send_to_arduino("motor 0 target_position " + to_string(state.m1));
    cout << "motor 0 target_position " + to_string(state.m1) << endl;
    send_to_arduino("motor 1 target_position " + to_string(state.m2));
    cout << "motor 1 target_position " + to_string(state.m2) << endl;
    send_to_arduino("motor 2 target_position " + to_string(state.m3));
    cout << "motor 2 target_position " + to_string(state.m3) << endl;
}

void console_to_arduino() {
    while (sp->is_connected()) {
        string message = "";
        getline(cin, message);

        if (peek_token(message) == "reach") {
            pop_token(message);
            vector<string> tokens = split_to_tokens(message);
            if (tokens.size() == 2) {
                State target;
                target.reach(stof(tokens[0]), stof(tokens[1]));
                apply(target);
            }
            else if (tokens.size() == 3) {
                State target;
                target.reach(stof(tokens[0]), stof(tokens[1]), stof(tokens[2]));
                apply(target);
            }
        }

        send_to_arduino(message);
    }
}

void console_from_arduino() {

    cout << "\033[1;32m";

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
        cout << "Arduino> " << buf << endl;
        cout << "\033[1;32m";
	}
}

int main() {
    sp = new Serial("\\\\.\\COM3");
	if (sp->is_connected()) {
		cout << "We're connected!" << endl;
	}

	thread(&console_from_arduino).detach();
	console_to_arduino();
}