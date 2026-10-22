#ifndef STOP_H
#define STOP_H

#include <string>
using namespace std;

class Stop {
protected:
    string name;
    double distance;  // miles from the route's first stop

public:
    Stop(string n, double d);
    virtual ~Stop() {}
    virtual void display() const;
    virtual string getType() const;
    string getName() const;
    double getDistance() const;
};

#endif
