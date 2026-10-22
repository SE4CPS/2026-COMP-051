#ifndef TRAINSTOP_H
#define TRAINSTOP_H

#include "Stop.h"

class TrainStop : public Stop {
private:
    int platform;

public:
    TrainStop(string n, double d, int p);
    void display() const override;
    string getType() const override;
    int getPlatform() const;
};

#endif
