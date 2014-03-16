# The final executable name
EXECUTABLE=gpsdate

# List of source files
SOURCES=gpsdate.c nmea.c

CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o $(EXECUTABLE)
