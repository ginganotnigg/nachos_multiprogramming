#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"

#define MAX_PROCESS 10

class PTable { 
private:
    int psize;
    Bitmap *bm;             // mark the locations that have been used in pcb
    PCB* pcb[MAX_PROCESS];
    Semaphore* bmsem;       // used to prevent the case of loading 2 processes at the same time

public:
    PTable(int size);   // Constructor
    PTable();
    ~PTable();          // Destructor

    int ExecUpdate(char* name); // Process the syscall SC_Exec
    int ExecV(int argc, char**argv);

    void ExitUpdate(int ec); // Process the syscall SC_Exit
    int JoinUpdate(int id); // Process the syscall SC_Join

    int GetFreeSlot(); // Find the free slot in PTable to save the new process information
    bool IsExist(int pid); // Check a process exist or not
    void Remove(int pid); // Delete the PID from the PTable
    char* GetFileName(int id); // Return the process name
};

#endif