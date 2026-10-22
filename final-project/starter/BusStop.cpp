#include "BusStop.h"
#include <iostream>
using namespace std;

BusStop::BusStop(string n, double d, int b)
    : Stop(n, d), bay(b) {}

void BusStop::display() const {
    cout << name << " (" << distance << " mi) - Bus Bay "
         << bay << endl;
}

string BusStop::getType() const {
    return "Bus Stop";
}

int BusStop::getBay() const {
    return bay;
}
