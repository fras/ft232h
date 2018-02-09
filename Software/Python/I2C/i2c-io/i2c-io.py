#!/usr/bin/env python
#
# File: i2c-io.py
# Auth: M. Fras, Electronics Division, MPI for Physics, Munich
# Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
# Date: 05 Feb 2018
# Rev.: 06 Feb 2018
#
# Simple raw I2C IO control program using libmpsse.
#
# The IO function is based on the code examples from README.I2C included in the
# libmpsse source code.
#



import argparse
import sys
from mpsse import *



# For debugging.
PREFIX_DEBUG = "DEBUG: "
DEBUG_LEVEL = 0
#DEBUG_LEVEL = 1
#DEBUG_LEVEL = 2
#DEBUG_LEVEL = 3
#DEBUG_LEVEL = 4



# Automatically determine the base (octal decimal, hexdecimal) of an integer.
def auto_int(x):
    return int(x, 0)



# Parse command line arguments.
parser = argparse.ArgumentParser(description='I2C Parameter: CHIP-ADR [DATA-ADR] [DATA].')
parser.add_argument('i2c_params', metavar='I2C-param', type=auto_int, nargs='+',
                   help='I2C control parameters.')
args = parser.parse_args()



# Initialize libmpsse for I2C operations.
#i2c = MPSSE(I2C, FOUR_HUNDRED_KHZ)
i2c = MPSSE(I2C, ONE_HUNDRED_KHZ)



# Show device information.
if DEBUG_LEVEL >= 1:
    print "I2C master device: %s" % i2c.GetDescription()
    print "I2C bus speed: %d Hz" % i2c.GetClock()



# Process I2C access.
# I2C read with chip address only.
if len(args.i2c_params) == 1:
    i2c_dev_adr = args.i2c_params[0]
    if DEBUG_LEVEL >= 3:
        print "%sI2C read from chip address 0x%02x." % (PREFIX_DEBUG, i2c_dev_adr)
    # Generate start condition.
    status = i2c.Start()
    if status:
        print "ERROR: Unable to generate start condition."
        sys.exit(1)
    # Send device address with read command.
    i2c_data = chr(((i2c_dev_adr & 0x7f) << 1) | 0x01)
    status = i2c.Write(i2c_data)
    if status:
        print "ERROR: Unable to set the I2C chip address 0x%02x." % i2c_dev_adr
        sys.exit(1)
    # Check for acknowledge.
    if i2c.GetAck() != ACK:
        print "ERROR: Did not get acknowledge from the I2C chip address 0x%02x." % i2c_dev_adr
        sys.exit(1)
    # Read from the I2C bus.
    i2c_data_len = 1
    i2c_data_str = i2c.Read(i2c_data_len);
    if len(i2c_data_str) != i2c_data_len:
        print "ERROR: Unable to read %d byte(s) from the I2C chip address 0x%02x." % (i2c_data_len, i2c_dev_adr)
        sys.exit(1)
    # Check for acknowledge.
    if i2c.GetAck() != ACK:
        print "ERROR: Did not get acknowledge from the I2C chip address 0x%02x after reading." % i2c_dev_adr
        sys.exit(1)
    # Generate stop condition.
    status = i2c.Stop()
    if status:
        print "ERROR: Unable to generate stop condition."
        sys.exit(1)
    # Print data read from I2C.
    print "0x%02x" % (ord(i2c_data_str) & 0xff)
# I2C read with chip address and data address.
elif len(args.i2c_params) == 2:
    i2c_dev_adr = args.i2c_params[0]
    i2c_data_adr = args.i2c_params[1]
    if DEBUG_LEVEL >= 3:
        print "%sI2C read from chip address 0x%02x, data address 0x%02x." % (PREFIX_DEBUG, i2c_dev_adr, i2c_data_adr)
    # Generate start condition.
    status = i2c.Start()
    if status:
        print "ERROR: Unable to generate start condition."
        sys.exit(1)
    # Send device address with write command.
    i2c_data = chr(((i2c_dev_adr & 0x7f) << 1) | 0x00)
    status = i2c.Write(i2c_data)
    if status:
        print "ERROR: Unable to set the I2C chip address 0x%02x." % i2c_dev_adr
        sys.exit(1)
    # Check for acknowledge.
    if i2c.GetAck() != ACK:
        print "ERROR: Did not get acknowledge from the I2C chip address 0x%02x." % i2c_dev_adr
        sys.exit(1)
    # Write the data address
    i2c_data = chr(i2c_data_adr & 0xff)
    status = i2c.Write(i2c_data)
    if status:
        print "ERROR: Unable to set the I2C data address 0x%02x." % i2c_data_adr
        sys.exit(1)
    # Check for acknowledge.
    if i2c.GetAck() != ACK:
        print "ERROR: Did not get acknowledge from the I2C chip address 0x%02x, data address 0x%02x." % (i2c_dev_adr, i2c_data_adr)
        sys.exit(1)
    # (Re-)Generate start condition.
    status = i2c.Start()
    if status:
        print "ERROR: Unable to (re-)generate start condition."
        sys.exit(1)
    # Send device address with read command.
    i2c_data = chr(((i2c_dev_adr & 0x7f) << 1) | 0x01)
    status = i2c.Write(i2c_data)
    if status:
        print "ERROR: Unable to set the I2C chip address 0x%02x." % i2c_dev_adr
        sys.exit(1)
    # Check for acknowledge.
    if i2c.GetAck() != ACK:
        print "ERROR: Did not get acknowledge from the I2C chip address 0x%02x." % i2c_dev_adr
        sys.exit(1)

    # Read from the I2C bus.
    i2c_data_len = 1
    i2c_data_str = i2c.Read(i2c_data_len);
    if len(i2c_data_str) != i2c_data_len:
        print "ERROR: Unable to read %d byte(s) from the I2C chip address 0x%02x." % (i2c_data_len, i2c_dev_adr)
        sys.exit(1)
    # Check for acknowledge.
    if i2c.GetAck() != ACK:
        print "ERROR: Did not get acknowledge from the I2C chip address 0x%02x after reading." % i2c_dev_adr
        sys.exit(1)
    # Generate stop condition.
    status = i2c.Stop()
    if status:
        print "ERROR: Unable to generate stop condition."
        sys.exit(1)
    # Print data read from I2C.
    print "0x%02x" % (ord(i2c_data_str) & 0xff)
# I2C write.
elif len(args.i2c_params) >= 3:
    i2c_dev_adr = args.i2c_params[0]
    i2c_data_adr = args.i2c_params[1]
    i2c_data_len = len(args.i2c_params) - 2
    if DEBUG_LEVEL >= 3:
        print "%sI2C write to chip address 0x%02x, data address 0x%02x." % (PREFIX_DEBUG, i2c_dev_adr, i2c_data_adr)
        print "%sData: " % PREFIX_DEBUG
        for i in range(0, i2c_data_len):
            print "0x%02x" % (args.i2c_params[i+2] & 0xff),
        print
    # Generate start condition.
    status = i2c.Start()
    if status:
        print "ERROR: Unable to generate start condition."
        sys.exit(1)
    # Send device address with write command.
    i2c_data = chr(((i2c_dev_adr & 0x7f) << 1) | 0x00)
    status = i2c.Write(i2c_data)
    if status:
        print "ERROR: Unable to set the I2C chip address 0x%02x." % i2c_dev_adr
        sys.exit(1)
    # Check for acknowledge.
    if i2c.GetAck() != ACK:
        print "ERROR: Did not get acknowledge from the I2C chip address 0x%02x." % i2c_dev_adr
        sys.exit(1)
    # Prepare the I2C data.
    i2c_data = chr(i2c_data_adr & 0xff)
    for i in range(0, i2c_data_len):
        i2c_data += chr(args.i2c_params[i+2] & 0xff)
    # Send the I2C data.
    status = i2c.Write(i2c_data)
    if status:
        print "ERROR: Unable to write %d byte(s) to the I2C chip address 0x%02x, data address 0x%02x." % (i2c_data_len, i2c_dev_adr, i2c_data_adr)
        sys.exit(1)
    # Check for acknowledge.
    if i2c.GetAck() != ACK:
        print "ERROR: Did not get acknowledge from the I2C chip address 0x%02x after writing.\n" % i2c_dev_adr
        sys.exit(1)
    # Generate stop condition.
    status = i2c.Stop()
    if status:
        print "ERROR: Unable to generate stop condition."
        sys.exit(1)
else:
    print "ERROR: Specify either 2..3 (for read) or 4 (for write) parameters!"

