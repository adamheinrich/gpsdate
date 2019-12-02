BIN = gpsdate

OBJS = $(patsubst %.c, %.o, $(wildcard *.c))

CFLAGS = -O3 -Wall -pedantic -Wconversion -Wcast-align
LDFLAGS = $(CFLAGS)

.PHONY: all
all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(BIN)
