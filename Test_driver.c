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


	/*
	while(1){
	moveForwards(10);
	stop(1);
	moveForwards(10);
	stop(1);
	moveBackwards(10);
	stop(1);
	turnRight(10);
	stop(10);
	moveForwards(10);
	stop(10);
	moveForwards(10);
	stop(10);
	turnRight(10);
	stop(10);
	moveForwards(10);
	stop(1);
	moveBackwards(10);
	stop(1);
	turnRight(10);
	stop(10);
	}	
	*/
	
	
	
	while(1){

	}
	return 0;
	
}

// Move fowards for 0.t second
/*
void moveForwards(int t) {
	if(direction == 0 ){
		carRow--;
	} else if (direction == 1){
		carCol++;
	} else if (direction == 2){
		carRow++;
	} else {
		carCol--;
	}
	//printf("Forward: %i\n", direction);
	unsigned char table[100];
	if((carRow > 7) || (carRow < 0) || (carCol > 7) || (carCol < 0)){  //stay
		sprintf(table, "%s",   "Out of board");
		if(direction == 0 ){
			carRow++;
		} else if (direction == 1){
			carCol--;
		} else if (direction == 2){
			carRow--;
		} else {
			carCol++;
		}
	} else if(map[carRow][carCol] == 1) {
		sprintf(table, "%s",   "!!!Boom!!!");
		info(table); //didnt work
		gameOver();
	} else {  //move
		if(direction == 0 ){
			map[carRow+1][carCol] = 0;
		} else if (direction == 1){
			map[carRow][carCol-1] = 0;
		} else if (direction == 2){
			map[carRow-1][carCol] = 0;
		} else {
			map[carRow][carCol+1] = 0;
		}
		int n = 0;
   	while (n < t * 10) {
			if (sensorDistance(path3) < 3000){
				sprintf(table, "%s","Position: ");
				char row[15];
				sprintf(row, "%d", carRow);
				strcat(table, row);
				strcat(table, ", ");
				char col[15];
				sprintf(col, "%d", carCol);
				strcat(table, col);
				controlLeft(0, 1);
				controlRight(1, 0);
				usleep(10000);
	  		} else {
	   		sprintf(table, "%s",   "Blocked!!");
	   	}
	  		n++;
		}
		map[carRow][carCol] = 2;
	}
	info(table);
	showMap();
}
*/
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
	stop(1);
}



// Move backwards for 0.t second
/*
void moveBackwards(int t) {
	if(direction == 0 ){
		carRow++;
	} else if (direction == 1){
		carCol--;
	} else if (direction == 2){
		carRow--;
	} else {
		carCol++;
	}
	//printf("Backward: %i\n", direction);
	unsigned char table[100];
	if((carRow > 7) || (carRow < 0) || (carCol > 7) || (carCol < 0)){
		sprintf(table, "%s",   "Out of board");
		if(direction == 0 ){
			carRow--;
		} else if (direction == 1){
			carCol++;
		} else if (direction == 2){
			carRow++;
		} else {
			carCol--;
		}
	} else if(map[carRow][carCol] == 1) {
		sprintf(table, "%s",   "!!!Boom!!!");
		info(table);
		//sleep(2);
		gameOver();
	} else {
		if(direction == 0 ){
			map[carRow-1][carCol] = 0;
		} else if (direction == 1){
			map[carRow][carCol+1] = 0;
		} else if (direction == 2){
			map[carRow+1][carCol] = 0;
		} else {
			map[carRow][carCol-1] = 0;
		}
		int n = 0;
		while( n < t * 10) {
			if (sensorDistance(path4) < 3000){
				sprintf(table, "%s","Position: ");
				char row[15];
				sprintf(row, "%d", carRow);
				strcat(table, row);
				strcat(table, ", ");
				char col[15];
				sprintf(col, "%d", carCol);
				strcat(table, col);
				controlLeft(1, 0);
				controlRight(0, 1);
				usleep(10000);
		   } else {
		   	sprintf(table, "%s",   "Blocked!!");
		   }
		   n++;
		}
		map[carRow][carCol] = 2;
	}
	info(table);
	showMap();
}
*/
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
	stop(1);
}

// Turn left
void turnLeft(int t) {
	direction--;
	controlLeft(0, 1);
	controlRight(0, 1);
	usleep(100000);

}


// Turn right
void turnRight(int t) {
	direction++;
	controlLeft(1, 0);
	controlRight(1, 0);
	usleep(100000);

}


// stop 
void stop(int t) {
	controlLeft(0, 0);
	controlRight(0, 0);
	sleep(1);
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
	usleep(1000);	
}

/*
//map: 0 for road, 1 for block, 2 for the car.
void playGame() {
	//map: Matrix (8 * 8) Start: (7, 0)
	carRow = 7;
	carCol = 0;
	direction = 0;
	int row;
	for (row = 0; row < 8; row++) {
		int col;
		for (col = 0; col <8; col++) {
			map[row][col] = 0;
		}
	 }
	map[carRow][carCol] = 2;
	unsigned char selectLevel[] = "Please select level(0 - 5).";
	info(selectLevel);
	sleep(1);
	char temp = getchar();
	int level = temp - '0';


	unsigned char start[] = "Game Start! Avoid red spot!";
	info(start);
	sleep(2);

	//Set the position to be 1 (aka bomb)
	int i;
   for (i = 0; i < 4 * level; i++) {
   	//start from bottom left corner
   	int randRow = 7;
   	int randCol = 0;
   	while (((randRow > 5) && (randCol < 2 )) || ((randRow > 5 && randCol > 5))) {
   		randRow = rand() % 8;
   		randCol = rand() % 8;
   	}
  		map[randRow][randCol] = 1;
  	}
  	showMap();
}


void showMap(){
	printf("\n");
	int h;
  	int j;
  	for ( h = 0; h < 8; h++ ) {
  		printf("\n");
      for ( j = 0; j < 8; j++ ) {
         printf("%d ",map[h][j] );
      }
   }
   
}


void gameOver(){
	//some LED patern : undetermined
	unsigned char play[] = "Play again? (y/n)";
	info (play);
	char restart = getchar(); //jumped???
	while(restart == '\n') {
		restart = getchar();
	}
	if (restart == 'y'){
		//playGame();
	} else {
		unsigned char end[] = "Bye";
		info (end);
			//*sys, *sys2, *PWMAduty, *PWMAperiod, *PWMBduty, *PWMBperiod, *sensor;
		fclose(sys);
		fclose(sys2);
		fclose(PWMAduty);
		fclose(PWMAperiod);
		fclose(PWMBduty);
		fclose(PWMBperiod);
		fclose(sensor);
		exit(1);
	}
	
}
*/ 

void signal_handler_IO (int status) {
	unsigned char str[100] = "received data from UART1";
    info(str);//Read data from file
    aval = fopen("/dev/ttyO1","r");
    fseek(aval,0,SEEK_SET);
    if(aval == NULL) {
		unsigned char str1[100] = "Error opening file------";
        info(str1);
    }

	
    printf("Current value " );
    char  temp[50];
    if( fgets(temp, 2, aval) >0 ) {
       printf("%s\n", temp );
    }
	

    fclose(aval);

    int signal = atoi(temp);
 
    //printf("Frontvalue %d\n", signal);
    //int pid =system("pgrep -f testtimer");
	
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