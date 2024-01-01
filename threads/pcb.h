#ifndef PCB_H
#define PCB_H

#include "string.h"
#include "thread.h"

class Semaphore;

class PCB {
private:
    Semaphore* joinsem; // semaphore for join process
    Semaphore* exitsem; // semaphore for exit process
    Semaphore* mutex;   // exclusive access semaphore
    int exitcode;
    int numwait;    // the number of join process
    Thread* thread;
    char fileName[32];
    
public:
    int parentID;   // The parent process’s ID
    PCB(int pid);   // Constructor
    PCB(int pid, char* fn);
    ~PCB();         // Destructor

    // Load the program has the name is “filename” and the process id is pid
    int Exec(char* name);   // Create a thread with the name is filename
    int ExecV(int argc, char** argv);

    int GetID(); // Return the PID of the current process
    int GetNumWait(); // Return the number of the waiting process
    
    void JoinWait(); // The parent process wait for the child process finishes
    void JoinRelease(); // The child process notice the parent process
    
    void ExitRelease(); // The parent process accept to exit thechild process
    void ExitWait(); // The child process finishes
    
    void IncNumWait(); // Increase the number of the waiting process
    void DecNumWait(); // Decrease the number of the waiting process
    
    void SetExitCode(int ec); // Set the exit code for the process
    int GetExitCode(); // Return the exitcode
    
    void SetFileName(char* fn); // Set the process name
    char* GetFileName(); // Return the process name

    void SetThread(Thread* thread) {
        this->thread = thread;
    }
};

#endif