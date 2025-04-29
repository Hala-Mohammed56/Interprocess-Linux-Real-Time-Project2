#include <stdio.h>    
#include <stdlib.h>  
#include <unistd.h>   
#include <sys/types.h>
#include <sys/ipc.h>  
#include <sys/shm.h>  
#include <sys/sem.h>  
#include <fcntl.h>    
#include <string.h>   

// IPC Identifiers (can be defined in a shared header file like ipc_setup.h)
#define SHM_PROFIT_ID 12345          // Shared memory ID for profit
#define SHM_PRODUCTS_ID 12346        // Shared memory ID for products
#define SEM_PROFIT_ID 12347          // Semaphore ID for profit
#define SEM_PRODUCTS_ID 12348        // Semaphore ID for products
#define PIPE_CUSTOMER_TO_SELLER "/pipe_customer_to_seller" // Pipe from customer to seller
#define PIPE_SELLER_TO_CUSTOMER "/pipe_seller_to_customer" // Pipe from seller to customer

#define SHM_SIZE 1024                // Size of shared memory (1024 bytes)

// Function prototype for calculating product prices
double get_price(const char *product);

int main() {
    // Step 1: Connect to shared resources
    int shmid_profit, shmid_products; // Shared memory IDs
    int semid_profit, semid_products; // Semaphore IDs
    int pipe_fd_from_customer, pipe_fd_to_customer; // Pipe file descriptors

    // Connect to shared memory for profit
    shmid_profit = shmget(SHM_PROFIT_ID, SHM_SIZE, 0666); // Get shared memory ID
    if (shmid_profit == -1) { // Check for errors
        perror("Error connecting to shared memory for profit");
        exit(1);
    }
    double *profit = (double *)shmat(shmid_profit, NULL, 0); // Attach shared memory to process
    if (profit == (void *)-1) {
        perror("Error attaching shared memory for profit");
        exit(1);
    }

    // Connect to shared memory for products
    shmid_products = shmget(SHM_PRODUCTS_ID, SHM_SIZE, 0666); // Get shared memory ID
    if (shmid_products == -1) { // Check for errors
        perror("Error connecting to shared memory for products");
        exit(1);
    }
    char *products = (char *)shmat(shmid_products, NULL, 0); // Attach shared memory to process
    if (products == (void *)-1) {
        perror("Error attaching shared memory for products");
        exit(1);
    }

    // Connect to semaphores
    semid_profit = semget(SEM_PROFIT_ID, 1, 0666); // Get semaphore ID for profit
    if (semid_profit == -1) {
        perror("Error connecting to semaphore for profit");
        exit(1);
    }
    semid_products = semget(SEM_PRODUCTS_ID, 1, 0666); // Get semaphore ID for products
    if (semid_products == -1) {
        perror("Error connecting to semaphore for products");
        exit(1);
    }

    // Open pipes for communication
    pipe_fd_from_customer = open(PIPE_CUSTOMER_TO_SELLER, O_RDONLY); // Open pipe for reading
    if (pipe_fd_from_customer == -1) { // Check for errors
        perror("Error opening pipe from customer to seller");
        exit(1);
    }

    pipe_fd_to_customer = open(PIPE_SELLER_TO_CUSTOMER, O_WRONLY); // Open pipe for writing
    if (pipe_fd_to_customer == -1) { // Check for errors
        perror("Error opening pipe from seller to customer");
        exit(1);
    }

    // Step 2: Main simulation loop
    while (1) {
        // Read request from customer
        char request[100]; // Buffer to store customer request
        ssize_t bytes_read = read(pipe_fd_from_customer, request, sizeof(request) - 1); // Read from pipe
        if (bytes_read > 0) {
            request[bytes_read] = '\0'; // Null-terminate the string
        } else {
            fprintf(stderr, "Error reading from pipe or connection closed.\n");
            break; // Exit loop if pipe is closed
        }

        // Log received request
        fprintf(stderr, "Received request: %s\n", request);

        // Parse the request (e.g., "buy bread")
        char product[50]; // Buffer to store product name
        sscanf(request, "%*s %s", product);

        // Handle termination command
        if (strcmp(product, "exit") == 0) {
            fprintf(stderr, "Termination command received. Exiting...\n");
            break; // Exit loop gracefully
        }

        // Define semaphore operation locally
        struct sembuf sem_op;

        // Check product availability
        sem_op.sem_num = 0;       // Semaphore number
        sem_op.sem_op = -1;       // Lock the semaphore
        sem_op.sem_flg = 0;       // No special flags
        if (semop(semid_products, &sem_op, 1) == -1) { // Perform semaphore operation
            perror("Error locking semaphore for products");
            continue; // Skip to next iteration
        }

        char *available_products = products; // Pointer to shared memory for products
        if (strstr(available_products, product) != NULL) { // Check if product exists
            // Product is available
            fprintf(stderr, "Seller: Product '%s' is available.\n", product);

            // Update profit
            sem_op.sem_num = 0;       // Semaphore number
            sem_op.sem_op = -1;       // Lock the semaphore
            sem_op.sem_flg = 0;       // No special flags
            if (semop(semid_profit, &sem_op, 1) == -1) { // Perform semaphore operation
                perror("Error locking semaphore for profit");
                semop(semid_products, &(struct sembuf){0, 1, 0}, 1); // Unlock products semaphore
                continue; // Skip to next iteration
            }

            *profit += get_price(product); // Increase profit based on product price

            sem_op.sem_op = 1;       // Unlock the semaphore
            if (semop(semid_profit, &sem_op, 1) == -1) { // Perform semaphore operation
                perror("Error unlocking semaphore for profit");
            }

            // Send response to customer
            const char *response = "Product available and sold.";
            if (write(pipe_fd_to_customer, response, strlen(response)) == -1) {
                perror("Error writing to pipe");
            }
        } else {
            // Product is not available
            fprintf(stderr, "Seller: Product '%s' is not available.\n", product);
            const char *response = "Product not available.";
            if (write(pipe_fd_to_customer, response, strlen(response)) == -1) {
                perror("Error writing to pipe");
            }
        }

        sem_op.sem_op = 1;         // Unlock the semaphore
        if (semop(semid_products, &sem_op, 1) == -1) { // Perform semaphore operation
            perror("Error unlocking semaphore for products");
        }
    }

    // Step 3: Cleanup resources
    shmdt(profit); // Detach shared memory for profit
    shmdt(products); // Detach shared memory for products
    close(pipe_fd_from_customer); // Close pipe for reading
    close(pipe_fd_to_customer); // Close pipe for writing

    return 0;
}

// Function to determine product prices based on product name
double get_price(const char *product) {
    if (strcmp(product, "bread") == 0) return 1.0; // Price of bread
    if (strcmp(product, "sandwich") == 0) return 3.0; // Price of sandwich
    if (strcmp(product, "cake") == 0) return 5.0; // Price of cake
    if (strcmp(product, "sweet") == 0) return 2.5; // Price of sweet
    if (strcmp(product, "sweet_patisserie") == 0) return 4.0; // Price of sweet patisserie
    if (strcmp(product, "savory_patisserie") == 0) return 4.0; // Price of savory patisserie
    return 0.0; // Default price if product is not found
}