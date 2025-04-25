#ifndef IPC_SETUP_H
#define IPC_SETUP_H

#include <semaphore.h>
#include <stddef.h> // For size_t

// Initialize the oven semaphore (create if not exists)
sem_t* init_oven_semaphore();

// Get an existing oven semaphore (no create)
sem_t* get_oven_semaphore();

// Initialize shared memory (create and map)
void* init_shared_memory(const char* name, size_t size);

// Attach to existing shared memory (read/write access)
void* attach_shared_memory(const char* name, size_t size);

// Unlink shared memory
void unlink_shared_memory(const char* name);

// Clean up both FIFO and semaphore (if needed)
void cleanup_ipc();

#endif
