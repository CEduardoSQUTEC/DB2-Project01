
#ifndef RECORD_H
#define RECORD_H

#include "../libs.h"

// Record
class Record {
public:
    long next;
protected:
    virtual void serialization(string object) = 0;

    virtual void print() = 0; // print Record Data
    virtual void input() = 0; // initialize from input
};

#endif