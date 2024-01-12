#include "ptable.h"
#include "synch.h"

void PTable::Remove(int id) {
    if (pcb[id] != NULL) {
        PCB* point = pcb[id];
        pcb[id] = NULL;
        delete point;
    }
}

PTable::PTable() {
    char *name = new char[12];
    strcpy(name, "mainthread");
    psize = MAX_PROCESS;
    for (int i = 0; i < psize; i++) {
        pcb[i] = NULL;
    }
    bmsem = new Semaphore("bmsem", 1);
    pcb[0] = new PCB(0,name);
    if (kernel->currentThread->processID == 0) {
        pcb[0]->setThread(kernel->currentThread);    
    }
    pcb[0]->parentID = -1;
    delete []name;
}

PTable::~PTable() {
    for (int i = 0; i < psize; i++) {
        if (!pcb[i]) delete pcb[i];
    }
    delete bmsem;
}
int PTable::GetFreeSlot() {
    for (int i =0; i < psize; i++) {
        if (pcb[i] == NULL) return i;
    }
    return -1;
}

char* PTable::GetFileName(int id) { return pcb[id]->GetFileName(); }

int PTable::ExecUpdate(char* name) {
    bmsem->P();

    if (name == NULL) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute name is NULL.\n");
        bmsem->V();
        return -1;
    }
    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute itself.\n");
        bmsem->V();
        return -1;
    }

    int index = this->GetFreeSlot();

    if (index < 0) {
        DEBUG(dbgSys, "\nPTable::Exec :There is no free slot.\n");
        bmsem->V();
        return -1;
    }
 
    pcb[index] = new PCB(index, name);

    pcb[index]->parentID = kernel->currentThread->processID;

    int pid = pcb[index]->Exec(name);

    bmsem->V();
    return pid;
}

int PTable::JoinUpdate(int id) {
    if (id < 0 || id >= psize || pcb[id] == NULL ||
        pcb[id]->parentID != kernel->currentThread->processID) {
        DEBUG(dbgSys, "\nPTable::Join : Can't not join.\n");
        return -1;
    }

    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();

    int exitcode = pcb[id]->GetExitCode();
    cout << endl << "Exitcode: "<< exitcode << endl;
    pcb[id]->ExitRelease();

    return exitcode;
}

void PTable::ExitUpdate(int exitcode) {
    int id = kernel->currentThread->processID;
    if (id == 0) {
        cout <<"Main Exitcode: " << exitcode << endl;
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
        //return 0;
    }

    if (pcb[id]== NULL) {
        DEBUG(dbgSys, "Process " << id << " is not exist.");
        //return -1;
    }
    pcb[id]->SetExitCode(exitcode);
    pcb[pcb[id]->parentID]->DecNumWait();

    
    pcb[id]->JoinRelease();
    pcb[id]->ExitWait();
    Remove(id);
}

int PTable::ExecV(int argc,char**argv) {
}