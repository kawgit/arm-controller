#include "controller.h"
#include <cmath>

using namespace std;

float law_of_cosines_A(float a, float b, float c) {
    return acos((a*a - b*b - c*c) / (-2 * b * c));
}

float hypotenuse(float a, float b) {
    return sqrt(a*a + b*b);
}

bool State::reach(float x, float y, float z) {
    m3 = atan(z/x);
    float adjusted_x = sqrt(x*x + z*z);
    return reach(adjusted_x, y);
}

bool State::reach(float x, float y) {
    float distance_to_target = hypotenuse(x, y);

    if (distance_to_target > UPPER_ARM_LENGTH + LOWER_ARM_LENGTH) {
        reach((x / distance_to_target * (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH)) * .99, (y / distance_to_target * (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH)) * .99);
        return false;
    }

    float E = atan(y/x);
    float C = law_of_cosines_A(LOWER_ARM_LENGTH, UPPER_ARM_LENGTH, distance_to_target);
    float A = law_of_cosines_A(distance_to_target, LOWER_ARM_LENGTH, UPPER_ARM_LENGTH);
    m1 = C + E;
    float p1x = UPPER_ARM_LENGTH * cos(C + E);
    float p1y = UPPER_ARM_LENGTH * sin(C + E);
    float p2x = p1x + LOWER_ELBOW_CONTROLL_BEAM_LENGTH * cos(C + E + A);
    float p2y = p1y + LOWER_ELBOW_CONTROLL_BEAM_LENGTH * sin(C + E + A);
    float distance_m2_to_p2 = hypotenuse(p2x + M2_DISPLACEMENT, p2y);

    if (distance_m2_to_p2 > UPPER_ARM_LENGTH + UPPER_ELBOW_CONTROLL_BEAM_LENGTH) {
        return false;
    }

    m2 = acos((p2x+M2_DISPLACEMENT)/distance_m2_to_p2)+law_of_cosines_A(UPPER_ARM_LENGTH, UPPER_ELBOW_CONTROLL_BEAM_LENGTH, distance_m2_to_p2);

    return true;
}
