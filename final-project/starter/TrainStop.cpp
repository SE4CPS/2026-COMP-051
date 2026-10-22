#include "TrainStop.h"
#include <iostream>
using namespace std;

TrainStop::TrainStop(string n, double d, int p)
    : Stop(n, d), platform(p) {}

void TrainStop::display() const {
    cout << name << " (" << distance << " mi) - Platform "
         << platform << endl;
}

string TrainStop::getType() const {
    return "Train Stop";
}

int TrainStop::getPlatform() const {
    return platform;
}
