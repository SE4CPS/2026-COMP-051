// CampusNav: single-file version for an online compiler.
// Paste this whole file into OnlineGDB (C++) and click Run.
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Stop {
protected:
    string name;
    double distance;  // miles from the route's first stop

public:
    Stop(string n, double d) : name(n), distance(d) {}
    virtual ~Stop() {}

    virtual void display() const {
        cout << name << " (" << distance << " mi)" << endl;
    }
    virtual string getType() const {
        return "Stop";
    }
    string getName() const {
        return name;
    }
    double getDistance() const {
        return distance;
    }
};

class BusStop : public Stop {
private:
    int bay;

public:
    BusStop(string n, double d, int b) : Stop(n, d), bay(b) {}

    void display() const override {
        cout << name << " (" << distance << " mi) - Bus Bay "
             << bay << endl;
    }
    string getType() const override {
        return "Bus Stop";
    }
    int getBay() const {
        return bay;
    }
};

class TrainStop : public Stop {
private:
    int platform;

public:
    TrainStop(string n, double d, int p)
        : Stop(n, d), platform(p) {}

    void display() const override {
        cout << name << " (" << distance << " mi) - Platform "
             << platform << endl;
    }
    string getType() const override {
        return "Train Stop";
    }
    int getPlatform() const {
        return platform;
    }
};

class Route {
private:
    vector<Stop*> stops;

public:
    Route() {}
    ~Route() {
        for (Stop* s : stops) {
            delete s;
        }
    }

    void addStop(Stop* s) {
        stops.push_back(s);
    }

    int indexOf(const string& name) const {
        for (size_t i = 0; i < stops.size(); i++) {
            if (stops[i]->getName() == name) {
                return (int)i;
            }
        }
        return -1;
    }

    bool isValidStop(const string& name) const {
        return indexOf(name) != -1;
    }

    Stop* nextStop(const string& currentName) const {
        int idx = indexOf(currentName);
        if (idx == -1 || idx + 1 >= (int)stops.size()) {
            return nullptr;
        }
        return stops[idx + 1];
    }

    static bool isRushHour(int hour) {
        return (hour >= 7 && hour < 9) ||
               (hour >= 16 && hour < 18);
    }

    double totalDistance(const string& startName,
                          const string& destName) const {
        int startIdx = indexOf(startName);
        int destIdx = indexOf(destName);
        if (startIdx == -1 || destIdx == -1) {
            return -1.0;
        }
        return fabs(stops[destIdx]->getDistance() -
                    stops[startIdx]->getDistance());
    }

    vector<double> buildCumulativeDistances() const {
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

    void reverseRoute() {
        for (size_t i = 0; i < stops.size() / 2; i++) {
            Stop* temp = stops[i];
            stops[i] = stops[stops.size() - 1 - i];
            stops[stops.size() - 1 - i] = temp;
        }
    }

    void printCountdown(const string& currentName,
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

    bool loadFromFile(const string& filename) {
        // TODO (student): open "filename", read it line by
        // line in the format TYPE,name,distance,extra (TYPE
        // is BUS or TRAIN), and addStop() a new BusStop or
        // TrainStop for each line. Return true once the file
        // loads successfully, false if it can't be opened.
        return false;
    }

    void displayAll() const {
        for (Stop* s : stops) {
            s->display();
        }
    }
};

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
