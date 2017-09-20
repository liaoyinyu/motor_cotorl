#include <stdlib.h>
#include <conio.h>
#include "bujindianji.h"

void main(void)
{
    int i;
    outp(MY8255_MODE,0x81);
    while(1)
    {
        for(i=0;i<300;i++)
        {
            keyget();
            ADget();
            delay(0x05);
        }
        dianjizhuandong();
    }
}



