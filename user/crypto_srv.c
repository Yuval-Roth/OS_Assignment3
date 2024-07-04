#include "kernel/types.h"
#include "user/user.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/fcntl.h"

#include "kernel/crypto.h"

int main(void) {


  if(getpid() != 2){
    exit(1);
  }

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  printf("crypto_srv: starting\n");

  // TODO: implement the cryptographic server here

  struct crypto_op *op;
    uchar *key = 0; 
    uchar *data = 0;
    uint64 size = 0;
    void *ptr = 0; // Ensure ptr is a valid pointer

    while(1) {
        if(take_shared_memory_request(&ptr, &size) < 0) {
            continue;
        }

        op = (struct crypto_op *)ptr; // Correctly cast ptr to struct crypto_op pointer

        if(op->state != CRYPTO_OP_STATE_INIT) {
            op->state = CRYPTO_OP_STATE_ERROR;
            goto update_ptr;
        }

        // Allocate memory for key and data
        key = malloc(op->key_size);
        data = malloc(op->data_size);
        if (key == 0 || data == 0) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        // Copy data and key
        memcpy(key, op->payload, op->key_size);
        memcpy(data, op->payload + op->key_size, op->data_size);

        // XOR encryption / decryption
        for(int i = 0; i < op->data_size; i++) {
            data[i] ^= key[i % op->key_size];
        }

        // Copy data back
        memcpy(op->payload + op->key_size, data, op->data_size);

        asm volatile("fence rw,rw" : : : "memory");

        op->state = CRYPTO_OP_STATE_DONE;

        update_ptr:
        memcpy(ptr, op, sizeof(struct crypto_op));
        remove_shared_memory_request(ptr, size);
        free(key);
        free(data);
    }

    exit(0);
}