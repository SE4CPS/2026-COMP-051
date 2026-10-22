#include "Stop.h"
#include <iostream>
using namespace std;

Stop::Stop(string n, double d) : name(n), distance(d) {}

void Stop::display() const {
    cout << name << " (" << distance << " mi)" << endl;
}

string Stop::getType() const {
    return "Stop";
}

string Stop::getName() const {
    return name;
}

double Stop::getDistance() const {
    return distance;
}
