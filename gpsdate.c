/*
Project gpsdate is licensed under the MIT License (MIT):

Copyright (c) 2014 Adam Heinrich, <hi@adamheinrich.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "nmea.h"

/** Main loop condition */
int read_gps;

/**
 * Convert baudrate to constant for cfsetispeed and cfsetospeed functions.
 * @param baudrate Requested baudrate (i.e. 9600)
 * @return Baudrate constant (i.e. B9600) or -1 for invalid baudrate
 */
int baudrate_constant(int baudrate) {
    switch (baudrate) {
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        default:
            return -1;
    }
}

int open_port(char *port_name, int baudrate) {
    int fd;

    fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("open_port: Unable to open: ");
    } else {
        fcntl(fd, F_SETFL, 0);
    }

    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, baudrate_constant(baudrate));
    cfsetospeed(&options, baudrate_constant(baudrate));

    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

/**
 * Listener fot the nmea_parse function. Parses time from GPRMC sentence and quits the main loop.
 * 
 * @param argc NMEA sentence arguments count
 * @param argv NMEA sentence arguments
 */
void message_complete(int argc, char argv[][32]) {

    if (argc == 14 && strcmp(argv[0], "GPRMC") == 0) {
        int hours, minutes, seconds;
        int day, month, year;

        /* Parse UTC time */
        sscanf(argv[1], "%02d%02d%02d", &hours, &minutes, &seconds);
        sscanf(argv[9], "%02d%02d%02d", &day, &month, &year);
        year += 2000;

        if (day > 0 && day <= 31 && month > 0 && month <= 12 && year >= 2000) {
            if (hours >= 0 && hours < 24 && minutes >= 0 && minutes <= 60 && seconds >= 0 && seconds <= 60) {

                time_t curr_time;
                struct tm *timeinfo;

                /* Get current time, print it and modify */
                time(&curr_time);
                timeinfo = localtime(&curr_time);

                printf("Local datetime was: %04d-%02d-%02d %02d:%02d:%02d (%s)\n", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday + 1, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_zone);
                printf("GPS   datetime  is: %04d-%02d-%02d %02d:%02d:%02d (UTC)\n", year, month, day, hours, minutes, seconds);

                timeinfo->tm_year = year - 1900;
                timeinfo->tm_mon = month - 1;
                timeinfo->tm_mday = day - 1;
                timeinfo->tm_hour = hours;
                timeinfo->tm_min = minutes;
                timeinfo->tm_sec = seconds;

                /* Calculate number of seconds since some old date, probably 1970s */
                time_t gps_time = mktime(timeinfo);

                /* Add number of seconds from UTC (timezone is probably different from UTC) */
                gps_time += timeinfo->tm_gmtoff;

                /* Set system time, may need root privilegies */
                if (stime(&gps_time) == 0) {
                    printf("Succesfully updated local time.\n");
                } else {
                    fprintf(stderr, "Local time can't be updated. Do you have root privilegies?\n");
                }

                read_gps = 0;
            }
        }
    }
}

void signal_handler(int signal_value) {
    read_gps = 0;
}

static void catch_signals() {
    struct sigaction action;
    action.sa_handler = signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

int main(int argc, char** argv) {
    /* Parameters (can be set using -options) */
    char* port_name = NULL;
    int delay = 10;
    int baudrate = 9600;

    /* Handle -options */
    int c;
    while ((c = getopt(argc, argv, "hb:d:")) != -1) {
        switch (c) {
            case 'b':
                sscanf(optarg, "%d", &baudrate);
                break;
            case 'd':
                sscanf(optarg, "%d", &delay);
                break;
            case 'h':
                printf("Usage: %s [-b <BAUDRATE>] [-d <DELAY>] PORT\n", argv[0]);
                printf("Saves time from a GPS receiver connected to PORT as local time.\n");
                printf("Waits specified time until valid $GPRMC NMEA sentence is received. If no valid time information is received (i.e. the signal is poor), the local time remains unchanged.\n");
                printf("This program has to be run with root privilegies in order to configure local time.\n");
                printf("\n");
                printf("  -b BAUDRATE  Sets specified baudrate. Only limited baudrate set {2400, 4800, ..., 230400} is supported (Default %d bd).\n", baudrate);
                printf("  -d DELAY     Sets the maximum time in seconds or -1 for infinite loop (Default %d s).\n", delay);
                printf("  -h           Displays this help.\n");
                printf("\n");
                printf("(c) 2014 Adam Heinrich, www.adamheinrich.com\n");
                return 0;
                break;
            case '?':
                if (optopt == 'b' || optopt == 'd') {
                    fprintf(stderr, "Option -%c requires an argument.\nUse %s -h for help.\n", optopt, argv[0]);
                } else {
                    fprintf(stderr, "Unknown option character '%c'.\nUse %s -h for help.\n", optopt, argv[0]);
                }
                return 1;
            default:
                fprintf(stderr, "Usage: %s [-b <BAUDRATE>] [-d <DELAY>] PORT_NAME\n", argv[0]);
                return 1;
        }
    }

    /* Save port name */
    if (optind < argc) {
        port_name = argv[optind];
    } else {
        fprintf(stderr, "Port name must be specified.\nUse %s -h for help.\n", argv[0]);
        return 1;
    }
    
    /* Check baudrate */
    if (baudrate_constant(baudrate) == -1) {
        fprintf(stderr, "Unsupported baudrate: %d.\nUse %s -h for help.\n", baudrate, argv[0]);
        return 1;
    }

    /* Open port */
    int fd = open_port(port_name, baudrate);
    if (fd == -1) {
        fprintf(stderr, "Can't open port %s\n", port_name);
        return 1;
    }

    /* Real gentlemen clean up after finishing the job! */
    catch_signals();

    int nread;
    char buffer[32];
    time_t first_time;
    time_t curr_time;
    time(&first_time);

    read_gps = 1;

    while (read_gps) {
        nread = read(fd, buffer, 32);
        if (nread > 0) {
            nmea_parse(nread, buffer, &message_complete);
        }

        time(&curr_time);
        if (delay != -1 && (curr_time - first_time) >= delay) {
            printf("No valid time information from GPS in last %d seconds.\n", delay);
            read_gps = 0;
        }
    }

    close(fd);

    return 0;
}

