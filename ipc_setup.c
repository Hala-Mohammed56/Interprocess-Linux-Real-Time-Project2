#include "ipc_setup.h"
#include "headers.h"
#include "constants.h"
#include <sys/mman.h> // for mmap, MAP_SHARED, etc.
#include <fcntl.h>    // for shm_open

// ✅ Create oven semaphore (value = 1)
sem_t *init_oven_semaphore()
{
    sem_t *sem = sem_open(OVEN_SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED)
    {
        perror("Failed to create oven semaphore");
        exit(EXIT_FAILURE);
    }
    return sem;
}

// ✅ Access existing oven semaphore
sem_t *get_oven_semaphore()
{
    sem_t *sem = sem_open(OVEN_SEM_NAME, 0);
    if (sem == SEM_FAILED)
    {
        perror("Failed to open existing oven semaphore");
        exit(EXIT_FAILURE);
    }
    return sem;
}

// ✅ Create and map shared memory
void *init_shared_memory(const char *name, size_t size)
{
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
    {
        perror("Failed to create shared memory");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, size) < 0)
    {
        perror("Failed to set shared memory size");
        exit(EXIT_FAILURE);
    }

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }

    return addr;
}

// ✅ Attach to existing shared memory
void *attach_shared_memory(const char *name, size_t size)
{
    int fd = shm_open(name, O_RDWR, 0666);
    if (fd < 0)
    {
        perror("Failed to open shared memory");
        exit(EXIT_FAILURE);
    }

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }

    return addr;
}

// ✅ Unlink a specific shared memory object
void unlink_shared_memory(const char *name)
{
    if (shm_unlink(name) < 0)
    {
        perror("Failed to unlink shared memory");
    }
}

// ✅ Clean up all IPC objects (FIFOs + semaphores + shared memory)
void cleanup_ipc()
{
    // FIFOs
    unlink(FIFO_CHEF_TO_BAKER);
    unlink(FIFO_BAKER_TO_SELLER);

    // Semaphores
    sem_unlink(OVEN_SEM_NAME);

    // Shared Memory (optional, if you want full cleanup)
    unlink_shared_memory(SHM_NAME_READY_ITEMS);
    unlink_shared_memory(SHM_NAME_STORE);
}
