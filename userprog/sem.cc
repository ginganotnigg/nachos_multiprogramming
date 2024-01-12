#include "sem.h"
#include "synch.h"

Sem::Sem(char* name, int initial){
    strcpy(this->name,name);
    sem = new Semaphore(this->name, initial);
}

Sem::~Sem(){
    if ( sem != NULL) {
        delete sem;
    }
}

char* Sem::GetName() {
    return name;
}

void Sem::Wait() {
    sem->P();
}
void Sem::Signal() {
    sem->V();
}