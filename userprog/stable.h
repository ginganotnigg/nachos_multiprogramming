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
    int Create(char* name, int init);
    int Wait(char* name);
    int Signal(char* name);
    int FindFreeSlot();
};

#endif