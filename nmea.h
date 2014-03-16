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

