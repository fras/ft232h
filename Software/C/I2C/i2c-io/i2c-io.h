// File: i2c-io.h
// Auth: M. Fras, Electronics Division, MPI for Physics, Munich
// Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
// Date: 05 Feb 2018
// Rev.: 09 Feb 2018
//
// Header file for raw I2C IO control program for the FTDI FH232H chip.
//



// Use I2C MPSSE library functions.
#define USE_LIBI2C_MPSSE
#ifdef USE_LIBI2C_MPSSE
#include "i2c_mpsse.h"
#endif

#define I2C_DATA_LEN_MAX        1024

//#define DEBUG_LEVEL 0
#define DEBUG_LEVEL 1
//#define DEBUG_LEVEL 2
//#define DEBUG_LEVEL 3
//#define DEBUG_LEVEL 4

#define PREFIX_DEBUG            "DEBUG: "
#define PREFIX_ERROR            "ERROR: "

