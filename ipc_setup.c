#include "ipc_setup.h"
#include "headers.h"
#include "constants.h"

// ✅ Create semaphore for oven with initial value 1
sem_t* init_oven_semaphore() {
    sem_t* sem = sem_open(OVEN_SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Failed to create oven semaphore");
        exit(EXIT_FAILURE);
    }
    return sem;
}

// ✅ Access existing semaphore (without creating)
sem_t* get_oven_semaphore() {
    sem_t* sem = sem_open(OVEN_SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("Failed to open existing oven semaphore");
        exit(EXIT_FAILURE);
    }
    return sem;
}

// ✅ Create and map shared memory (returns pointer)
void* init_shared_memory(const char* name, size_t size) {
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("Failed to create shared memory");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, size); // Set size
    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }
    return addr;
}

// ✅ Attach to existing shared memory
void* attach_shared_memory(const char* name, size_t size) {
    int fd = shm_open(name, O_RDWR, 0666);
    if (fd < 0) {
        perror("Failed to open shared memory");
        exit(EXIT_FAILURE);
    }
    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }
    return addr;
}

// ✅ Unlink shared memory by name
void unlink_shared_memory(const char* name) {
    if (shm_unlink(name) < 0) {
        perror("Failed to unlink shared memory");
    }
}

// ✅ Clean up semaphores and FIFOs (call once at the end)
void cleanup_ipc() {
    sem_unlink(OVEN_SEM_NAME);
    unlink(FIFO_CHEF_TO_BAKER);
    unlink(FIFO_BAKER_TO_SELLER);
    // Add more if needed
}
