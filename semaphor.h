#ifndef SEMAPHOR_UTILS_H
#define SEMAPHOR_UTILS_H

#include <sys/ipc.h>

// Create a System V semaphore with the specified key
int create_semaphore(key_t key);

// Initialize the semaphore to the specified initial value
void init_semaphore(int semid, int value);

// Wait (P operation) - acquire/lock the semaphore
void sem_wait(int semid);

// Signal (V operation) - release/unlock the semaphore
void sem_signal(int semid);

#endif // SEMAPHOR_UTILS_H
