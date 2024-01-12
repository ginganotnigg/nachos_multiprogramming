#include "syscall.h"
#define maxLength 32

int main()
{
    char buffer[256];
    int socket1, socket2, socket3, socket4;

    PrintString("\nSocket 1:\n");
    socket1 = SocketTCP();
    Connect(socket1, "127.0.0.1", 1234);
    Send(socket1, "This is a message sent from socket 1", 40);
    if (Receive(socket1, buffer, 40) > 0)
    {
        PrintString("Message: ");
        PrintString(buffer);
    }

    PrintString("\n\nSocket 2:\n");
    socket2 = SocketTCP();
    Connect(socket2, "127.0.0.1", 1234);
    Send(socket2, "This is a message sent from socket 2", 40);
    if (Receive(socket2, buffer, 40) > 0)
    {
        PrintString("Message: ");
        PrintString(buffer);
    }

    PrintString("\n\nSocket 3:\n");
    socket3 = SocketTCP();
    Connect(socket3, "127.0.0.1", 1234);
    Send(socket3, "This is a message sent from socket 3", 40);
    if (Receive(socket3, buffer, 40) > 0)
    {
        PrintString("Message: ");
        PrintString(buffer);
    }
    
    PrintString("\n\nClose...\n");
    Close(socket1);
    Close(socket2);
    Close(socket3);

    Halt();
}