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
