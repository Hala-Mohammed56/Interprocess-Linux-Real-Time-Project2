#include "headers.h"
#include "constants.h"
#include "config_loader.h"
#include <time.h>     // for time(), clock_gettime(), CLOCK_MONOTONIC
#include <sys/time.h> // for struct timespec on some systems

#define REQUEST_TEMPLATE "buy %s\n"
#define RESPONSE_BUFFER 128

const char *possible_items[] = {
    "bread", "sandwich", "cake", "sweet", "sweet_patisserie", "savory_patisserie"};
#define NUM_ITEMS (sizeof(possible_items) / sizeof(possible_items[0]))

int main()
{
    Config config = load_config("config.txt");

    int fd_to_seller = open("/tmp/pipe_customer_to_seller", O_WRONLY);
    if (fd_to_seller == -1)
    {
        perror("[Customer] Failed to open pipe to seller");
        exit(1);
    }

    int fd_from_seller = open("/tmp/pipe_seller_to_customer", O_RDONLY);
    if (fd_from_seller == -1)
    {
        perror("[Customer] Failed to open pipe from seller");
        exit(1);
    }

    srand(time(NULL) ^ getpid());

    while (1)
    {
        const char *item = possible_items[rand() % NUM_ITEMS];

        char request[64];
        snprintf(request, sizeof(request), REQUEST_TEMPLATE, item);

        printf("[Customer] Requesting: %s", request);
        write(fd_to_seller, request, strlen(request));

        // Start timer
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        char response[RESPONSE_BUFFER] = {0};
        ssize_t len = read(fd_from_seller, response, sizeof(response) - 1);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                            (end.tv_nsec - start.tv_nsec) / 1000000.0;

        if (len > 0)
        {
            response[len] = '\0';
            printf("[Customer] Received: %s (Response time: %.2f ms)\n", response, elapsed_ms);

            // Handle delays and frustration
            if (elapsed_ms > config.max_wait_time_ms)
            {
                printf("[Customer] Frustrated due to delay and left the bakery 😡\n");
            }
            else if (strstr(response, "not available"))
            {
                printf("[Customer] Couldn't find item and left 😞\n");
            }
            else
            {
                // Random complaint chance
                int chance = rand() % 100;
                if (chance < config.complaint_chance_percent)
                {
                    printf("[Customer] Complained about quality! 😠\n");

                    // Others might leave
                    int leave_chance = rand() % 100;
                    if (leave_chance < config.leave_if_unhappy_percent)
                    {
                        printf("[Customer] Others saw complaint and left! 😢\n");
                    }
                }
            }
        }

        usleep(config.customer_arrival_interval_ms * 1000);
    }

    close(fd_to_seller);
    close(fd_from_seller);
    return 0;
}
