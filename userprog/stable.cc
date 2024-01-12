#include "synch.h"
#include "stable.h"

STable::STable() {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        this->sem[i]= NULL;
    }
}

STable::~STable() {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (this->sem[i]) {
            delete this->sem[i];
            this->sem[i] = NULL;
        }
    }
}
int STable::Create(char* name, int init) {
    for (int i=0; i< MAX_SEMAPHORE; i++) {
        if(sem[i]!=NULL) {
            if (strcmp(sem[i]->GetName(),name) ==0) {
                return -1;
            }
        }
    }
    int id = this->FindFreeSlot();
    if (id <0) return -1;
    this->sem[id] = new Sem(name,init);
    return 0;
}

int STable::Wait(char* name) {
    int i;
    for (i=0; i< MAX_SEMAPHORE; i++) {
        if(sem[i]!=NULL) {
            if (strcmp(sem[i]->GetName(),name) ==0) {
                sem[i]->Wait();
                return 0;
            }
        }
    }
    return -1;
}
int STable::Signal(char* name){
     int i;
    for (i=0; i< MAX_SEMAPHORE; i++) {
        if(sem[i]!=NULL) {
            if (strcmp(sem[i]->GetName(),name) ==0) {
                sem[i]->Signal();
                return 0;
            }
        }
    }
    return -1;
}
int STable::FindFreeSlot(){
    for (int i=0; i< MAX_SEMAPHORE; i++) {
        if (sem[i] == NULL) return i;
    }
    return -1;
}