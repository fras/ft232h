Readm for the FTDI FT232H Hi-Speed Single Channel USB UART/FIFO IC
==================================================================
Auth: M. Fras, Electronics Division, MPI for Physics, Munich
Mod.: M. Fras, Electronics Division, MPI for Physics, Munich
Date: 05 Feb 2018
Rev.: 06 Feb 2018



Prerequisites
=============
* Install these software packages:
  - libusb
  - libftdi
  - swig
  - libmpsse
* If available on your platform, install the binary packages of libftdi and
  swig. E.g. on Ubuntu 16.04:
  $ apt install libusb-1.0-0 libftdi1 libftdi1-dev swig
* If there is no binary package available, build the packages:
  - libusb:
    $ tar xf libusb-1.0.21.tar.bz2
    $ cd libusb-1.0.21
    $ ./configure
    $ make
    $ make install
  - libftdi:
    $ tar xf libftdi1-1.4.tar.bz2
    $ cd libftdi1-1.4
    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_INSTALL_PREFIX="/usr/local" ../
    $ make
    $ make install
  - swig:
    $ tar xf swig-3.0.12.tar.gz
    $ cd swig-3.0.12
    $ ./configure
    $ make
    $ make install
* Finally, build libmpsse:
  - Hints:
    - If the configure complains about missing libftdi1 library, add the
      path to libftdi1 and its headers manually. E.g.:
      $ export CFLAGS="-I/usr/local/include/libftdi1"
      $ export LDFLAGS="-L/usr/local/lib/"
    - On CygWin, you need to manually add also the path to the python library
      for lining (otherwise, there will be a lot of errors saying "undefined
      reference to `PyString_...'):
      $ export LDFLAGS="-L/usr/local/lib/ -L/usr/lib/python2.7 -lpython2.7"
    - The Python header (package python2-dev) must be installed in order to
      build libmpsse with Python support.
    - On CygWin, you might need to edit the Makefile and manually delete all
      occurances of "$(DESTDIR)/" so that the installation can succeed.
  - Extract and build libmpsse:
    $ tar xf libmpsse-1.3.tar.gz
    $ cd libmpsse-1.3/src
    $ ./configure
    $ make
    $ make install
  - On CygWin, after "make install", you need to rename "_pylibmpsse.so" to
    "_pylibmpsse.dll" and make it executable:
    $ cd /usr/lib/python2.7/site-packages
    $ ln -s _pylibmpsse.so _pylibmpsse.dll
    $ chmod +x _pylibmpsse.dll
* Access to the FT232H device:
  - On Linux, by default only root can access the FT232H device. In order to
    allow access for users, add an udev rule:
    - Add/edit the file "/etc/udev/rules.d/90-ftdi.rules".
    - Add this content:
      "
      # FTDI-based devices
      #
      # FTDI vid=0403
      # FT2232, FT2232H: pid=6010
      # FT4232H:  pid=6011
      # FT232H:   pid=6014
      SUBSYSTEM=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6010|6011|6014", OWNER="root", GROUP="root", MODE="0666"
      "
  - On Windows/CygWin, you need to replace the FTDI driver with the WinUSB
    driver. Otherwise, access to the device will *NOT* work and you will get
    error messages like "Failed to open MPSSE: device not found".
    - Problem: By default, Windows installs the FT232H as "USB Serial
               Converter". This cannot be opened by libusb.
    - Solution:
      - Dowload the zadiag tool from "http://zadig.akeo.ie/"
      - Run the tool.
      - Select "Options"->"List all devices".
      - Choose "UM232H-B" from the drop down box.
      - Chosse "WinUSB (v...)" as replacement.
      - Click "Replace Driver".
      - If a security window open, click "Install".
      - Note that in the Windows device manager, there is now a "UM232H-B"
        device under "Universal Serial Bus devices".



Gotchas, Problems, Troubleshooting
==================================
* I2C
  - FTDI FT232H pinning:
    - ADBUS0 (13): SCL
    - ADBUS1 (14): SDA output
    - ADBUS2 (15): SDA input
  - The pins ADBUS1 (14) and ADBUS2 (15) *must* be tied together! Otherwise,
    either no data will be driven onto SDA or only a constant high signal level
    (i.e. NACK, 0xFF) will be seen by the FT232H chip!
  - Under CygWin make sure that the FTDI FT232H device is *not* shown as "USB
    Serial Converter" in the device mananger. For details and a solution, see
    the section "Prerequisites".

