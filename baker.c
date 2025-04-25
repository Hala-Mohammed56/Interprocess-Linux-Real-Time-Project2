#include "headers.h"        // All C libraries
#include "constants.h"      // FIFO and semaphore names, sizes, durations
#include "ipc_setup.h"      // Semaphore init/get functions

sem_t* oven_sem;
int fd_in = -1, fd_out = -1;

// Handle Ctrl+C cleanup
void handle_sigint(int sig) {
    printf("\n[Baker] Cleaning up resources...\n");

    if (fd_in >= 0) close(fd_in);
    if (fd_out >= 0) close(fd_out);
    if (oven_sem) sem_close(oven_sem);

    unlink(FIFO_CHEF_TO_BAKER);
    unlink(FIFO_BAKER_TO_SELLER);

    printf("[Baker] Resources cleaned. Exiting.\n");
    exit(0);
}

// Setup semaphores and FIFOs
void setup_ipc() {
    // Get oven semaphore created in main or ipc_setup
    oven_sem = get_oven_semaphore();

    // Create and open FIFOs
    mkfifo(FIFO_CHEF_TO_BAKER, 0666);
    mkfifo(FIFO_BAKER_TO_SELLER, 0666);

    fd_in = open(FIFO_CHEF_TO_BAKER, O_RDONLY);
    if (fd_in < 0) {
        perror("Failed to open FIFO from chef to baker");
        exit(EXIT_FAILURE);
    }

    fd_out = open(FIFO_BAKER_TO_SELLER, O_WRONLY);
    if (fd_out < 0) {
        perror("Failed to open FIFO from baker to seller");
        exit(EXIT_FAILURE);
    }
}

// Baking logic
void process_baking(const char* item) {
    printf("[Baker] Received from chef: %s\n", item);

    sem_wait(oven_sem);
    printf("[Baker] Baking in progress: %s...\n", item);
    sleep(BAKING_DURATION);
    sem_post(oven_sem);

    char result[MAX_MESSAGE_SIZE];
    snprintf(result, sizeof(result), "%s ready for sale", item);
    write(fd_out, result, strlen(result));
    printf("[Baker] Sent to seller: %s\n", result);
}

int main() {
    signal(SIGINT, handle_sigint);
    printf("[Baker] Process started ✅\n");

    setup_ipc();

    char buffer[MAX_MESSAGE_SIZE];
    while (1) {
        int bytes = read(fd_in, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            process_baking(buffer);
        }
    }

    return 0;
}
