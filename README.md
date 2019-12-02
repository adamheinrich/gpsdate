# gpsdate

Gpsdate is a simple tool which reads date and time from a GPS receiver connected
via serial port (or a CDC ACM virtual serial port over USB) and saves it as a
local time on Linux.

The program waits for a specified maximum time until a valid [`$GPRMC`][1] NMEA
sentence is received. If the GPS receiver is not fixed and no valid time
information is received, the local time remains unchanged.

# Building

	git clone https://github.com/adamheinrich/gpsdate # Or download as zip
	cd gpsdate
	make

# Usage

Connect the GPS receiver and obtain ts device name. Mine is `/dev/ttyACM0` as
found in `dmesg`:

	$ dmesg | tail
	...
	[22625.034377] usb 1-1.1: new full-speed USB device number 9 using ehci_hcd
	[22625.128637] cdc_acm 1-1.1:1.0: ttyACM0: USB ACM device

Then check if it sends back NMEA sentences, e.g. using `cat`:

	$ stty -F /dev/ttyACM0 9600 # Optional
	$ cat /dev/ttyACM0
	$GPRMC,191745.80,V,,,,,,,160314,,,N*7B
	$GPVTG,,,,,,,,,N*30

Now you can `gpsdate` to change local time:

	# ./gpsdate /dev/ttyACM0
	Local time was: 2014-03-16 20:19:06 (CET)
	GPS   time  is: 2014-03-16 19:19:07 (UTC)
	Successfully updated local time.

Note that changing time requires root privileges. To achieve this, run:

	sudo ./gpsdate /dev/ttyACM0

It is possible to change the default baud rate or a timeout. Run the program
with option `-h` for a complete list of options:

```
$ ./gpsdate -h
Usage: ./gpsdate [options] port
Sets time from a GPS receiver connected to a serial port as a local time.

Options:
  -b <baudrate>    Sets baud rate. Only a limited set of baud rates {2400,
                   4800, ..., 230400} is supported (Default 9600 baud).
  -t,-d <timeout>  Sets the maximum timeout in seconds or 0 for no timeout
                   (Default 10 seconds)
  -h               Displays this help.
```

# License

Gpsdate is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

See `COPYING` for details.

[1]: https://www.gpsinformation.org/dale/nmea.htm#RMC
