
#ifndef PCB_H
#define PCB_H
#include "string.h"

//quản lý thông tin một tiến trình
class Semaphore;
class Thread;

class PCB {
private:
    Semaphore* joinsem; //sempahore join
    Semaphore* exitsem; //semaphore exit
    Semaphore* mutex;
    int exitcode;
    int numwait;
    int processID;
    
    char filename[128];
    Thread *thread;
public:
    int parentID;
    PCB();
    PCB(int pid, char* fn);
    ~PCB();

    int Exec(char* name);
    int ExecV(int argc, char** argv);
    int GetID();
    int GetNumWait();

    void JoinWait();
    void ExitWait();
    void JoinRelease();
    void ExitRelease();

    void IncNumWait();
    void DecNumWait();
    
    void SetExitCode(int exitCode);
    int GetExitCode();

    char *GetFileName();
    void setThread(Thread* current);
};

#endif