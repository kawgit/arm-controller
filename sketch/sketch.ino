typedef int32_t Timestamp;

const Timestamp TIME_MAX = 0x7FFFFFFF;
const char EOT = '~';
const int NUM_MOTORS = 3;
const bool DEBUG = false;
const Timestamp SIG_DEBOUNCE = 700;

struct Motor {
    float position = 0;
    float step_size = .03141592653;
    float target_position = 0;
    Timestamp time_of_last_edge = 0;
    bool is_sig_high = false;
};

Motor motors[NUM_MOTORS];

String buf = "";

String peek_token() {
    return buf.substring(0, buf.indexOf(' '));
}

String pop_token() {
    String token = peek_token();
    buf = buf.substring(token.length() + 1, buf.length());
    return token;
}

int tokens_available() {
    int cnt = 0;
    int index = 0;
    while (index < buf.length()) {
        index = buf.indexOf(' ', index) + 1;
        cnt++;
    }
    return cnt;
}

void send_message(String message) {
    Serial.print(message);
    Serial.print(EOT);
}

int dir_pin(int motor_number) {
    return motor_number * 2 + 2;
}

int sig_pin(int motor_number) {
    return motor_number * 2 + 1 + 2;
}

void setup() {
    Serial.begin(9600);

    for (int i = 0; i < NUM_MOTORS; i++) {
        pinMode(dir_pin(i), OUTPUT);
        pinMode(sig_pin(i), OUTPUT);
    }
}

Timestamp time_of_last_cycle = 0;

void loop() {

    //  to deal with micros() overflows occuring every 35 minutes
    if (micros() < time_of_last_cycle) {
        time_of_last_cycle -= TIME_MAX;
        for (int i = 0; i < NUM_MOTORS; i++) {
            motors[i].time_of_last_edge -= TIME_MAX;
        }
    }

    for (int i = 0; i < NUM_MOTORS; i++) {
        float displacement = motors[i].target_position - motors[i].position;
        if (!motors[i].is_sig_high && micros() - motors[i].time_of_last_edge > SIG_DEBOUNCE && abs(displacement) > motors[i].step_size / 2) {
            if (displacement < 0) {
                digitalWrite(dir_pin(i), HIGH);
                motors[i].position -= motors[i].step_size;
            }
            else {
                digitalWrite(dir_pin(i), LOW);
                motors[i].position += motors[i].step_size;
            }
            digitalWrite(sig_pin(i), HIGH);
            motors[i].is_sig_high = true;
            motors[i].time_of_last_edge = micros();
        }
    }

    for (int i = 0; i < NUM_MOTORS; i++) {
        if (motors[i].is_sig_high && micros() - motors[i].time_of_last_edge > SIG_DEBOUNCE) {
            digitalWrite(sig_pin(i), LOW);
            motors[i].is_sig_high = false;
            motors[i].time_of_last_edge = micros();
        }
    }

    if (Serial.available() > 0) {
        while (true) {
            while (Serial.available() < 1) {}
            char curr = Serial.read();
            if (curr == EOT) {
                break;
            }
            buf.concat(curr);
        }

        if (DEBUG) {
            send_message("Buffer: " + buf);
        }

        while (tokens_available() > 0) {
            String token = pop_token();
            if (token == "motor") {
                int motor_number = pop_token().toInt();
                while (tokens_available() > 0) {
                    String property = peek_token();
                    if (property == "position") {
                        pop_token();
                        motors[motor_number].position = pop_token().toFloat();
                    }
                    else if (property == "target_position") {
                        pop_token();
                        motors[motor_number].target_position = pop_token().toFloat();
                    }
                    else {
                        break;
                    }
                }
            }
            else if (token == "status") {
                Serial.print("\n");
                for (int i = 0; i < NUM_MOTORS; i++) {
                    Serial.println("Motor: " + String(i));
                    Serial.println("    position:        " + String(motors[i].position));
                    Serial.println("    target_position: " + String(motors[i].target_position));
                }
                Serial.print(EOT);
            }
        }
    }
}
