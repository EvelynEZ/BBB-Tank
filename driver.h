// Header file for driver.c
#ifndef DRIVER_H 
#define DRIVER_H

#include <stdio.h>      // for File IO and printf
#include <stdlib.h>     // for standard library
#include <math.h>       // for math related operations
#include <time.h>       // for usleep
#include <string.h>     // for string related operations
#include <ctype.h>      // for testing characters for membership in a particular class of characters
#include <fcntl.h>      // for open 
#include <sys/stat.h>   // defines the structure of the data returned by the functions fstat(), lstat(), and stat()
#include <unistd.h>     // for read, write and close files
#include <sys/types.h>  // for define data types
#include <sys/socket.h> // for socket interface
#include <netinet/in.h> // for unsigned char and sockaddr_in structure
#include <sys/signal.h> // for signal operation fuctions
#include <errno.h>      // for printing error message
#include <termios.h>    // for the terminal I/O interfaces
#include <sys/time.h>   // defines the timeval structure


#define ALL_PINS 9      // Sets ALL_PINS = 9
// GPIOs for Shift Register
#define SER 26          // SER   ---- GPIO_PIN_26
#define RS 46           // RS    ---- GPIO_PIN_46
#define RCLK 65         // RCLK  ---- GPIO_PIN_65
#define E 61            // E     ---- GPIO_PIN_61
#define SRCLK 27        // SRCLK ---- GPIO_PIN_27

// GPIOs for Motor Control
#define AIN1 66 		// AIN1  ---- GPIO_PIN_66
#define AIN2 67 		// AIN2  ---- GPIO_PIN_67
#define BIN1 69			// BIN1  ---- GPIO_PIN_69
#define BIN2 68		    // BIN2  ---- GPIO_PIN_68


static int carRow;     //Current postion of the tank
static int carCol;
static int direction;  //Current direction
static int fd;
static int map [8][8]; //An 8*8 Maze
static FILE *sys, *sys2, *PWMAduty, *PWMAperiod, *PWMBduty, *PWMBperiod, *sensor *aval;  //Pointers	declaration
static FILE *dir[ALL_PINS];
static FILE *value[ALL_PINS];
static char path3[50] = "/sys/bus/iio/devices/iio:device0/in_voltage0_raw";  //BBB AIN0 path
static char path4[50] = "/sys/bus/iio/devices/iio:device0/in_voltage2_raw";  //BBB AIN2 path
static void moveForwards();
static void moveBackwards();
static void stop();
static void controlLeft(int, int);
static void controlRight(int, int);
static void turnLeft();
static void turnRight();
static int sensorDistance(char *);
static void enableVal(void);
static void write_com(unsigned char);
static void write_data(unsigned char);
static void fcnSet(unsigned char);
static void displayOff(void);
static void displayOn(void);
static void clearDisplay(void);
static void initialization(void);
static void switchRCLK(void); 
static void switchSRCLK(void);
static void info(unsigned char[]); 
static void initialization();
static void win();
static void closeAll();
static void playGame();
static void gameOver();
static void showMap();
static void move(int);
static int readFromKeyboard();

#endif




	
