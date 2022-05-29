#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdint.h>
///#include <Arduino.h>

///#define __cplusplus 1

//#ifdef IS_LITTLE_ENDIAN
#if 1
#define LO 0
#define HI 1
#else
#define LO 1
#define HI 0
#endif

typedef unsigned char byte;

typedef unsigned char un8;
typedef unsigned short un16;
typedef unsigned int un32;

typedef signed char n8;
typedef signed short n16;
typedef signed int n32;


#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t byte;
typedef uint16_t addr; // 16-bit address space for the gameboy

/* stuff from main.c ... */
#define die(fmt, ...) do { \
	ets_printf("%s:%d died: " fmt "\n", __FILE__, __LINE__, ## __VA_ARGS__); \
	vTaskDelay(1000); \
} while(1)

//void die(char *fmt, ...);
void doevents();
void pad_press(byte k);
void pad_release(byte k);

#ifdef __cplusplus
};
#endif

#endif
