#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_FRAME_SIZE 1024 //1024 bytes per page
#define FRAME_COUNT 64 //64 frames at one time in the physical memory
#define PAGE_COUNT 1024 //1024 pages in virtual memory, inside backing store aswell.
#define READ_BUFF_SIZE 12 //amount to read from input document

#define OFFSET_PAGE_LEN_BITS 10 //10 bits of the adress is offset and 10 is page number
#define OFFSET_MASK 1023 //not 1024 because we start counting from 0 (11 1111 1111)


int pagetable[PAGE_COUNT]; //pagetable is as large as the amount of pages
unsigned char physical_memory[FRAME_COUNT * PAGE_FRAME_SIZE]; //physical memory is made out of the number of frames of size 1024
char read_buff[READ_BUFF_SIZE]; //the buffer the lines from the virtual adress file are read into
bool occupied_frames_physical[FRAME_COUNT]; //this array is used to keep a log of the used frames inside the physical memory

int check_physical_for_place(){ //check for empry frame in the physical memory
    for (int i = 0; i < FRAME_COUNT; ++i){ //loop thru the whole boolean array
        if(occupied_frames_physical[i] == 0){ //if there is an empty frame
            occupied_frames_physical[i] = 1; //set this frame to occupied
            return i; //return the frame number
        }
    }
    return -1; //if no empty frame is found, return -1
}

int main(int argc, char *argv[]){
    //if there is no input argument
    if (argc != 3)
        printf("USAGE: ./simulated-memory-management [virtual adresses] [backing store]");
    int virtual_adress, physical_adress, page_number, frame_number, offset, val_pos;
    double total_adresses_in, page_faults;
    float pf_ratio;
    unsigned char val;//define variables
    FILE *va_fp = fopen(argv[1], "r");//file pointer to the virtual adress file (simulated process)
    int bs_fp = open(argv[2], O_RDONLY);//need to have a file descriptor for the memory mapping function
    unsigned char *mapptr = mmap(0, PAGE_COUNT*PAGE_FRAME_SIZE, PROT_READ, MAP_PRIVATE, bs_fp, 0); //mapping the backing-store to make it into an acessible "array"
    //mapptr has to be unsigned char to keep right values in the physical_memory array
    if (mapptr == MAP_FAILED){ //if the mapping of the secondary memory failed
        printf("Could not map secondary storage!\n");
        return 1;
    }
    
    memset(pagetable, -1, sizeof(pagetable));//set all entries in the pagetable to -1 to represent "empty"
    memset(occupied_frames_physical, 0, FRAME_COUNT);//set all entries in occupied_frames_physical to 0 to represent "free"

    while(fgets(read_buff, READ_BUFF_SIZE, va_fp) != NULL){//loop for reading each row of the virtual adress file

        virtual_adress= atoi(read_buff);// 1. input adress to int
        offset = virtual_adress & OFFSET_MASK;// 2. getting the offset thru the AND operator
        page_number = virtual_adress >> OFFSET_PAGE_LEN_BITS & OFFSET_MASK;// 3. shifting right so the page number bits are the LSB and 4. same operation as 2 but now to get the page number

        total_adresses_in++; //adress has been read

        if(pagetable[page_number] != -1){ //if there is a frame number associated with this page
            frame_number = pagetable[page_number]; //use this frame_number
        }
        else if (pagetable[page_number] == -1){ //page fault
            page_faults++; //add to page fault counter
            frame_number = check_physical_for_place();
            if (frame_number == -1){ // if there is no available palce in the physical memory
                printf("No free frames to copy page into from backing store!\n");
                return 1;
            }
            //update pagetable
            pagetable[page_number] = frame_number;
            //copying page into physical memory from secondary
            memcpy(physical_memory + frame_number*PAGE_FRAME_SIZE, mapptr + page_number*PAGE_FRAME_SIZE, PAGE_FRAME_SIZE);
            //physical_memory(starting adress of physical memory) + frame_number*PAGE_FRAME_SIZE = at what adress we want to start pasting in a page.
            //mapptr(starting adress of backing store) + page_number*PAGE_FRAME_SIZE = from what adress we start copying. 
            //obviously we use PAGE_FRAME_SIZE as the "step amount" to keep the structure and also as copy amount
        }
        else {
            printf("Error in the values of the pagetable!\n");
            return 1;
        }

        //get value at adress
        val_pos = ((frame_number * PAGE_FRAME_SIZE) + offset);
        val = physical_memory[val_pos];
        //we move the bits to the left, to leave space for the offset to be added back, then i add it back with the OR operator.
        physical_adress = (frame_number << OFFSET_PAGE_LEN_BITS) | offset;

        printf("Virtual adress: %d | Page number: %d | Offset: %d | Value: %d | Physical adress:%d\n", 
            virtual_adress, page_number, offset, val, physical_adress);
        printf("--------------------------------\n");

    }
    pf_ratio = page_faults/total_adresses_in;
    printf("Total: %f | Faults: %f\n", total_adresses_in, page_faults);
    printf("Page fault rate: %f%%\n", pf_ratio*100);
}