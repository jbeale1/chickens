#!/usr/bin/env python3

# Read lines from serial port and save to file

# tested with Python 3.6.5 (default, Apr  1 2018, 05:46:30) 
import serial, datetime, time

logfile='log.csv'      # where to write log file
# device='/dev/ttyACM0'  # Teensy 3.2
device='/dev/ttyUSB0'  # Arduino Nano compatible board

FLIM = 100  # flush buffer after writing this many lines

f = open(logfile, 'w')

fctr = 8  # saved-line counter (flush buffer after FLIM lines)
pctr = 5  # sample decimation counter 
firstline = 1  # have not yet finished the first line

f.write("ADC1, ADC2\n") # write CSV file column header
oline = "# Start: " + str(datetime.datetime.now()) + "\n"
f.write(oline)
f.write("# MCP3424 Diff 01+23 channels 2023-07-30  JPB\n")
f.flush()

oldm = int(time.time()/60.0)  # time.time() = floating-point seconds since epoch

with serial.Serial(device, 9600, timeout=10) as ser:
  while True:
    line = ser.readline()
    # print(line) # DEBUG for immediate feedback
    s0 = line.decode("utf-8").strip()  # bytes to string, without start/end whitespace
    s = s0.strip('\0')  # serial port open sometimes gives a null byte
    nc = len(s)  # how many useful characters in the input string?
    s = s + "\n"
    print(s,end='')

    if ( nc > 0 ):
      m = int(time.time()/60.0)  # time.time() = floating-point seconds since epoch
      if (m != oldm):  # minute marker at top of each minute
        oline = "# " + str(datetime.datetime.now()) + "\n"
        f.write(oline)
        oldm=m

      f.write(s)
      fctr += 1
      if (fctr > FLIM):
        f.flush()
        fctr = 0
