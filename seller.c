#include "headers.h"
#include "constants.h"

int fd_in = -1;

void handle_sigint(int sig) {
    printf("\n[Seller] Cleaning up...\n");

    if (fd_in >= 0) close(fd_in);
    unlink(FIFO_BAKER_TO_SELLER);

    printf("[Seller] Exiting.\n");
    exit(0);
}

void setup_fifo() {
    mkfifo(FIFO_BAKER_TO_SELLER, 0666);

    fd_in = open(FIFO_BAKER_TO_SELLER, O_RDONLY);
    if (fd_in < 0) {
        perror("[Seller] Failed to open FIFO from baker to seller");
        exit(EXIT_FAILURE);
    }
}

int main() {
    signal(SIGINT, handle_sigint);
    printf("[Seller] Ready to receive baked goods 🛒\n");

    setup_fifo();

    FILE* fifo_in = fdopen(fd_in, "r");
    if (!fifo_in) {
        perror("[Seller] Failed to open FIFO stream");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_MESSAGE_SIZE];
    int customer_id = 1;
    while (fgets(buffer, sizeof(buffer), fifo_in)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        printf("[Seller] Customer #%d served: %s\n", customer_id++, buffer);
    }

    return 0;
}
