#include "user.h"

int main() {
    // alocate memory in parent process
    void* memory = malloc(1000); 
    int ppid = getpid();
    
    if(fork() == 0) {
        char* ptr;
        if((ptr = (char*) map_shared_pages(ppid,getpid(), (uint64)memory, 20)) < 0){
            printf("Error in mapping shared pages\n");
            exit(0);
        }
        printf("Child: %s\n", ptr);
        exit(0);
    } else {
        strcpy((char*)memory, "Hello child");
        wait(0);
    }

    return 0;
}