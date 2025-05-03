#include "headers.h"
#include "constants.h"
#include "config_loader.h"
#include "ipc_setup.h"
#include "shared_defs.h"
#include "logger.h"

#include <time.h>
#include <signal.h>
#include <sys/mman.h>

int fd_in = -1;
BakerySharedData *shm = NULL;
Config config;

float get_price(const char *item_name)
{
    if (strstr(item_name, "Bread"))
        return config.price_bread;
    if (strstr(item_name, "Cake"))
        return config.price_cake;
    if (strstr(item_name, "Sweet Patisserie"))
        return config.price_sweet_patisserie;
    if (strstr(item_name, "Savory Patisserie"))
        return config.price_savory_patisserie;
    return 1.0;
}

void handle_sigint(int sig)
{
    printf("\n[Seller] Cleaning up...\n");

    if (fd_in >= 0)
        close(fd_in);
    unlink(FIFO_BAKER_TO_SELLER);

    if (shm)
        munmap(shm, sizeof(BakerySharedData));

    printf("[Seller] Exiting.\n");
    exit(0);
}

void setup_fifo()
{
    // Create the FIFOs needed by customers
    mkfifo("/tmp/pipe_customer_to_seller", 0666);
    mkfifo("/tmp/pipe_seller_to_customer", 0666);

    // Already existing pipe for baker
    mkfifo(FIFO_BAKER_TO_SELLER, 0666);
    fd_in = open(FIFO_BAKER_TO_SELLER, O_RDONLY);
    if (fd_in < 0)
    {
        perror("[Seller] Failed to open FIFO from baker to seller");
        exit(EXIT_FAILURE);
    }
}

void setup_shared_memory()
{
    shm = (BakerySharedData *)attach_shared_memory(SHM_NAME_STORE, sizeof(BakerySharedData));
}

void load_config_file()
{
    config = load_config("config.txt");
}

int main()
{
    signal(SIGINT, handle_sigint);
    printf("[Seller] Ready to receive baked goods 🛒\n");

    setup_fifo();
    setup_shared_memory();
    load_config_file();

    FILE *fifo_in = fdopen(fd_in, "r");
    if (!fifo_in)
    {
        perror("[Seller] Failed to open FIFO stream");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_MESSAGE_SIZE];
    int customer_id = 1;
    time_t start_time = time(NULL);

    while (fgets(buffer, sizeof(buffer), fifo_in))
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        printf("[Seller] Customer #%d served: %s\n", customer_id++, buffer);

        shm->sales.total_customers++;
        float earned = get_price(buffer);
        shm->sales.profit += earned;
        printf("[Seller] Earned: %.2f 💰\n", earned);

        int elapsed_time = time(NULL) - start_time;

        // 🚨 Termination condition
        if (shm->sales.frustrated_customers >= config.max_frustrated_customers ||
            shm->sales.complaints >= config.max_complaints ||
            shm->sales.profit >= config.profit_goal ||
            elapsed_time >= config.simulation_duration_minutes * 60)
        {

            printf("\n[Seller] ✅ Termination condition met. Stopping simulation...\n");
            break;
        }

        // 🧠 Dynamic feedback: remove a chef if complaints are high
        if (shm->sales.complaints > config.max_complaints / 2)
        {
            printf("[Seller] Too many complaints! Considering removing a chef... 🍳\n");

            FILE *f = fopen("chef_pid.txt", "r");
            if (f)
            {
                int pid;
                while (fscanf(f, "%d", &pid) != EOF)
                {
                    kill(pid, SIGINT);
                    printf("[Seller] Sent SIGINT to Chef PID: %d\n", pid);
                }
                fclose(f);
            }
        }

        // ⏱️ Speed up service if many customers
        if (shm->sales.total_customers > 10)
            usleep(500 * 1000); // 0.5s
        else
            usleep(1000 * 1000); // 1s
    }

    fclose(fifo_in);
    save_final_report(); // Save simulation results
    handle_sigint(SIGINT);
    return 0;
}
