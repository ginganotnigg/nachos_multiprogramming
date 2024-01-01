#ifndef STABLE_H
#define STABLE_H

#include "sem.h"
#define MAX_SEMAPHORE 10

class STable {
private:
    Sem* sem[MAX_SEMAPHORE];
public:
    STable();
    ~STable();
    // Check the semaphore name, create a new one if not already
    int Create(char *name, int init);
    // If the semaphore name already exists, call this->P() to execute it.
    // If not, report an error in Wait, Signal function
    int Wait(char *name);
    int Signal(char* name);
    int FindFreeSlot();
};

#endif