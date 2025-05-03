#include "headers.h"
#include <sys/wait.h>

int main()
{
    printf(" Launcher: Starting the Bakery Simulation!\n");

    //  Clean old IPC resources
    unlink("/tmp/chef_to_baker");
    unlink("/tmp/baker_to_seller");
    sem_unlink("/oven_semaphore");

    //  Create oven semaphore and shared memory
    system("./main");

    //  Start Baker(s)
    pid_t pid_baker1 = fork();
    if (pid_baker1 == 0)
        execl("./baker", "baker", "bread", NULL);

    pid_t pid_baker2 = fork();
    if (pid_baker2 == 0)
        execl("./baker", "baker", "cake", NULL);

    pid_t pid_baker3 = fork();
    if (pid_baker3 == 0)
        execl("./baker", "baker", "sweets", NULL);

    sleep(1); // to let bakers initialize

    // Start Seller
    pid_t pid_seller = fork();
    if (pid_seller == 0)
        execl("./seller", "seller", NULL);

    sleep(1); // to let seller initialize

    // Start Chef
    pid_t pid_chef1 = fork();
    if (pid_chef1 == 0)
        execl("./chef", "chef", "paste", NULL);

    pid_t pid_chef2 = fork();
    if (pid_chef2 == 0)
        execl("./chef", "chef", "cake", NULL);

    pid_t pid_chef3 = fork();
    if (pid_chef3 == 0)
        execl("./chef", "chef", "sweets", NULL);

    // Start Multiple Customers
    for (int i = 0; i < 4; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            char id[10];
            snprintf(id, sizeof(id), "%d", i + 1);
            execl("./customer", "customer", id, NULL);
            exit(0); // fallback if execl fails
        }
    }

    // Wait for seller to finish (termination condition)
    waitpid(pid_seller, NULL, 0);

    // Kill all other children after seller exits
    kill(pid_baker1, SIGINT);
    kill(pid_baker2, SIGINT);
    kill(pid_baker3, SIGINT);
    kill(pid_chef1, SIGINT);
    kill(pid_chef2, SIGINT);
    kill(pid_chef3, SIGINT);

    for (int i = 0; i < 6 + 4; i++) // 6 = chefs+bakers, 4 = customers
        wait(NULL);

    printf(" Launcher: Simulation ended and all processes cleaned up.\n");
    return 0;
}
