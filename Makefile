CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O2 -g

# Compilation under MacOS X or Linux
UNAME=$(shell uname -s)
ifeq ($(UNAME),Darwin)
    CPPFLAGS=-D_REENTRANT -I./include -I/usr/local/Cellar/ncurses/6.3/include
    LDFLAGS=-L/usr/local/Cellar/ncurses/6.3/lib -lncurses -lpthread -lm
endif
ifeq ($(UNAME),Linux)
    CPPFLAGS=-D_REENTRANT -I./include
    LDFLAGS=-lncurses -lpthread -lrt -lm
endif

.PHONY: all clean distclean

all: bin/monitor bin/spy_simulation bin/timer bin/enemy_spy_network bin/citizen_manager bin/counter_intelligence bin/enemy_country

# ----------------------------------------------------------------------------
# MONITOR
# ----------------------------------------------------------------------------
bin/monitor: src/monitor/main.o \
             src/monitor/monitor.o \
             src/monitor/monitor_common.o \
             src/common/logger.o \
			 src/common/posix_semaphore.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/monitor/main.o: src/monitor/main.c include/monitor.h include/monitor_common.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/monitor/monitor.o: src/monitor/monitor.c include/monitor.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/monitor/monitor_common.o: src/monitor/monitor_common.c include/monitor_common.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# SPY_SIMULATION
# ----------------------------------------------------------------------------

bin/spy_simulation: src/spy_simulation/spy_simulation.o \
			 		src/common/posix_semaphore.o \
					src/spy_simulation/main.o \
					src/timer/timer.o \
					src/common/cipher.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/spy_simulation/spy_simulation.o: src/spy_simulation/spy_simulation.c include/spy_simulation.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/spy_simulation/main.o: src/spy_simulation/main.c include/spy_simulation.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# CITIZEN MANAGER
# ----------------------------------------------------------------------------

bin/citizen_manager: src/citizen_manager/citizen_manager.o \
			 		src/common/posix_semaphore.o \
					src/citizen_manager/main.o \
					src/timer/timer.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/citizen_manager/citizen_manager.o: src/citizen_manager/citizen_manager.c include/citizen_manager.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/citizen_manager/main.o: src/citizen_manager/main.c include/citizen_manager.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c


# ----------------------------------------------------------------------------
# ENEMY_SPY_NETWORK
# ----------------------------------------------------------------------------

bin/enemy_spy_network: src/enemy_spy_network/enemy_spy_network.o \
			 		src/common/posix_semaphore.o \
					src/enemy_spy_network/main.o \
					src/common/cipher.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/enemy_spy_network/enemy_spy_network.o: src/enemy_spy_network/enemy_spy_network.c include/enemy_spy_network.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/enemy_spy_network/main.o: src/enemy_spy_network/main.c include/enemy_spy_network.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# COUNTER_INTELLIGENCE
# ----------------------------------------------------------------------------

bin/counter_intelligence: src/counter_intelligence/counter_intelligence.o \
			 		src/common/posix_semaphore.o \
					src/counter_intelligence/main.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/counter_intelligence/counter_intelligence.o: src/counter_intelligence/counter_intelligence.c include/counter_intelligence.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/counter_intelligence/main.o: src/counter_intelligence/main.c include/counter_intelligence.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# ENEMY_COUNTRY
# ----------------------------------------------------------------------------

bin/enemy_country: src/enemy_country/enemy_country.o \
			 		src/common/posix_semaphore.o \
					src/enemy_country/main.o \
					src/common/cipher.o \
					src/timer/timer.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/enemy_country/enemy_country.o: src/enemy_country/enemy_country.c include/enemy_country.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/enemy_country/main.o: src/enemy_country/main.c include/enemy_country.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c


# ----------------------------------------------------------------------------
# TIMER
# ----------------------------------------------------------------------------

bin/timer: src/timer/timer.o \
			src/common/posix_semaphore.o \
			src/timer/main.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/timer/timer.o: src/timer/timer.c include/timer.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c
		
src/timer/main.o: src/timer/main.c include/timer.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# COMMON OBJECTS FILES
# ----------------------------------------------------------------------------

src/common/logger.o: src/common/logger.c include/logger.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/common/posix_semaphore.o: src/common/posix_semaphore.c include/posix_semaphore.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/common/cipher.o: src/common/cipher.c include/cipher.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# CLEANING
# ----------------------------------------------------------------------------
clean:
	rm src/monitor/*.o src/common/*.o src/spy_simulation/*.o src/timer/*.o src/enemy_spy_network/*.o src/citizen_manager/*.o src/counter_intelligence/*.o src/enemy_country/*.o

distclean: clean
	rm bin/monitor bin/spy_simulation bin/citizen_manager bin/enemy_spy_network bin/timer bin/counter_intelligence bin/enemy_country
