#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = _ConsoleInput;
    OpenFileId output = _ConsoleOutput;
    char prompt[2], ch, buffer[100];
    int i;

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
        Write("Enter command: ", 15, output);

        i = 0;
        
        do {
        
            Read(&buffer[i], 1, input); 

        } while( buffer[i++] != '\n' );

        buffer[--i] = '\0';

        if( i > 0 ) {
            newProc = Exec(buffer);
            Join(newProc);
        }
        // PrintString("Available commands\n");
        // PrintString("Enter command prompt: ");
        // ReadString(buffer, 100);
        // newProc = Exec(buffer);
        // buffer[0] = '\0';
        // Join(newProc);
    }
}
