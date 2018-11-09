// 2018, Natalia Frumkin http://github.com/bu-vip/ActiLocate
// usb-reader-lib.h -- simple library for reading/writing serial ports
// adapted from:
// 2006-2013, Tod E. Kurt, http://todbot.com/blog/

#ifndef __USB_READER_LIB_H__
#define __USB_READER_LIB_H__

#include <stdint.h>   // Standard types 

int serialport_init(const char* serialport, int baud);
int serialport_close(int fd);
int serialport_read_until(int fd, char* buf, char until, int buf_max,int timeout);
int serialport_flush(int fd);

#endif

