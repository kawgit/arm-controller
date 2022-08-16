#pragma once

const float UPPER_ARM_LENGTH = .5; // elbow to shoulder
const float LOWER_ARM_LENGTH = .5; // wrist to elbow
const float UPPER_ELBOW_CONTROLL_BEAM_LENGTH = .0869;
const float LOWER_ELBOW_CONTROLL_BEAM_LENGTH = .08;
const float M2_DISPLACEMENT = .09;

struct State {
    float m1 = 0;   // X-axis/shoulder
    float m2 = 0;   // X-axis/elbow
    float m3 = 0;   // Y-axis/waist
    
    bool reach(float x, float y, float z);
    bool reach(float x, float y);
};