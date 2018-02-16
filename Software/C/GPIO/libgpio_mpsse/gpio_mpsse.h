// File: gpio_mpsse.h
// Auth: M. Fras, Electronics Division, MPI for Physics, Munich
// Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
// Date: 16 Feb 2018
// Rev.: 16 Feb 2018
//
// Header file for the basic hardware GPIO IO functions based on FTDI's
// Multi-Protocol Synchronous Serial Engine (MPSSE).
//



#ifndef __GPIO_MPSSE_H
#define __GPIO_MPSSE_H



// Message prefixes.
#define PREFIX_DEBUG            "DEBUG: "
#define PREFIX_ERROR            "ERROR: "



// Function prototypes.
int gpio_init(void);
int gpio_reset(void);
int gpio_close(void);
int gpio_info(void);
int gpio_set_verbose(int verbose);
int gpio_set_pins(int gpio_data, int gpio_mask);
int gpio_get_pins(int *gpio_data);



#endif

