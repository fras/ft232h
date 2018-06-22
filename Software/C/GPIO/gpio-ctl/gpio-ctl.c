// File: gpio-ctl.c
// Auth: M. Fras, Electronics Division, MPI for Physics, Munich
// Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
// Date: 16 Feb 2018
// Rev.: 22 Jun 2018
//
// GPIO control program for the FTDI FH232H chip using FTDI's Multi -
// Protocol Synchronous Serial Engine (MPSSE).
//
// FTDI FT232H pinning:
// - ADBUS4(17): GPIOL0
// - ADBUS5(18): GPIOL1
// - ADBUS6(19): GPIOL2
// - ADBUS7(20): GPIOL3
// - ACBUS0(21): GPIOH0
// - ACBUS1(25): GPIOH1
// - ACBUS2(26): GPIOH2
// - ACBUS3(27): GPIOH3
// - ACBUS4(28): GPIOH4
// - ACBUS5(29): GPIOH5
// - ACBUS6(30): GPIOH6
// - ACBUS7(31): GPIOH7
//



#include <stdio.h>
#include <stdlib.h>
#include <mpsse.h>
#include "gpio-ctl.h"



// Function protoypes.
int show_help(char* prog_name);



int main(int argc, char **argv)
{
    int status;
    // GPIO pins data and mask
    int gpio_data;
    int gpio_mask;

    // Check command line arguments.
    if(argc > 1) {
        if(!strncmp(argv[1], "-h", 2) || !strncmp(argv[1], "--h", 3)) {
            show_help(argv[0]);
            return 1;
        }
    }

    // Initialize the GPIO device.
    // CAUTION: Calling gpio_init() resets all GPIO output levels to low!
    status = gpio_init();
    if(status) {
        printf("%sUnable to open the GPIO device.\n", PREFIX_ERROR);
        return 1;
    }
    // Set verbosity of the GPIO library functions.
    gpio_set_verbose(1);

    // Show device information.
    #if DEBUG_LEVEL >= 1
    gpio_info();
    #endif

    // Get the input levels of the GPIO pins.
    if(argc == 1) {
        #if DEBUG_LEVEL >= 3
        printf("%sGetting the input levels of the GPIO pins.\n", PREFIX_DEBUG);
        #endif
        status = gpio_get_pins(&gpio_data);
        if(status) {
            printf("%sUnable to get the input levels of the GPIO pins.\n", PREFIX_ERROR);
            return 1;
        }
        // Print the GPIO pin levels.
        printf("0x%03x\n", gpio_data);
    // Set the output levels of the GPIO pins without masking bits.
    } else if(argc == 2) {
        gpio_data = strtoul(argv[1], NULL, 0) & 0xfff;  // 12 GPIO pins available.
        gpio_mask = 0xfff;                              // 12 GPIO pins available.
        #if DEBUG_LEVEL >= 3
        printf("%sSetting the output levels of the GPIO pins to 0x%03x.\n", PREFIX_DEBUG, gpio_data);
        #endif
        status = gpio_set_pins(gpio_data, gpio_mask);
        if(status) {
            printf("%sUnable to set the output levels of the GPIO pins to 0x%03x.\n", PREFIX_DEBUG, gpio_data);
            return 1;
        }
    // Set the output levels of the GPIO pins with masking bits.
    } else if(argc == 3) {
        gpio_data = strtoul(argv[1], NULL, 0) & 0xfff;  // 12 GPIO pins available.
        gpio_mask = strtoul(argv[2], NULL, 0) & 0xfff;  // 12 GPIO pins available.
        #if DEBUG_LEVEL >= 3
        printf("%sSetting the output levels of the GPIO pins to 0x%03x with mask 0x%03x.\n", PREFIX_DEBUG, gpio_data, gpio_mask);
        #endif
        status = gpio_set_pins(gpio_data, gpio_mask);
        if(status) {
            printf("%sUnable to set the output levels of the GPIO pins to 0x%03x with mask 0x%03x.\n", PREFIX_DEBUG, gpio_data, gpio_mask);
            return 1;
        }
    } else {
        printf("%sSpecify 0..2 parameters!", PREFIX_ERROR);
        return 1;
    }

    // Close the GPIO device.
    // CAUTION: After calling gpio_close(), all GPIO pins will be set to high!
//    status = gpio_close();
//    if(status) {
//        printf("%sUnable to close the GPIO device.\n", PREFIX_DEBUG);
//        return 1;
//    }

    return 0;
}



// Show help message.
int show_help(char* prog_name)
{
    printf("GPIO control program\n");
    printf("\n");
    printf("Usage: %s [GPIO-DATA] [GPIO-MASK]\n", prog_name);
    return 0;
}

