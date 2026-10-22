#ifndef ROUTE_H
#define ROUTE_H

#include "Stop.h"
#include <string>
#include <vector>
using namespace std;

class Route {
private:
    vector<Stop*> stops;

public:
    Route();
    ~Route();

    void addStop(Stop* s);
    int indexOf(const string& name) const;
    bool isValidStop(const string& name) const;
    Stop* nextStop(const string& currentName) const;
    static bool isRushHour(int hour);
    double totalDistance(const string& startName,
                          const string& destName) const;
    vector<double> buildCumulativeDistances() const;
    void reverseRoute();
    void printCountdown(const string& currentName,
                         const string& destName) const;
    bool loadFromFile(const string& filename);
    void displayAll() const;
};

#endif
