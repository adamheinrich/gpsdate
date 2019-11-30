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
#include <stdlib.h>
#include "nmea.h"

/* NMEA message argument values (array of strings). Argument 0 is the sentence name (typically GPxxx), the last argument is message checksum (hexadecimal number behind '*') */
char argv[32][32];

/* NMEA message argument count (incl. NMEA sentence name and checksum) */
int argc = 0;

int checksum = 0;
int calcchecksum = 0;
int argvbyte = 0;

void clear() {
    checksum = 0;
    argc = 0;
    argvbyte = 0;
    calcchecksum = 0;
    argv[argc][0] = '\0';
}

void nmea_parse(int numbytes, char *buffer, void (*nmea_parsed)(int argc, char argv[][32])) {

    while (numbytes > 0 && *buffer) {
        switch (*buffer) {
            case '$': /* NMEA sentence start, calculate checksum */
                clear();
                calcchecksum = 1;
                break;
            case '*': /* Stop calculating checksum */
                calcchecksum = 0;
                argc++;
                argvbyte = 0;
                argv[argc][0] = '\0';
                break;
            case '\n': /* NMEA sentence end, validate checksum and fire listener */
                argc++;

                int read_checksum = 0;
                sscanf(argv[argc - 1], "%x", &read_checksum);
                
                if (checksum == read_checksum) {
                    nmea_parsed(argc, argv);
                }

                clear();
                break;
            case ',': /* Argument delimiter */
                argc++;
                argvbyte = 0;
                argv[argc][0] = '\0';
                break;
            default:
                argv[argc][argvbyte++] = *buffer;
                argv[argc][argvbyte] = '\0';
                break;
        }

        if (calcchecksum && *buffer != '$') {
            checksum ^= *buffer;
        }

        buffer++;
        numbytes--;
    }
}
