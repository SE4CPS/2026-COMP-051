#ifndef BUSSTOP_H
#define BUSSTOP_H

#include "Stop.h"

class BusStop : public Stop {
private:
    int bay;

public:
    BusStop(string n, double d, int b);
    void display() const override;
    string getType() const override;
    int getBay() const;
};

#endif
