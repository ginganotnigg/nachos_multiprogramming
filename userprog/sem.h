#ifndef SEM_H
#define SEM_H
#include "string.h"
class Semaphore;

class Sem {
private:
    Semaphore* sem;
    char name[50];
public:
    Sem(char* name, int i);
    ~Sem();
    void Wait();
    void Signal();
    char* GetName();
};


#endif