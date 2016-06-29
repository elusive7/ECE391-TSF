#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "lib.h"

/* RTC Constants */
#define RTC 0x08
#define RTC_INDEX 0x70
#define RTC_DATA 0x71
#define STATUS_A 0x0a
#define STATUS_B 0x0b
#define STATUS_C 0x0c
#define NMI_DISABLE 0x80
#define INT_ON 0x40
#define RATE_MASK 0xF0
#define TWO_HZ 0x0F

#define FREQ_MIN 2
#define FREQ_MAX 1024
#define THEORY_MAX 32768

#define LOW 0
#define HIGH 1

/* RTC Initialization */
void rtc_init(void);

/* RTC Handler */ 
void rtc_handler(void);

/* Terminal System Calls */
int32_t rtc_open(const uint8_t * filename);
int32_t rtc_read(int32_t fd, char * buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const char * buf, int32_t nbytes);
int32_t rtc_close(int32_t fd);

#endif




