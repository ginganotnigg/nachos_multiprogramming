#ifndef SEM_H
#define SEM_H
#include "string.h"

class Semaphore;

class Sem {    
private:
    char name[50];      // The semaphore nameSemaphore* sem; // Create semaphore for management
    Semaphore* sem;     // Create semaphore for management

public:
    Sem(char* name, int i);
    ~Sem();
    void Wait();
    void Signal();
    char* GetName();
};

#endif