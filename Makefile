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

all: bin/monitor bin/spy_simulation bin/timer
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
			 		src/common/posix_semaphore.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/spy_simulation/spy_simulation.o: src/spy_simulation/spy_simulation.c include/spy_simulation.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# TIMER
# ----------------------------------------------------------------------------

bin/timer: src/timer/timer.o \
			src/common/posix_semaphore.o \
			src/timer/microseconds_sleep.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/timer/timer.o: src/timer/timer.c include/timer.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c
		
src/timer/microseconds_sleep.o: src/timer/microseconds_sleep.c include/timer.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# COMMON OBJECTS FILES
# ----------------------------------------------------------------------------

src/common/logger.o: src/common/logger.c include/logger.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/common/posix_semaphore.o: src/common/posix_semaphore.c include/posix_semaphore.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# CLEANING
# ----------------------------------------------------------------------------
clean:
	rm src/monitor/*.o src/common/*.o src/spy_simulation/*.o

distclean: clean
	rm bin/monitor

