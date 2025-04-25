#include "headers.h"
#include <sys/wait.h>   // for wait()

int main() {
    printf("🚀 Launcher: Starting the Bakery Simulation!\n");

    // 🧹 Clean old IPC resources
    unlink("/tmp/chef_to_baker");
    unlink("/tmp/baker_to_seller");
    sem_unlink("/oven_semaphore");

    // 🔧 Create oven semaphore
    system("./main");

    // 🧑‍🍳 Start Baker(s)
    pid_t pid_baker1 = fork();
    if (pid_baker1 == 0) {
        execl("./baker", "baker", "bread", (char*)NULL); // Bread bakers
        perror("Failed to start baker (bread)");
        exit(EXIT_FAILURE);
    }

    pid_t pid_baker2 = fork();
    if (pid_baker2 == 0) {
        execl("./baker", "baker", "cake", (char*)NULL); // Cake bakers
        perror("Failed to start baker (cake)");
        exit(EXIT_FAILURE);
    }

    pid_t pid_baker3 = fork();
    if (pid_baker3 == 0) {
        execl("./baker", "baker", "sweets", (char*)NULL); // Sweets bakers
        perror("Failed to start baker (sweets)");
        exit(EXIT_FAILURE);
    }

    sleep(1); // wait for bakers to be ready

    // 🛒 Start Seller
    pid_t pid_seller = fork();
    if (pid_seller == 0) {
        execl("./seller", "seller", (char*)NULL);
        perror("Failed to start seller");
        exit(EXIT_FAILURE);
    }

    sleep(1); // wait for seller to be ready

    // 🧑‍🍳 Start Chef(s)
    pid_t pid_chef1 = fork();
    if (pid_chef1 == 0) {
        execl("./chef", "chef", "paste", (char*)NULL); // Paste team
        perror("Failed to start chef (paste)");
        exit(EXIT_FAILURE);
    }

    pid_t pid_chef2 = fork();
    if (pid_chef2 == 0) {
        execl("./chef", "chef", "cake", (char*)NULL); // Cake team
        perror("Failed to start chef (cake)");
        exit(EXIT_FAILURE);
    }

    pid_t pid_chef3 = fork();
    if (pid_chef3 == 0) {
        execl("./chef", "chef", "sweets", (char*)NULL); // Sweets team
        perror("Failed to start chef (sweets)");
        exit(EXIT_FAILURE);
    }

    // 📌 Parent waits for all children
    for (int i = 0; i < 7; i++) {
        wait(NULL);
    }

    printf("✅ Launcher: All processes finished.\n");
    return 0;
}
