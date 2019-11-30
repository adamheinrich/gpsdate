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

#ifndef NMEA_H
#define	NMEA_H

/**
 * Parses bytes from GPS receiver and fires listener action when the NMEA sentence is complete and has valid checksum.
 * The buffer size is arbitrary.
 * 
 * @param numbytes Number of bytes read in buffer
 * @param buffer Buffer of incoming bytes
 * @param listener Function to be fired after the sentence is parsed
 */
void nmea_parse(int numbytes, char *buffer, void (*listener)(int argc, char argv[][32]));

#endif	/* NMEA_H */

