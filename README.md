# gpsdate

Simple tool for fetching date and time from GPS receiver and saving it as a local time on Linux system. Waits specified time until valid $GPRMC NMEA sentence is received. If no valid time information is received (i.e. the signal is poor), the local time remains unchanged.

# Fetch

Download the ZIP archive or fetch source code using <code>git</code>:

    git clone https://github.com/adamheinrich/gpsdate

# Build

To build the project, simply navigate to project's directory and call <code>make</code>:

    cd gpsdate
    make all

# Usage

Connect the GPS receiver and obtain ts device name. Mine is located in <code>/dev/ttyACM0</code> as found in <code>dmesg</code>:

    $ dmesg | tail
    ...
    [22625.034377] usb 1-1.1: new full-speed USB device number 9 using ehci_hcd
    [22625.128637] cdc_acm 1-1.1:1.0: ttyACM0: USB ACM device

Then check if it sends back NMEA sentences, i.e. using <code>cat</code>:

    $ cat /dev/ttyACM0 
    $GPRMC,191745.80,V,,,,,,,160314,,,N*7B
    $GPVTG,,,,,,,,,N*30
    ...

Now you can use the <code>gpsdate</code> tool to change your local time. Simply call

    # ./gpsdate /dev/ttyACM0
    Local datetime was: 2014-03-16 20:19:06 (CET)
    GPS   datetime  is: 2014-03-16 19:19:07 (UTC)
    Succesfully updated local time.

Note that the utility has to be run with root privilegies. To achieve this on Debian or Ubuntu, run <code>gpsdate</code> using <code>sudo</code>:

    $ sudo ./gpsdate /dev/ttyACM0

You will propably want to change the default baudrate or maximum time to wait for a valid NMEA sentence. The <code>-h</code> option might help you:

    $ ./gpsdate -h
    Usage: ./gpsdate [-b <BAUDRATE>] [-d <DELAY>] PORT
    ...
      
        -b BAUDRATE  Sets specified baudrate. Only limited baudrate set {2400, 4800, ..., 230400} is supported (Default 9600 bd).
        -d DELAY     Sets the maximum time in seconds or -1 for infinite loop (Default 10 s).
        -h           Displays this help.

