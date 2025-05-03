# Compiler
CC = gcc
CFLAGS = -Wall -lrt -lpthread

# Common sources
SRC = ipc_setup.c config_loader.c shm_ingredients.c semaphor.c
HEADERS = headers.h constants.h config_loader.h ipc_setup.h shm_ingredients.h semaphor.h shared_defs.h

# Targets
all: main chef baker seller supply customer launcher

main: main.c $(SRC) $(HEADERS)
	$(CC) main.c $(SRC) -o main $(CFLAGS)

chef: chef.c $(SRC) $(HEADERS)
	$(CC) chef.c $(SRC) -o chef $(CFLAGS)

baker: baker.c $(SRC) $(HEADERS)
	$(CC) baker.c $(SRC) -o baker $(CFLAGS)

seller: seller.c logger.c logger.h $(SRC) $(HEADERS)
	$(CC) seller.c logger.c $(SRC) -o seller $(CFLAGS)

supply: supply.c config_loader.c config_loader.h
	$(CC) supply.c config_loader.c -o supply $(CFLAGS)

customer: customer.c $(SRC) $(HEADERS)
	$(CC) customer.c $(SRC) -o customer $(CFLAGS)

launcher: launcher.c $(SRC) $(HEADERS)
	$(CC) launcher.c $(SRC) -o launcher $(CFLAGS)

clean:
	rm -f main chef baker seller supply customer launcher
	rm -f /tmp/chef_to_baker /tmp/baker_to_seller
	rm -f *.o *.log
	@echo " Cleaned all build files and FIFOs."
