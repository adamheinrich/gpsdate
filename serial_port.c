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

#include "serial_port.h"
#include <termios.h>
#include <fcntl.h>

static speed_t baudval(int baudrate)
{
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
		return B0;
	}
}

int serial_port_open(const char *port_name, int baudrate)
{
	speed_t speed = baudval(baudrate);
	if (speed == B0)
		return -1;

	int fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
		return -1;

	struct termios options = {
		/* 8N1, ignore modem lines, receive: */
		.c_cflag = (CS8 | CLOCAL | CREAD),

		/* Ignore parity errors, raw output, canonical input: */
		.c_iflag = IGNPAR,
		.c_oflag = 0,
		.c_lflag = ICANON,

		/* A read() is satisfied when either VMIN characters have been
		   transferred to the caller's buffer, or when VTIME (in tenths
		   of second) expires between characters: */
		.c_cc[VMIN] = 1,
		.c_cc[VTIME] = 5,
	};

	tcflush(fd, TCIFLUSH);
	cfsetospeed(&options, speed);
	cfsetispeed(&options, speed);
	tcsetattr(fd, TCSANOW, &options);

	return fd;
}
