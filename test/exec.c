#include "syscall.h" 
#define maxlen 32 
int main() 
{   int i = 4;
    int pid;
    while (--i) {
        char command[maxlen];
        PrintString("Enter command (cat, copy, createfile, halt, ...): ");
        ReadString(command, maxlen);
        pid = Exec(command);
        if (pid < 0) {
            PrintString("Failed!\n");
        } 
        else Join(pid);
    } 
    
}