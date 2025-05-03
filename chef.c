#include "headers.h"
#include "constants.h"
#include "ipc_setup.h"

int fd_out = -1;
char* team = NULL; // Team assignment

// تنظيف الموارد عند الإنهاء
void handle_sigint(int sig) {
    printf("\n[Chef] Cleaning up resources...\n");

    if (fd_out >= 0) close(fd_out);
    unlink(FIFO_CHEF_TO_BAKER);

    printf("[Chef] Resources cleaned. Exiting.\n");
    exit(0);
}

// إعداد FIFO للإرسال إلى البايكر
void setup_ipc() {
    mkfifo(FIFO_CHEF_TO_BAKER, 0666);

    fd_out = open(FIFO_CHEF_TO_BAKER, O_WRONLY);
    if (fd_out < 0) {
        perror("[Chef] Failed to open FIFO to baker");
        exit(EXIT_FAILURE);
    }
}

// تحديد زمن التحضير حسب الصنف
int get_prep_time(const char* item) {
    if (strcmp(item, "Bread Dough") == 0) return 2;
    if (strcmp(item, "Cake Mixture") == 0) return 3;
    if (strcmp(item, "Sweet Patisserie Paste") == 0) return 4;
    if (strcmp(item, "Savory Patisserie Paste") == 0) return 4;
    return 2; // افتراضي
}

// إرسال صنف واحد للبايكر
void send_item(const char* item) {
    printf("[Chef] Preparing item: %s\n", item);
    sleep(get_prep_time(item)); // تمثيل وقت التحضير
    dprintf(fd_out, "%s\n", item);
    printf("[Chef] Sent to baker: %s\n", item);

    // لوق
    FILE* log = fopen("chef_log.txt", "a");
    if (log) {
        fprintf(log, "Prepared: %s\n", item);
        fclose(log);
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, handle_sigint);
    printf("[Chef] Process started ✅\n");

    if (argc > 1) {
        team = argv[1];
        printf("[Chef] Assigned to team: %s\n", team);
    } else {
        printf("[Chef] No team assigned, defaulting to all.\n");
    }

    setup_ipc();

    // تحديد العناصر التي سيُحضّرها الشيف حسب الفريق
    const char* items_to_prepare[5];
    int num_items = 0;

    if (team && strcmp(team, "paste") == 0) {
        items_to_prepare[0] = "Bread Dough";
        num_items = 1;
    } else if (team && strcmp(team, "cake") == 0) {
        items_to_prepare[0] = "Cake Mixture";
        num_items = 1;
    } else if (team && strcmp(team, "sweets") == 0) {
        items_to_prepare[0] = "Sweet Patisserie Paste";
        items_to_prepare[1] = "Savory Patisserie Paste";
        num_items = 2;
    } else {
        // الوضع الافتراضي: يحضر كل الأصناف
        items_to_prepare[0] = "Bread Dough";
        items_to_prepare[1] = "Cake Mixture";
        items_to_prepare[2] = "Sweet Patisserie Paste";
        items_to_prepare[3] = "Savory Patisserie Paste";
        num_items = 4;
    }

    int i = 0;
    while (1) {
        send_item(items_to_prepare[i]);
        i = (i + 1) % num_items;
        sleep(1); // تأخير بسيط بين العناصر
    }

    return 0;
}
