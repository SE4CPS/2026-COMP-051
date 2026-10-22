#include "Stop.h"
#include "BusStop.h"
#include "TrainStop.h"
#include "Route.h"
#include <iostream>
using namespace std;

int main() {
    Route route;

    if (!route.loadFromFile("route.txt")) {
        cout << "route.txt not loaded, using default route."
             << endl;
        route.addStop(new BusStop(
            "University Transit Center", 0.0, 3));
        route.addStop(new TrainStop(
            "North Quad Station", 0.8, 1));
        route.addStop(new BusStop("Library Plaza", 1.5, 1));
        route.addStop(new TrainStop(
            "Downtown Junction", 2.9, 2));
        route.addStop(new BusStop("Riverside Mall", 4.1, 4));
        route.addStop(new TrainStop(
            "East Hills Terminal", 5.6, 3));
    }

    cout << "=== CampusNav Route ===" << endl;
    route.displayAll();

    string start, dest;
    int hour;

    cout << "\nStarting stop: ";
    getline(cin, start);
    cout << "Destination stop: ";
    getline(cin, dest);
    cout << "Current hour (0-23): ";
    cin >> hour;

    if (!route.isValidStop(start) || !route.isValidStop(dest)) {
        cout << "One of those stops isn't on this route."
             << endl;
        return 1;
    }

    if (route.indexOf(dest) < route.indexOf(start)) {
        route.reverseRoute();
    }

    bool rush = Route::isRushHour(hour);
    if (rush) {
        cout << "\nRUSH HOUR: expect delays." << endl;
    }

    cout << "\nDeparting " << start << "..." << endl;
    string current = start;
    while (current != dest) {
        route.printCountdown(current, dest);
        Stop* next = route.nextStop(current);
        current = next->getName();
        cout << "Arrived at " << current << endl;
    }

    double dist = route.totalDistance(start, dest);
    cout << "\nTotal distance: " << dist << " mi" << endl;
    if (rush) {
        cout << "Estimated time (rush hour): " << dist * 4
             << " min" << endl;
    } else {
        cout << "Estimated time: " << dist * 2.5 << " min"
             << endl;
    }

    return 0;
}
