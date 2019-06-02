
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/io.h>

#define R1_ON   0x1
#define R2_ON   0x2
#define R3_ON   0x4
#define R4_ON   0x8
#define R5_ON   0x10
#define R6_ON   0x20
#define R7_ON   0x40
#define R8_ON   0x80

#define BASE    0x300

#define GPIO0_DIR   0x98
#define GPIO1_DIR   0x99

#define GPIO0_BASE  0x78
#define GPIO1_BASE  0x79

static const uint8_t rltbl[8] = {
    R1_ON, R2_ON, R3_ON, R4_ON, R5_ON, R6_ON, R7_ON, R8_ON
};

void print_usage()
{
    printf("Usage: switch [id] [on|off]\n");
}

int main(int argc, char *argv[])
{
    int r;                      /* return value storage */
    uint8_t state = 0;          /* previous output state storage */
    uint8_t newstate = 0;       /* proposed output state storage */
    uint8_t ob = 0;             /* output relay bit index */

    if (argc < 2)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        print_usage();
        exit(-1);
    }

    r = setuid(0);
    if (r)
    {
        perror("Unable to set uid to 0");
        exit(-1);
    }

    r = ioperm(BASE, 10, 1);                                                       
    if (r)                                                                         
    {                                                                              
        perror("Unable to obtain I/O permissions");                                
        exit(-1);
    }                                                                              

    if (strncasecmp(argv[1], "-s", 2) == 0)                                        
    {                                                                              
        int i;                                                                     

        state = inb(BASE);

        for (i = 0; i < 8; i++)
        {
            printf("Out%i state: %u\n", i + 1, state & rltbl[i] ? 1 : 0); 
        }
        return 0;                                                                  
    }                                                                              

    ob = atoi(argv[1]) - 1;

    if (ob < 0 || ob > 7)
    {
        fprintf(stderr, "id should be integer between 1 and 8\n");
        exit(-1);
    }

    state = inb(BASE);

    if (strncasecmp(argv[2], "on", 2) == 0)
    {
        newstate = state | rltbl[ob];
        outb(newstate, BASE);
        exit(0);
    } 
    
    else if (strncasecmp(argv[2], "off", 3) == 0) 
    {
        newstate = state & ~rltbl[ob];
        outb(newstate, BASE);
        exit(0);
    }
    
    else 
    {
        fprintf(stderr, "Action should be \"on\" or \"off\", but you req"
                "uested: %s\n", argv[2]);
        exit(-1);
    }

    printf("0x%.2X\n", rltbl[ob]);

    return 0;
}
