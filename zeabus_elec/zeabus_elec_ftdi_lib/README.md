# Zeabus-Elec-2017-Code
Library for Power Distributer and Peripheral Bridge

## The base FTDI library
The base library is an open-source _libftdi_. It requires _libusb_ and _libconfuse_. 
Therefore, both dependencies were built and merged into the _libftdi.a_ library file. 
This makes the final library consists of 2 layers as:

* Physical layer consisting of _libftdi.a_ and _ftdi.h_.
* Implementation layer consisting of _ftdi\_impl.h_ and _ftdi\_impl.cpp_.

## Normally, Zeabus team use only Implementation layer and let the library manipulate the Physical layer.

## Pre-requisite
For Ubuntu16.04, we must install the _udev_ with the command `sudo apt-get install libudev-dev`.
The _libudev_ allow us to access the USB bus from user space with normal user privilege instead of `root`.
A rule file must be created in _/etc/udev/rules.d_ to allow that. However, we already provide the rule file under the directory _udev-rules_.
To install it, we just copy the files inside it to _/etc/udev/rules.d/_. We also need to prevent the Ubuntu from loading _ftdi\_sio_ modules
by copying the file under _modprobe-blacklist_ to _/etc/modprobe.d/_. Finally, restart the system.

The library requires "pthread" and "udev" libraries provided by the system.
Therefore, to build the program, we need _-lpthread -ludev_ option. 
For example `gcc main.c zblib.c -o runing libftd2xx -lpthread -ldev`.



## Zeabus Library
Zebus library was built over the base FTDI library. It provides high-level functions to control various components via FTxxx chips.

The components under control of FTxx are:

* _FT232H_ The chip must be pre-programmed its serial number as "PowerDist".
  * 8-Channel galvanic-isolated power switches assigned as:
    * 5 channels for thrusters, DVL, and Sonar (including spared channels) with directed connection to batteries
    * 1 channel for head lamps with 12V supply
    * 1 channel for DSP board with 5V supply
    * 1 reserved channel

* _FT4232H_ This chip consists of 4 independent modules Their serial numbers must be pre-programmed as "PerpheralBridge-A", "PerpheralBridge-B", "PerpheralBridge-C", and "PerpheralBridge-D".
  * 8-Channel galvanic-isolated switches for pneumatic valves.
  * 2-Channel galvanic-isolated RS232 for communication with DSP and DVL boards
