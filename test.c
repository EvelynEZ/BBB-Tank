#include "driver.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <errno.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/time.h>

static void move(int);

void signal_handler_IO (int status);
struct termios termAttr; 
struct sigaction saio;
int fd;
FILE *aval;

int main(int argc, char *argv[]) {


     fd = open("/dev/ttyO1", O_RDWR | O_NOCTTY | O_NDELAY);

     if (fd == -1)
     {
        perror("open_port: Unable to open /dev/ttyO1\n"); 
        exit(1);
     }
     
     //Set baud rate to be 115200 and mode to be slave mode 
     saio.sa_handler = signal_handler_IO;//Initiating bluetooth
     saio.sa_flags = 0;
     saio.sa_restorer = NULL; 
     sigaction(SIGIO,&saio,NULL);

     fcntl(fd, F_SETFL, FNDELAY);
     fcntl(fd, F_SETOWN, getpid());
     fcntl(fd, F_SETFL,  O_ASYNC ); 

     tcgetattr(fd,&termAttr);
     cfsetispeed(&termAttr,B115200);
     cfsetospeed(&termAttr,B115200);
     termAttr.c_cflag &= ~PARENB;
     termAttr.c_cflag &= ~CSTOPB;
     termAttr.c_cflag &= ~CSIZE;
     termAttr.c_cflag |= CS8;
     termAttr.c_cflag |= (CLOCAL | CREAD);
     termAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
     termAttr.c_iflag &= ~(IXON | IXOFF | IXANY);
     termAttr.c_oflag &= ~OPOST;
     tcsetattr(fd,TCSANOW,&termAttr);

	 //unsigned char str[100] = "UART1 configured"; //, initializing
    //info(str);

    // Go throught the start up process for tank	



    ///////////////////////GPIO//////////////////////
	// Creates an array for easier GPIO access
	int gpioPins[ALL_PINS] = {AIN1, AIN2, BIN1, BIN2, SER, RCLK, SRCLK, RS, E};

	// Specifies the file that the pointer will be used to write in.
	sys = fopen("/sys/class/gpio/export", "w");
	fseek(sys, 0, SEEK_SET);

	// Writes the value corresponding to the GPIO digital pins used
	int i;
	for(i = 0; i < ALL_PINS; i++) {
		fprintf(sys, "%d", gpioPins[i]);
		fflush(sys);
	}

	// Sets the direcion of each GPIO digital pin to output
	// Specifies the file that is used to write in the values
	char path1[50];
	char path2[50];
	

	int j;
	for(j = 0; j < ALL_PINS; j++) {
		sprintf(path1, "/sys/class/gpio/gpio%d/direction", gpioPins[j]);
		sprintf(path2, "/sys/class/gpio/gpio%d/value", gpioPins[j]);
		dir[j] = fopen(path1, "w");
		value[j] = fopen(path2, "w");
		fseek(dir[j], 0, SEEK_SET);
		fseek(value[j], 0, SEEK_SET);
		fprintf(dir[j], "%s", "out");
		fflush(dir[j]);
		fflush(value[j]);
	}

	///////////////////////////////PWM////////////////////////////////
	sys2 = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(sys2, 0, SEEK_END);

	fprintf(sys2, "am33xx_pwm");
	fflush(sys2);

	//loop
	fprintf(sys2, "bone_pwm_P9_14");
	fflush(sys2);
	fprintf(sys2, "bone_pwm_P8_19");
	fflush(sys2);

	// Sets the pointers to the appropriate duty, period and run files
	PWMAduty = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w");
	PWMAperiod = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w");
	
	fseek(PWMAduty, 0, SEEK_END);
	fseek(PWMAperiod, 0, SEEK_END);
	fprintf(PWMAperiod, "%d", 100000);
	fflush(PWMAperiod);
	fprintf(PWMAduty, "%d", 50000);
	fflush(PWMAduty);

	PWMBduty = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/duty", "w");
	PWMBperiod = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/period", "w");

	fseek(PWMBduty, 0, SEEK_END);
	fseek(PWMBperiod, 0, SEEK_END);
	fprintf(PWMBperiod, "%d", 100000);
	fflush(PWMBperiod);
	fprintf(PWMBduty, "%d", 50000);
	fflush(PWMBduty);
    
	///////////////////////////////sensor///////////////////////////////
	fprintf(sys2, "cape-bone-iio");
	fflush(sys2);
   
   //////////////////////////////LCD//////////////////////////////////
   initialization(); // Initializes the LCD diplay

   //////////////////////////////Game/////////////////////////////////

   //char temp = getchar();
   //putchar(temp);
   //playGame();
	// Since we put sensor on the back of beaglebone and two motors are opposite placed,
	// one of motor rotates forwads and the other rotates backwards in order to make the car go forward

	
	while(1){

	}
	return 0;
	
}

void moveForwards(int t) {
	
	//unsigned char table[100] = "Moving Forward";
	//info(table);

	unsigned char table[100];
   int n = 0;
   while (n < t * 10) {
		if (sensorDistance(path3) < 3000){
			sprintf(table, "%s",   "Moving Forward");
			controlLeft(0, 1);
			controlRight(1, 0);
			usleep(1000);
	   } else {
	   	sprintf(table, "%s",   "Blocked!!");
	   }
	   n++;
	}
	info(table);
}


void moveBackwards(int t) {

	//unsigned char table[100] = "Moving Backward";
	//info(table);

	unsigned char table[100];
	int n = 0;
	while( n < t * 10) {
		if (sensorDistance(path4) < 3000){
			sprintf(table, "%s",   "Moving Backward");
			controlLeft(1, 0);
			controlRight(0, 1);
			usleep(1000);
	   } else {
	   	sprintf(table, "%s",   "Blocked!!");
	   }
	   n++;
	}
	info(table);
}

// Turn left
void turnLeft(int t) {
	controlLeft(0, 1);
	controlRight(0, 1);
	//stop(1);
}


// Turn right
void turnRight(int t) {
	controlLeft(1, 0);
	controlRight(1, 0);
	//stop(1);
}
												 

// stop 
void stop(int t) {
	controlLeft(0, 0);
	controlRight(0, 0);
	usleep(1000);
}


// LEFT wheel
void controlLeft(int IN1, int IN2) {
	fprintf(value[0], "%d", IN1); //AIN1 -0
	fflush(value[0]);
	fprintf(value[1], "%d", IN2); //AIN2 -1
	fflush(value[1]);
}


// RIGHT wheel
void controlRight(int IN1, int IN2) {
	fprintf(value[2], "%d", IN1); //BIN1 -0
	fflush(value[2]);
	fprintf(value[3], "%d", IN2); //BIN2 -1
	fflush(value[3]);
}

int sensorDistance(char * path){
	sensor= fopen(path, "r");
	char buff[255];
	fgets(buff, 255, (FILE*) sensor);
	//printf("Reading from Sensor(sensor): %s...\n",  buff); //Important!!!!!
	int tmp;
	char *ptr;
	tmp = strtol(buff, &ptr, 0);
	return tmp;
}


// Initializes the board
// A blinking cursor will be displayed on the left bottom corner of the lCD display
void initialization() {
	fprintf(value[4], "%d", 0); // Function Set #1 
	fflush(value[4]);				 // RS: 0
	usleep(1000);

	fprintf(value[8], "%d", 0); //E: 0
	fflush(value[8]);
	usleep(1000);

	fcnSet((unsigned char) 0x3f); 
	usleep(1000);
	enableVal();

	displayOff();

	fcnSet((unsigned char) 0x06); //Entry Mode Set
	usleep(500);
	enableVal();

	displayOn();
    
	clearDisplay();
}


void enableVal() {
	fprintf(value[8], "%d", 1); 
	fflush(value[8]);				 // E: 1
	usleep(1000);
	fprintf(value[8], "%d", 0); 
	fflush(value[8]);				 // E: 0
	usleep(1000);
}

// Controls the behavior and position of the cursor on LCD display
void write_com(unsigned char com) {

   fprintf(value[7], "%d", 0); 
	fflush(value[7]);				 // RS: 0
	usleep(1000);

   fprintf(value[8], "%d", 0); 
	fflush(value[8]);				 // E: 0
	usleep(1000);
    
   fcnSet(com);
   usleep(1000);
    
   fprintf(value[8], "%d", 1); 
	fflush(value[8]);				 // E: 1
	usleep(1000);

   fprintf(value[8], "%d", 0); 
	fflush(value[8]);				 // E: 0
	usleep(1000);
}

// Sends in data for diplaying on LCD display.
void write_data(unsigned char data) {
   fprintf(value[7], "%d", 1); 
	fflush(value[7]);				 // RS: 0
	usleep(1000);

   fprintf(value[8], "%d", 0); 
	fflush(value[8]);				 // E: 0
	usleep(1000);

   fcnSet(data);
   usleep(1000);
    
   fprintf(value[8], "%d", 1); 
	fflush(value[8]);				 // E: 1
	usleep(1000);

   fprintf(value[8], "%d", 0); 
	fflush(value[8]);				 // E: 0
	usleep(1000);
}

// display off
void displayOff() {
	write_com((unsigned char) 0x08);
}

// display on
void displayOn() {
	write_com((unsigned char) 0x0F);
}

// clear display
void clearDisplay() {
	write_com((unsigned char) 0x01);
}



void switchRCLK() {
	fprintf(value[5], "%d", 0); 
	fflush(value[5]);				 // RCLK: 0
	usleep(1000);
	fprintf(value[5], "%d", 1); 
	fflush(value[5]);				 // RCLK: 1
	usleep(1000);
	fprintf(value[5], "%d", 0); 
	fflush(value[5]);				 // RCLK:0
	usleep(1000);
}


void switchSRCLK() {
	fprintf(value[6], "%d", 0); 
	fflush(value[6]);				 // SRCLK: 0
	usleep(1000);
	fprintf(value[6], "%d", 1); 
	fflush(value[6]);				 // SRCLK: 1
	usleep(1000);
	fprintf(value[6], "%d", 0); 
	fflush(value[6]);				 // SRCLK:0
	usleep(1000);
}


void fcnSet(unsigned char num) {
	int bitArray[8];
	int i;

	for (i = 0; i < 8; i++) {
		bitArray[i] = num % 2 ;
		num  = num /2;	
	}

	for (i = 0; i < 8; i++) {
		fprintf(value[4], "%d",  bitArray[7-i]); 
		fflush(value[4]);				 
		switchSRCLK();
	}

	switchRCLK();
}

void info(unsigned char str[]) {
	clearDisplay();
	int i;
	if(strlen(str) < 16) {
		for (i = 0; str[i] != '\0'; i++){
			write_data(str[i]);	
		}		
	} else {
		for(i = 0; i < 16;i++) {
			write_data(str[i]);	
		}	
		write_com(0xC0);
		for (i = 16; str[i] != '\0'; i++){
			write_data(str[i]);	
		}
	}	
}


void signal_handler_IO (int status) {
	unsigned char str[100] = "received data from UART1";
    info(str);//Read data from file
    aval = fopen("/dev/ttyO1","r");
    fseek(aval,0,SEEK_SET);
    if(aval == NULL) {
		unsigned char str1[100] = "Error opening file------";
        info(str1);
    }

    char  temp[5];
	temp = fgets(temp, 2, aval);

    printf("Current value " );
    printf("%s\n", temp[0]);

    fclose(aval);

    int signal = atoi(temp[0]);
	
	move(signal);

	if(temp[1] != '\0'){
	    printf("Current value " );
	    printf("%s\n\n", temp[1]);
	
		signal = atoi(temp[1]);
		
		move(signal);
	}

 	/*
	unsigned char str[100] = "received data from UART1";
    info(str);//Read data from file
    aval = fopen("/dev/ttyO1","r");
    fseek(aval,0,SEEK_SET);
    if(aval == NULL) {
		unsigned char str1[100] = "Error opening file------";
        info(str1);
    }

	
    printf("Current value " );
    char temp;
	temp = fgetc(aval);
    printf("%c\n", temp);
	

    fclose(aval);

	move(signal);



    aval = fopen("/dev/ttyO1","r");
    fseek(aval,0,SEEK_SET);
    if(aval == NULL) {
		unsigned char str1[100] = "Error opening file------";
        info(str1);
    }

	
    printf("Current value " );
	temp = fgetc(aval);
    printf("%c\n\n", temp);
	

    fclose(aval);

    signal = atoi(temp);
	
	move(signal);
	*/
}

void move(int signal){
    if(signal == 1 ){
		moveForwards(1);

    }else if(signal == 2){
		moveBackwards(1);   

    }else if(signal == 3){
		turnLeft(1);

    }else if(signal == 4){
		turnRight(1);

    }else if(signal == 0){
        stop(1);
    }	
}