#include "types.h"

#include "gdt.h"

// Simple print fonction using 0xB8000 adress of processor to print some stuff on screen for test
void printf(char * str) {
    static uint16_t * VideoMemory = (uint16_t *) 0xB8000;

    static uint8_t x = 0, y = 0;



    for (int i = 0 ; str[i] != '\0'; ++i ) {
        switch(str[i])
        {
        case '\n':
            y++;
            x=0;
            break;
        default:
            VideoMemory[80*y+x]= ( VideoMemory[80*y+x] & 0xFF00) | str[i];
            // replacing a part of the binary to print char, and not change the background
            x++;

        }
        if (x>= 80) //if x reach end of line, new line
        {
            y++;
            x=0;
        }

        if (y>=25)
        {
            for(y=0; y<25; y++)
                for (x=0; x<80; x++)
                    VideoMemory[80*y+x]= ( VideoMemory[80*y+x] & 0xFF00) | ' ';
            x=0;
            y=0;

        }

    }


}



// initialize constructor for kernel
typedef void (*constructor)();
extern "C" constructor* start_ctors;
extern "C" constructor* end_ctors;
extern "C" void callConstructors() {

    for (constructor* &i = start_ctors; i != end_ctors; i++)
        (*i)();

}


// extern to be linked in assembly file
// print some stuff on screen to test, then loop
// Have some information from bootloader in multiboot_structure and magicnumber that we keep
extern "C" void kernelMain(void * multiboot_structure, uint32_t magicnumber) {

    printf("\n");
    printf("UwU !!\n");
    printf("OwO !!\n");

    GlobalDescriptorTable gdt;

    while(1);

}
