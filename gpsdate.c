/*
 * This file is part of gpsdate.
 *
 * Copyright (C) 2014 Adam Heinrich <adam@adamh.cz>
 *
 * Gpsdate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gpsdate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gpsdate.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "nmea.h"
#include "serial_port.h"

#define DEFAULT_BAUDRATE	9600
#define DEFAULT_TIMEOUT		10

#define TIME_FMT		"%04d-%02d-%02d %02d:%02d:%02d"

static volatile bool read_gps = true;
static bool date_changed;

static const char *help_text =
"Sets time from a GPS receiver connected to a serial port as a local time.\n\n"
"Options:\n"
"  -b <baudrate>    Sets baud rate. Only a limited set of baud rates {2400,\n"
"                   4800, ..., 230400} is supported (Default %d baud).\n"
"  -t,-d <timeout>  Sets the maximum timeout in seconds or 0 for no timeout\n"
"                   (Default %d seconds)\n"
"  -h               Displays this help.\n";

static void print_help(bool full_help, const char *program_name)
{
	if (full_help) {
		printf("Usage: %s [options] port\n", program_name);
		printf(help_text, DEFAULT_BAUDRATE, DEFAULT_TIMEOUT);
	} else {
		fprintf(stderr, "Usage: %s [-bt] port\n", program_name);
		fprintf(stderr, "Run `%s -h` for help.\n", program_name);
	}
}

static void process_message(const char *msgid, const char **data)
{
	size_t length = 0;
	while (data[length])
		length++;

	if (strcmp(msgid, "GPRMC") == 0 && (length == 11 || length == 12)) {

		/* Only parse time when there is a fix: */
		if (strcmp(data[1], "A") != 0)
			return;

		/* Parse UTC time: */
		int day, month, year, hours, minutes, seconds;
		const char *tfmt = "%02d%02d%02d";

		if (sscanf(data[0], tfmt, &hours, &minutes, &seconds) != 3)
			return;

		if (sscanf(data[8], tfmt, &day, &month, &year) != 3)
			return;

		year += 2000;

		if ((day >= 1 && day <= 31) &&
		    (month >= 1 && month <= 12) &&
		    (year >= 2000) &&
		    (hours >= 0 && hours <= 23) &&
		    (minutes >= 0 && minutes <= 59) &&
		    (seconds >= 0 && seconds <= 59)) {

			/* Get current time, print it and modify: */
			time_t curr_time;
			time(&curr_time);
			struct tm *t = localtime(&curr_time);

			printf("Local time was: " TIME_FMT " (%s)\n",
			       (t->tm_year + 1900), (t->tm_mon + 1), t->tm_mday,
			       t->tm_hour, t->tm_min, t->tm_sec, t->tm_zone);
			printf("GPS   time  is: " TIME_FMT " (UTC)\n",
			       year, month, day, hours, minutes, seconds);

			t->tm_year = year - 1900;
			t->tm_mon = month - 1;
			t->tm_mday = day;
			t->tm_hour = hours;
			t->tm_min = minutes;
			t->tm_sec = seconds;

			/* Set new system time: */
			time_t gps_time = mktime(t);
			gps_time += t->tm_gmtoff;

			if (stime(&gps_time) == 0) {
				printf("Successfully updated local time.\n");
				date_changed = true;
			} else {
				fprintf(stderr, "Couldn't set local time. "
					"Do you have root privileges?\n");
			}

			read_gps = false;
		}
	}
}

static void signal_handler(int signo)
{
	read_gps = false;
}

int main(int argc, char **argv)
{
	const char *port_name = NULL;
	int timeout = DEFAULT_TIMEOUT;
	int baudrate = DEFAULT_BAUDRATE;

	/* Handle options: */
	int c;
	while ((c = getopt(argc, argv, "hb:d:t:")) != -1) {
		switch (c) {
		case 'b':
			sscanf(optarg, "%d", &baudrate);
			break;
		case 't':
		case 'd': /* For backward compatibility ("delay"): */
			sscanf(optarg, "%d", &timeout);
			break;
		case 'h':
			print_help(true, argv[0]);
			return 0;
		default:
			print_help(false, argv[0]);
			return 1;
		}
	}

	/* Open port: */
	if (optind < argc) {
		port_name = argv[optind];
	} else {
		print_help(false, argv[0]);
		return 1;
	}

	int fd = serial_port_open(port_name, baudrate);
	if (fd == -1) {
		fprintf(stderr, "Can't open port %s at %d baud\n",
			port_name, baudrate);
		return 1;
	}

	signal(SIGINT, &signal_handler);
	signal(SIGTERM, &signal_handler);

	time_t start_time, curr_time;
	time(&start_time);

	char buffer[64];

	while (read_gps) {
		time(&curr_time);
		if (timeout > 0 && (curr_time - start_time) >= timeout) {
			printf("No valid time information from GPS in the last "
			       "%d seconds.\n", timeout);
			break;
		}

		ssize_t nread = read(fd, buffer, sizeof(buffer));
		if (nread > 0)
			nmea_parse(buffer, (size_t)nread, &process_message);

		usleep(100000);
	}

	close(fd);

	return date_changed ? 0 : 2;
}
