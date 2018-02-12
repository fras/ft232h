// File: i2c-io.c
// Auth: M. Fras, Electronics Division, MPI for Physics, Munich
// Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
// Date: 05 Feb 2018
// Rev.: 12 Feb 2018
//
// Raw I2C IO control program for the FTDI FH232H chip using FTDI's Multi -
// Protocol Synchronous Serial Engine (MPSSE.
//
// FTDI FT232H pinning:
// - ADBUS0(13): SCL
// - ADBUS1(14): SDA output
// - ADBUS2(15): SDA input
//
// CAUTION:
// The pins ADBUS1(14) and ADBUS2(15) *must* be tied together! Otherwise,
// either no data will be driven onto SDA or only a constant high signal level
// (i.e. NACK, 0xFF) will be received!
//



#include <stdio.h>
#include <stdlib.h>
#include <mpsse.h>
#include "i2c-io.h"



// Function protoypes.
int show_help(char* prog_name);



int main(int argc, char **argv)
{
    int i;
    int status;
    // FTDI I2C hardware.
    #ifndef USE_LIBI2C_MPSSE
    struct mpsse_context *mpsse_i2c = NULL;
    #endif
    int i2c_freq = ONE_HUNDRED_KHZ;
//    int i2c_freq = FOUR_HUNDRED_KHZ;
    // I2C address and data.
    int i2c_dev_adr;
    int i2c_data_adr = 0;
    char i2c_data[I2C_DATA_LEN_MAX+1];
    #ifndef USE_LIBI2C_MPSSE
    char *i2c_data_ptr = NULL;
    #endif
    int i2c_data_len;

    // Check command line arguments.
    if(argc < 2) {
        show_help(argv[0]);
        return 1;
    }
    i2c_dev_adr = (int)(strtoul(argv[1], NULL, 0) & 0x7f);
    if(argc > 2) {
        i2c_data_adr = (int)(strtoul(argv[2], NULL, 0) & 0xff);
    }
    i2c_data_len = argc - 3;
    if(i2c_data_len > I2C_DATA_LEN_MAX)
        i2c_data_len = I2C_DATA_LEN_MAX;

    #ifdef USE_LIBI2C_MPSSE
    // Initialize the I2C master device.
    status = i2c_init();
    if(status) {
        printf("%sUnable to open the I2C device.\n", PREFIX_ERROR);
        return 1;
    }
    // Set the I2C bus frequency.
    status = i2c_set_freq(i2c_freq);
    if(status) {
        printf("%sUnable to set the I2C frequency to %d Hz.\n", PREFIX_ERROR, i2c_freq);
        return 1;
    }
    // Set verbosity of the I2C library functions.
    i2c_set_verbose(1);
    #else
    // Open the I2C master.
    if(!((mpsse_i2c = MPSSE(I2C, i2c_freq, MSB)) != NULL && mpsse_i2c->open))
    {
        printf("%sFailed to initialize MPSSE: %s\n", PREFIX_ERROR, ErrorString(mpsse_i2c));
        return -1;
    }
    #endif

    // Show device information.
    #if DEBUG_LEVEL >= 1
    #ifdef USE_LIBI2C_MPSSE
    i2c_info();
    #else
    printf("I2C master device: %s\n", GetDescription(mpsse_i2c));
    printf("I2C master device VID: 0x%04x\n", GetVid(mpsse_i2c));
    printf("I2C master device PID: 0x%04x\n", GetPid(mpsse_i2c));
    printf("I2C bus speed: %d Hz\n", GetClock(mpsse_i2c));
    #endif
    #endif

    // I2C read with chip address only.
    if(argc == 2) {
        #if DEBUG_LEVEL >= 3
        printf("%sI2C read from chip address 0x%02x.\n", PREFIX_DEBUG, i2c_dev_adr);
        #endif
        #ifdef USE_LIBI2C_MPSSE
        // Read 1 byte from the I2C device.
        i2c_data_len = 1;
        status = i2c_read(i2c_dev_adr, i2c_data, i2c_data_len);
        if(status) {
            printf("%sUnable to read %d byte(s) from the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_data_len, i2c_dev_adr);
            return 1;
        }
        // Print data read from I2C.
        printf("0x%02x\n", i2c_data[0] & 0xff);
        #else
        // Generate start condition.
        status = Start(mpsse_i2c);
        if(status) {
            printf("%sUnable to generate start condition.\n", PREFIX_ERROR);
            return 1;
        }
        // Send device address with read command.
        i2c_data[0] = ((i2c_dev_adr & 0x7f) << 1) | 0x01;
        status = Write(mpsse_i2c, i2c_data, 1);
        if(status) {
            printf("%sUnable to set the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Check for acknowledge.
        if(GetAck(mpsse_i2c) != ACK) {
            printf("%sDid not get acknowledge from the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Read from the I2C bus.
        i2c_data_len = 1;
        i2c_data_ptr = Read(mpsse_i2c, i2c_data_len);
        if(i2c_data_ptr == NULL) {
            printf("%sUnable to read %d byte(s) from the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_data_len, i2c_dev_adr);
            return 1;
        }
        // Check for acknowledge.
        if(GetAck(mpsse_i2c) != ACK) {
            printf("%sDid not get acknowledge from the I2C chip address 0x%02x after reading data.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Generate stop condition.
        status = Stop(mpsse_i2c);
        if(status) {
            printf("%sUnable to generate stop condition.\n", PREFIX_ERROR);
            return 1;
        }
        // Print data read from I2C.
        printf("0x%02x\n",(*i2c_data_ptr) & 0x000000ff);
        free(i2c_data_ptr);
        #endif
    // I2C read with chip address and data address.
    } else if(argc == 3) {
        #if DEBUG_LEVEL >= 3
        printf("%sI2C read from chip address 0x%02x, data address 0x%02x.\n", PREFIX_DEBUG, i2c_dev_adr, i2c_data_adr);
        #endif
        #ifdef USE_LIBI2C_MPSSE
        // Set the I2C data address.
        i2c_data[0] = i2c_data_adr & 0xff;
        status = i2c_write(i2c_dev_adr, i2c_data, 1);
        if(status) {
            printf("%sUnable to set the I2C data address 0x%02x for device address 0x%02x.\n", PREFIX_ERROR, i2c_data_adr, i2c_dev_adr);
            return 1;
        }
        // Read 1 byte from the I2C device.
        i2c_data_len = 1;
        status = i2c_read(i2c_dev_adr, i2c_data, i2c_data_len);
        if(status) {
            printf("%sUnable to read %d byte(s) from the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_data_len, i2c_dev_adr);
            return 1;
        }
        // Print data read from I2C.
        printf("0x%02x\n", i2c_data[0] & 0xff);
        #else
        // Generate start condition.
        status = Start(mpsse_i2c);
        if(status) {
            printf("%sUnable to generate start condition.\n", PREFIX_ERROR);
            return 1;
        }
        // Send device address with write command.
        i2c_data[0] = ((i2c_dev_adr & 0x7f) << 1) | 0x00;
        status = Write(mpsse_i2c, i2c_data, 1);
        if(status) {
            printf("%sUnable to set the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Check for acknowledge.
        if(GetAck(mpsse_i2c) != ACK) {
            printf("%sDid not get acknowledge from the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Write the data address
        i2c_data[0] = i2c_data_adr & 0xff;
        status = Write(mpsse_i2c, i2c_data, 1);
        if(status) {
            printf("%sUnable to set the I2C data address 0x%02x for device address 0x%02x.\n", PREFIX_ERROR, i2c_data_adr, i2c_dev_adr);
            return 1;
        }
        // Check for acknowledge.
        if(GetAck(mpsse_i2c) != ACK) {
            printf("%sDid not get acknowledge from the I2C chip address 0x%02x, data address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr, i2c_data_adr);
            return 1;
        }
        //(Re-)Generate start condition.
        status = Start(mpsse_i2c);
        if(status) {
            printf("%sUnable to(re-)generate start condition.\n", PREFIX_ERROR);
            return 1;
        }
        // Send device address with read command.
        i2c_data[0] = ((i2c_dev_adr & 0x7f) << 1) | 0x01;
        status = Write(mpsse_i2c, i2c_data, 1);
        if(status) {
            printf("%sUnable to set the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Check for acknowledge.
        if(GetAck(mpsse_i2c) != ACK) {
            printf("%sDid not get acknowledge from the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Read from the I2C bus.
        i2c_data_len = 1;
        i2c_data_ptr = Read(mpsse_i2c, i2c_data_len);
        if(i2c_data_ptr == NULL) {
            printf("%sUnable to read %d byte(s) from the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_data_len, i2c_dev_adr);
            return 1;
        }
        // Check for acknowledge.
        if(GetAck(mpsse_i2c) != ACK) {
            printf("%sDid not get acknowledge from the I2C chip address 0x%02x, data address 0x%02x after reading data.\n", PREFIX_ERROR, i2c_dev_adr, i2c_data_adr);
            return 1;
        }
        // Generate stop condition.
        status = Stop(mpsse_i2c);
        if(status) {
            printf("%sUnable to generate stop condition.\n", PREFIX_ERROR);
            return 1;
        }
        // Print data read from I2C.
        printf("0x%02x\n",(*i2c_data_ptr) & 0x000000ff);
        free(i2c_data_ptr);
        #endif
    // I2C write.
    } else if(argc >= 4) {
        #if DEBUG_LEVEL >= 3
        printf("%sI2C write to chip address 0x%02x, data address 0x%02x.\n", PREFIX_DEBUG, i2c_dev_adr, i2c_data_adr);
        printf("%sData: ", PREFIX_DEBUG);
        for(i = 0; i < i2c_data_len; i++) {
            printf("0x%02x ",(char)(strtoul(argv[i+3], NULL, 0) & 0xff));
        }
        printf("\n");
        #endif
        #ifdef USE_LIBI2C_MPSSE
        // Prepare the I2C data.
        i2c_data[0] = (char)(i2c_data_adr & 0xff);
        for(i = 0; i < i2c_data_len; i++)
            i2c_data[i+1] = (char)(strtoul(argv[i+3], NULL, 0) & 0xff);
        // Send the I2C data.
        status = i2c_write(i2c_dev_adr, i2c_data, i2c_data_len + 1);
        if(status) {
            printf("%sUnable to write %d byte(s) to the I2C chip address 0x%02x, data address 0x%02x.\n", PREFIX_ERROR, i2c_data_len, i2c_dev_adr, i2c_data_adr);
            return 1;
        }
        #else
        // Generate start condition.
        status = Start(mpsse_i2c);
        if(status) {
            printf("%sUnable to generate start condition.\n", PREFIX_ERROR);
            return 1;
        }
        // Send device address with write command.
        i2c_data[0] = ((i2c_dev_adr & 0x7f) << 1) | 0x00;
        status = Write(mpsse_i2c, i2c_data, 1);
        if(status) {
            printf("%sUnable to set the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Check for acknowledge.
        if(GetAck(mpsse_i2c) != ACK) {
            printf("%sDid not get acknowledge from the I2C chip address 0x%02x.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Prepare the I2C data.
        i2c_data[0] = (char)(i2c_data_adr & 0xff);
        for(i = 0; i < i2c_data_len; i++)
            i2c_data[i+1] = (char)(strtoul(argv[i+3], NULL, 0) & 0xff);
        // Send the I2C data.
        status = Write(mpsse_i2c, i2c_data, i2c_data_len + 1);
        if(status) {
            printf("%sUnable to write %d byte(s) to the I2C chip address 0x%02x, data address 0x%02x.\n", PREFIX_ERROR, i2c_data_len, i2c_dev_adr, i2c_data_adr);
            return 1;
        }
        // Check for acknowledge.
        if(GetAck(mpsse_i2c) != ACK) {
            printf("%sDid not get acknowledge from the I2C chip address 0x%02x after writing data.\n", PREFIX_ERROR, i2c_dev_adr);
            return 1;
        }
        // Generate stop condition.
        status = Stop(mpsse_i2c);
        if(status) {
            printf("%sUnable to generate stop condition.\n", PREFIX_ERROR);
            return 1;
        }
        #endif
    } else {
        printf("%sSpecify either 2..3(for read) or 4(for write) parameters!", PREFIX_ERROR);
        return 1;
    }

    // Close the I2C device.
    #ifdef USE_LIBI2C_MPSSE
    i2c_close();
    #else
    Close(mpsse_i2c);
    #endif

    return 0;
}



// Show help message.
int show_help(char* prog_name)
{
    printf("Raw I2C IO control program (read/write)\n");
    printf("\n");
    printf("Usage: %s CHIP-ADR [DATA-ADR] [DATA]\n", prog_name);
    return 0;
}

