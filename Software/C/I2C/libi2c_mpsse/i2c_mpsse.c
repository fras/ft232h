// File: i2c_mpsse.c
// Auth: M. Fras, Electronics Division, MPI for Physics, Munich
// Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
// Date: 09 Feb 2018
// Rev.: 12 Feb 2018
//
// Basic hardware I2C IO functions based on FTDI's Multi-Protocol Synchronous
// Serial Engine (MPSSE).
//



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpsse.h>
#include "i2c_mpsse.h"



// Global variables.
struct mpsse_context *i2c_mpsse = NULL;
int i2c_mpsse_verbose = 1;



// Initialize the I2C hardware.
int i2c_init(void)
{
    // Check if the I2C device was already initialized.
    if(i2c_mpsse != NULL) return 0;

    // Open the I2C device with default frequency of 100 kHz.
    if(!((i2c_mpsse = MPSSE(I2C, ONE_HUNDRED_KHZ, MSB)) != NULL && i2c_mpsse->open))
    {
        fprintf(stderr, "%s: %s: %sFailed to initialize MPSSE: %s\n", __FILE__, __FUNCTION__, PREFIX_ERROR, ErrorString(i2c_mpsse));
        return -1;
    }

    return 0;
}



// Reset the I2C hardware.
int i2c_reset(void)
{
    // This is a dummy function, no operation.
    return 0;
}



// Close the I2C hardware.
int i2c_close(void)
{
    Close(i2c_mpsse);

    return 0;
}



// Get the I2C frequency.
int i2c_get_freq(int *i2c_freq)
{
    // Check if the I2C device was initialized.
    if(i2c_mpsse == NULL) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sThe I2C device was not properly initialized.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    *i2c_freq = GetClock(i2c_mpsse);

    return 0;
}



// Set the I2C frequency.
int i2c_set_freq(int i2c_freq)
{
    int status;

    // Check if the I2C device was initialized.
    if(i2c_mpsse == NULL) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sThe I2C device was not properly initialized.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    status = SetClock(i2c_mpsse, i2c_freq);
    if(status) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to set the I2C frequency to %d Hz.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i2c_freq);
        return -1;
    }

    return 0;
}



// Get information about the I2C device.
int i2c_info(void)
{
    // Check if the I2C device was initialized.
    if(i2c_mpsse == NULL) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sThe I2C device was not properly initialized.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    printf("I2C master device: %s\n", GetDescription(i2c_mpsse));
    printf("I2C master device VID: 0x%04x\n", GetVid(i2c_mpsse));
    printf("I2C master device PID: 0x%04x\n", GetPid(i2c_mpsse));
    printf("I2C bus speed: %d Hz\n", GetClock(i2c_mpsse));

    return 0;
}



// Set verbosity of the I2C functions.
int i2c_set_verbose(int verbose)
{
    i2c_mpsse_verbose = verbose;
    return 0;
}



// Write data to the I2C bus.
int i2c_write(int i2c_dev_adr, char *data, int size)
{
    int status;
    char i2c_data[2];

    // Check if the I2C device was initialized.
    if(i2c_mpsse == NULL) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sThe I2C device was not properly initialized.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    // Generate start condition.
    status = Start(i2c_mpsse);
    if(status) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to generate start condition.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    // Send device address with write command.
    i2c_data[0] = ((i2c_dev_adr & 0x7f) << 1) | 0x00;
    status = Write(i2c_mpsse, i2c_data, 1);
    if(status) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to set the I2C chip address 0x%02x.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i2c_dev_adr);
        return -1;
    }

    // Check for acknowledge.
    if(GetAck(i2c_mpsse) != ACK) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sDid not get acknowledge from the I2C chip address 0x%02x.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i2c_dev_adr);
        return -1;
    }

    // Send the I2C data.
    status = Write(i2c_mpsse, data, size);
    if(status) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to write %d byte(s) to the I2C chip address 0x%02x.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, size, i2c_dev_adr);
        return -1;
    }

    // Check for acknowledge.
    if(GetAck(i2c_mpsse) != ACK) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sDid not get acknowledge from the I2C chip address 0x%02x after writing data.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i2c_dev_adr);
        return -1;
    }

    // Generate stop condition.
    status = Stop(i2c_mpsse);
    if(status) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to generate stop condition.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    return 0;
}



// Read data from the I2C bus.
int i2c_read(int i2c_dev_adr, char *data, int size)
{
    int status;
    char i2c_data[2];
    char *i2c_data_ptr = NULL;    

    // Check if the I2C device was initialized.
    if(i2c_mpsse == NULL) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sThe I2C device was not properly initialized.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    // Generate start condition.
    status = Start(i2c_mpsse);
    if(status) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to generate start condition.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    // Send device address with read command.
    i2c_data[0] = ((i2c_dev_adr & 0x7f) << 1) | 0x01;
    status = Write(i2c_mpsse, i2c_data, 1);
    if(status) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to set the I2C chip address 0x%02x.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i2c_dev_adr);
        return -1;
    }

    // Check for acknowledge.
    if(GetAck(i2c_mpsse) != ACK) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sDid not get acknowledge from the I2C chip address 0x%02x.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i2c_dev_adr);
        return -1;
    }

    // Read from the I2C bus.
    i2c_data_ptr = Read(i2c_mpsse, size);
    if(i2c_data_ptr == NULL) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to read %d byte(s) from the I2C chip address 0x%02x.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, size, i2c_dev_adr);
        return -1;
    }

    // Check for acknowledge.
    if(GetAck(i2c_mpsse) != ACK) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sDid not get acknowledge from the I2C chip address 0x%02x after reading data.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i2c_dev_adr);
        return -1;
    }

    // Generate stop condition.
    status = Stop(i2c_mpsse);
    if(status) {
        if(i2c_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sUnable to generate stop condition.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    // Copy the data read to the buffer passed to this function as argument and
    // free the memory that has been reserved by the Read function.
    strncpy(data, i2c_data_ptr, size);
    free(i2c_data_ptr);

    return 0;
}

