#ifndef SEMAPHORE_UTILS_H
#define SEMAPHORE_UTILS_H

#include <sys/ipc.h>

// Create (or get) a semaphore with a given key
int create_semaphore(key_t key);

// Wait (P operation)
void sem_wait(int semid);

// Signal (V operation)
void sem_signal(int semid);

// Initialize semaphore to a given value (like 1)
void init_semaphore(int semid, int value);

#endif
