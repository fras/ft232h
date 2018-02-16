// File: gpio_mpsse.c
// Auth: M. Fras, Electronics Division, MPI for Physics, Munich
// Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
// Date: 16 Feb 2018
// Rev.: 16 Feb 2018
//
// Basic hardware GPIO functions based on FTDI's Multi-Protocol Synchronous
// Serial Engine (MPSSE).
//



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpsse.h>
#include "gpio_mpsse.h"



// Global variables.
struct mpsse_context *gpio_mpsse = NULL;
int gpio_mpsse_verbose = 1;

// GPIO pin list.
#define GPIO_MPSSE_PIN_COUNT (4 + 8)
const int gpio_mpsse_pin[] =
{
    GPIOL0,
    GPIOL1,
    GPIOL2,
    GPIOL3,
    GPIOH0,
    GPIOH1,
    GPIOH2,
    GPIOH3,
    GPIOH4,
    GPIOH5,
    GPIOH6,
    GPIOH7
};



// Initialize the GPIO hardware.
// CAUTION: Calling gpio_init() resets all GPIO output levels to low!
int gpio_init(void)
{
    // Check if the GPIO device was already initialized.
    if(gpio_mpsse != NULL) return 0;

    // Open the GPIO device with default frequency of 100 kHz.
    if(!((gpio_mpsse = MPSSE(GPIO, 0, 0)) != NULL && gpio_mpsse->open))
    {
        fprintf(stderr, "%s: %s: %sFailed to initialize MPSSE: %s\n", __FILE__, __FUNCTION__, PREFIX_ERROR, ErrorString(gpio_mpsse));
        return -1;
    }

    return 0;
}



// Reset the GPIO hardware.
int gpio_reset(void)
{
    // This is a dummy function, no operation.
    return 0;
}



// Close the GPIO hardware.
// CAUTION: After calling gpio_close(), all GPIO pins will be set to high!
int gpio_close(void)
{
    Close(gpio_mpsse);
    gpio_mpsse = NULL;

    return 0;
}



// Get information about the GPIO device.
int gpio_info(void)
{
    // Check if the GPIO device was initialized.
    if(gpio_mpsse == NULL) {
        if(gpio_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sThe GPIO device was not properly initialized.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return -1;
    }

    printf("GPIO master device: %s\n", GetDescription(gpio_mpsse));
    printf("GPIO master device VID: 0x%04x\n", GetVid(gpio_mpsse));
    printf("GPIO master device PID: 0x%04x\n", GetPid(gpio_mpsse));

    return 0;
}



// Set verbosity of the GPIO functions.
int gpio_set_verbose(int verbose)
{
    gpio_mpsse_verbose = verbose;
    return 0;
}



// Set the output levels of the GPIO pins.
int gpio_set_pins(int gpio_data, int gpio_mask)
{
    int i;
    int status;

    // Check if the GPIO device was initialized.
    if(gpio_mpsse == NULL) {
        if(gpio_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sThe GPIO device was not properly initialized.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return 1;
    }

    // Loop over all GPIO pins.
    for(i = 0; i < GPIO_MPSSE_PIN_COUNT; i++) {
        // GPIO pin selected.
        if(((gpio_mask >> i) & 0x1) == 1) {
            // Set GPIO pin low or high, according to data value.
            if(((gpio_data >> i) & 0x1) == 0)
                status = PinLow(gpio_mpsse, gpio_mpsse_pin[i]);
            else
                status = PinHigh(gpio_mpsse, gpio_mpsse_pin[i]);
            if(status) {
                if(gpio_mpsse_verbose)
                    fprintf(stderr, "%s: %s: %sUnable to set the output level of GPIO pin %d.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i);
                return 1;
            }
        }
    }

    return 0;
}



// Get the input levels of the GPIO pins.
// CAUTION: Only the state of the first 4 GPIO pins (GPIOL0..GPIOL3) can be
//          read! All other states will always read 0!
int gpio_get_pins(int *gpio_data)
{
    int i;
    int status;
    int gpio_data_tmp;

    // Check if the GPIO device was initialized.
    if(gpio_mpsse == NULL) {
        if(gpio_mpsse_verbose)
            fprintf(stderr, "%s: %s: %sThe GPIO device was not properly initialized.\n", __FILE__, __FUNCTION__, PREFIX_ERROR);
        return 1;
    }

    // Loop over all GPIO pins.
    gpio_data_tmp = 0;
    for(i = 0; i < GPIO_MPSSE_PIN_COUNT; i++) {
        // Get the state of the current pin.
        status = PinState(gpio_mpsse, gpio_mpsse_pin[i], -1);
        if(status < 0) {
            if(gpio_mpsse_verbose)
                fprintf(stderr, "%s: %s: %sUnable to get the input level of GPIO pin %d.\n", __FILE__, __FUNCTION__, PREFIX_ERROR, i);
            return status;
        }
        // Store the current input level
        gpio_data_tmp |= (status & 0x1) << i;
    }

    *gpio_data = gpio_data_tmp;

    return 0;
}

