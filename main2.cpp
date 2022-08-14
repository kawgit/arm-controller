#include <iostream>
#include "controller.h"

using namespace std;

int main() {
    State state;
    state.reach(1000, 50);
    cout << state.m1 << " " << state.m2 << " " << state.m3 << " " << endl;
}