#include "headers.h"
#include "constants.h"
#include "ipc_setup.h"
#include "shm_ingredients.h" // لإدارة المخزون من المكونات

sem_t* oven_sem;
int fd_in = -1, fd_out = -1;
char* team = NULL;

// إغلاق الموارد عند الإنهاء
void handle_sigint(int sig) {
    printf("\n[Baker] Cleaning up resources...\n");

    if (fd_in >= 0) close(fd_in);
    if (fd_out >= 0) close(fd_out);
    if (oven_sem) sem_close(oven_sem);

    unlink(FIFO_CHEF_TO_BAKER);
    unlink(FIFO_BAKER_TO_SELLER);

    cleanup_ingredients();

    printf("[Baker] Resources cleaned. Exiting.\n");
    exit(0);
}

// إعداد قنوات الاتصال
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

    init_ingredient_ipc(0); // ربط بالمخزون المشترك (لا تنشئ جديد)
}

// تحقق مما إذا كان العنصر مناسبًا لفريق الخباز
int is_item_for_team(const char* item) {
    if (team == NULL) return 1;

    if (strcmp(team, "bread") == 0)
        return strstr(item, "Bread Dough") != NULL;
    else if (strcmp(team, "cake") == 0)
        return strstr(item, "Cake Mixture") != NULL;
    else if (strcmp(team, "sweets") == 0)
        return strstr(item, "Sweet Patisserie Paste") != NULL || strstr(item, "Savory Patisserie Paste") != NULL;

    return 0;
}

// الوقت المخصص لخبز كل صنف
int get_bake_time(const char* item) {
    if (strcmp(item, "Bread Dough") == 0) return 5;
    if (strcmp(item, "Cake Mixture") == 0) return 6;
    if (strcmp(item, "Sweet Patisserie Paste") == 0) return 7;
    if (strcmp(item, "Savory Patisserie Paste") == 0) return 7;
    return 5;
}

// عملية الخَبز
void process_baking(const char* item) {
    if (!is_item_for_team(item)) {
        printf("[Baker] (Skipped) Not my team's item: %s\n", item);
        return;
    }

    // تحقق من توفر المكونات
    if (!try_consume_ingredients(item)) {
        printf("[Baker] Not enough ingredients for: %s ❌\n", item);
        return;
    }

    printf("[Baker] Received from chef: %s\n", item);

    sem_wait(oven_sem);
    printf("[Baker] Baking in progress: %s...\n", item);
    sleep(get_bake_time(item));
    sem_post(oven_sem);

    char result[MAX_MESSAGE_SIZE];
    snprintf(result, sizeof(result), "%s ready for sale", item);
    dprintf(fd_out, "%s\n", result);
    printf("[Baker] Sent to seller: %s\n", result);

    // توثيق العملية في ملف لوق
    FILE* log = fopen("baker_log.txt", "a");
    if (log) {
        fprintf(log, "Baked: %s\n", item);
        fclose(log);
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, handle_sigint);
    printf("[Baker] Process started ✅\n");

    if (argc > 1) {
        team = argv[1];
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
