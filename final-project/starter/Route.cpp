#include "Route.h"
#include <cmath>
#include <iostream>
using namespace std;

Route::Route() {}

Route::~Route() {
    for (Stop* s : stops) {
        delete s;
    }
}

void Route::addStop(Stop* s) {
    stops.push_back(s);
}

int Route::indexOf(const string& name) const {
    for (size_t i = 0; i < stops.size(); i++) {
        if (stops[i]->getName() == name) {
            return (int)i;
        }
    }
    return -1;
}

bool Route::isValidStop(const string& name) const {
    return indexOf(name) != -1;
}

Stop* Route::nextStop(const string& currentName) const {
    int idx = indexOf(currentName);
    if (idx == -1 || idx + 1 >= (int)stops.size()) {
        return nullptr;
    }
    return stops[idx + 1];
}

bool Route::isRushHour(int hour) {
    return (hour >= 7 && hour < 9) ||
           (hour >= 16 && hour < 18);
}

double Route::totalDistance(const string& startName,
                             const string& destName) const {
    int startIdx = indexOf(startName);
    int destIdx = indexOf(destName);
    if (startIdx == -1 || destIdx == -1) {
        return -1.0;
    }
    return fabs(stops[destIdx]->getDistance() -
                stops[startIdx]->getDistance());
}

vector<double> Route::buildCumulativeDistances() const {
    vector<double> cumulative;
    double total = 0.0;
    for (size_t i = 0; i < stops.size(); i++) {
        if (i > 0) {
            total += fabs(stops[i]->getDistance() -
                           stops[i - 1]->getDistance());
        }
        cumulative.push_back(total);
    }
    return cumulative;
}

void Route::reverseRoute() {
    for (size_t i = 0; i < stops.size() / 2; i++) {
        Stop* temp = stops[i];
        stops[i] = stops[stops.size() - 1 - i];
        stops[stops.size() - 1 - i] = temp;
    }
}

void Route::printCountdown(const string& currentName,
                            const string& destName) const {
    int curIdx = indexOf(currentName);
    int destIdx = indexOf(destName);
    if (curIdx == -1 || destIdx == -1) {
        cout << "Unknown stop." << endl;
        return;
    }
    cout << abs(destIdx - curIdx) << " stop(s) to go."
         << endl;
}

bool Route::loadFromFile(const string& filename) {
    // TODO (student): open "filename", read it line by
    // line in the format TYPE,name,distance,extra (TYPE
    // is BUS or TRAIN), and addStop() a new BusStop or
    // TrainStop for each line. Return true once the file
    // loads successfully, false if it can't be opened.
    return false;
}

void Route::displayAll() const {
    for (Stop* s : stops) {
        s->display();
    }
}
