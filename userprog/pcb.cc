#include "pcb.h"
#include "synch.h"

PCB::PCB(int pid, char* fn) {
    this->processID = pid;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    mutex = new Semaphore("multex", 1);
    strcpy(this->filename, fn);
}
PCB::~PCB() {

    // if (filename != NULL) {
    //     delete filename;
    // }
    if (joinsem != NULL) delete joinsem;
    if (exitsem != NULL) delete exitsem;
    if (mutex != NULL) delete mutex; 
    joinsem = exitsem = mutex = NULL;
    if (thread != NULL) {
        if (kernel->currentThread->processID == thread->processID) {
            thread->FreeSpace();
            thread->Finish();
            thread = NULL;
        }
        //schedular will delete thread later
    }
}

void StartProcess_1(void* pid) {
    int id;
    id = *((int*)pid);
    char* fileName = kernel->pTab->GetFileName(id);
    AddrSpace* space;
    space = new AddrSpace(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }
    space->Execute();
    kernel->currentThread->space = space;

    // space->InitRegisters();	// set the initial register values
    // space->RestoreState();	// load page table register

    // kernel->machine->Run();	// jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

void StartProcess_2(void* argv) {
    char** arg= (char**)(argv);
    char* fileName = arg[0];
    AddrSpace* space;
    space = new AddrSpace(fileName);
    //space->Load(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }
    space->Execute();
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

int PCB::Exec(char* filename) {
    mutex->P();
    this->thread = new Thread(filename);
    this->thread->processID = processID;
    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        mutex->V();
        return -1;
    }
    this->thread->Fork(StartProcess_1, &(this->processID));

    mutex->V();
    return this->processID;
}


int PCB::ExecV(int argc, char** argv) {
    mutex->P();
    this->thread = new Thread(argv[0]);
    this->thread->processID = processID;
    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        mutex->V();
        return -1;
    }
    this->thread->Fork(StartProcess_2, argv);
    mutex->V();
    return this->processID;
}

char* PCB::GetFileName() {
    return filename;
}


void PCB::setThread(Thread* current) {
    this->thread = current;
}


void PCB::IncNumWait() {
    mutex->P();
    ++numwait;
    mutex->V();
}

void PCB::DecNumWait() {
    mutex->P();
    if (numwait > 0) --numwait;
    mutex->V();
}

void PCB::JoinWait() {
    joinsem->P();
}

void PCB::ExitWait() {
    exitsem->P();
}

void PCB::JoinRelease() {
    joinsem->V();
}

void PCB::ExitRelease() {
    exitsem->V();
}

void PCB::SetExitCode(int ec) { exitcode = ec; }

int PCB::GetExitCode() { return exitcode; }

int PCB::GetID() {return processID;}  