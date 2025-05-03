#ifndef IPC_SETUP_H
#define IPC_SETUP_H
#include "constants.h"
#include <semaphore.h>
#include <stddef.h> // For size_t

// Named Semaphore for oven
#define OVEN_SEM_NAME "/oven_semaphore"

// Shared memory names (POSIX identifiers)
#define SHM_NAME_READY_ITEMS "/shm_ready_items"

// Oven semaphore
sem_t *init_oven_semaphore();
sem_t *get_oven_semaphore();

// Shared memory setup
void *init_shared_memory(const char *name, size_t size);
void *attach_shared_memory(const char *name, size_t size);
void unlink_shared_memory(const char *name);

// Final cleanup (semaphores + FIFOs + SHMs)
void cleanup_ipc();

#endif
