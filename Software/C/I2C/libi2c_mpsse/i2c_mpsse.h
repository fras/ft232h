// File: i2c_mpsse.h
// Auth: M. Fras, Electronics Division, MPI for Physics, Munich
// Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
// Date: 09 Feb 2018
// Rev.: 09 Feb 2018
//
// Header file for the basic hardware I2C IO functions based on FTDI's
// Multi-Protocol Synchronous Serial Engine (MPSSE).
//



// Function prototypes.
int i2c_init(void);
int i2c_reset(void);
int i2c_close(void);
int i2c_info(void);
int i2c_get_freq(int *i2c_freq);
int i2c_set_freq(int i2c_freq);
int i2c_set_verbose(int verbose);
int i2c_write(int i2c_dev_adr, char *data, int size);
int i2c_read(int i2c_dev_adr, char *data, int size);

