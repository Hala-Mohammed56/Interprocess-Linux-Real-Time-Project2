#include "headers.h"
#include "constants.h"
#include "ipc_setup.h"

sem_t* oven_sem;
int fd_in = -1, fd_out = -1;
char* team; // Team assignment

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

void setup_ipc() {
    oven_sem = get_oven_semaphore();

    mkfifo(FIFO_CHEF_TO_BAKER, 0666);
    mkfifo(FIFO_BAKER_TO_SELLER, 0666);

    fd_in = open(FIFO_CHEF_TO_BAKER, O_RDONLY);
    if (fd_in < 0) {
        perror("[Baker] Failed to open FIFO from chef to baker");
        exit(EXIT_FAILURE);
    }

    fd_out = open(FIFO_BAKER_TO_SELLER, O_WRONLY);
    if (fd_out < 0) {
        perror("[Baker] Failed to open FIFO from baker to seller");
        exit(EXIT_FAILURE);
    }
}

int is_item_for_team(const char* item) {
    if (team == NULL) return 1; // No team assigned, bake everything

    if (strcmp(team, "bread") == 0) {
        return strstr(item, "Bread Dough") != NULL;
    } else if (strcmp(team, "cake") == 0) {
        return strstr(item, "Cake Mixture") != NULL;
    } else if (strcmp(team, "sweets") == 0) {
        return strstr(item, "Sweet Patisserie Paste") != NULL || strstr(item, "Savory Patisserie Paste") != NULL;
    }

    return 0; // Unknown team
}

void process_baking(const char* item) {
    if (!is_item_for_team(item)) {
        printf("[Baker] (Skipped) Not my team's item: %s\n", item);
        return; // Skip items not for this baker's team
    }

    printf("[Baker] Received from chef: %s\n", item);

    sem_wait(oven_sem);
    printf("[Baker] Baking in progress: %s...\n", item);
    sleep(BAKING_DURATION);
    sem_post(oven_sem);

    char result[MAX_MESSAGE_SIZE];
    snprintf(result, sizeof(result), "%s ready for sale", item);
    dprintf(fd_out, "%s\n", result);
    printf("[Baker] Sent to seller: %s\n", result);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, handle_sigint);
    printf("[Baker] Process started ✅\n");

    if (argc > 1) {
        team = argv[1]; // Read team from command line
        printf("[Baker] Assigned to team: %s\n", team);
    } else {
        printf("[Baker] No team assigned, defaulting to all.\n");
    }

    setup_ipc();

    FILE* fifo_in = fdopen(fd_in, "r");
    if (!fifo_in) {
        perror("[Baker] Failed to open FIFO stream");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_MESSAGE_SIZE];
    while (fgets(buffer, sizeof(buffer), fifo_in)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        process_baking(buffer);
    }

    return 0;
}
