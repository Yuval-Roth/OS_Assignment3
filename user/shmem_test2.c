#include "user.h"

#define PGSIZE 4096
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))


int main() {
    // alocate memory in parent process
    printf("Parent before malloc: %d\n", memsize());
    char* memory = malloc(1000); 
    printf("Parent after malloc: %d\n", memsize());
    int ppid = getpid();
    
    if(fork() == 0) {
        printf("Child before mapping: %d\n", memsize());
        char* ptr;
        if((ptr = (char*) map_shared_pages(ppid,getpid(), (uint64)memory, 20)) < 0){
            printf("Error in mapping shared pages\n");
            exit(0);
        }
        // write to shared memory
        strcpy(ptr, "Hello daddy");

        printf("Child after mapping: %d\n", memsize());
        unmap_shared_pages(getpid(), (uint64)ptr, 20);
        printf("Child after unmapping: %d\n", memsize());
        malloc(20*4096);
        printf("Child after malloc: %d\n", memsize()); 

    } else {
        wait(0);
        printf("Parent: %s\n", memory);
        
    }

    return 0;
}