// This program is the main project program for lab 4
// The program allows user to control the movement of a tank by using bluetooth module
// to receieve signal sent from user's phone. The bluetooth module is connected to Beaglebone's
// serial port so the signal will be transferred into Beaglebone and Beaglebone will decode the 
// signal then decide the movement of tank. 

// This program also has a bulit-in game to make this project more fun.
// The game will creates a internal 8x8 block game map randomly when the tank first communicate with
// user's phone. Each time the tank move, it will move on the map as well. User starts the game from 
// the bottom left of the map and wins the game only when the tank reach the bottom right corner of 
// the map. The map can be checked when Beaglebone is connected to the station.

// When the tank hit map boundary or hit a "rock", there will be corresponding information display on LCD
// Screen. User is allowed to continue the game when the tank runs on "road" or hits the boundary.
// But when the tank hits a "rock", game will be over. Winning and losing will also have LCD display.
  
#include "driver.h"

void signal_handler_IO (int status);
struct termios termAttr; 
struct sigaction saio;
static int count = 0;

int main(int argc, char *argv[]) {

    // Start up process for the tank
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

	
	fprintf(sys2, "bone_pwm_P9_14");
	fflush(sys2);
	fprintf(sys2, "bone_pwm_P8_19");
	fflush(sys2);

	// Sets the pointers to the appropriate duty, period and run files
    //PWM period controls the speed of the tank
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
   
    /////////////////////////////////LCD////////////////////////////////
    initialization();

    ////////////////////////////////Game////////////////////////////////
    if (count == 0) {
        playGame();
    }

	////////////////////////////////Bluetooth////////////////////////////////
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


	while(1); //Loop and run the program forever
	return 0;	
}


// Moves the tank forward in the current direction for 0.5s.
// The direction variable for the game will also be updated
// Possible displays on LCD
// 1. Current position will be printed out on LCD
// 2. If the tank goes out of the range of internal game map, 
//	  the movement will be rejected and tank will display "Out of board"
// 3. If the tank hit a rock on the game map, LCD displays "!!!Boom!!!"
// 4. If the tank is too close to obstacle in REAL world, LCD displays "Blocked!!"
void moveForwards() {
	if(direction == 0 ){
		carRow--;
	} else if (direction == 1){
		carCol++;
	} else if (direction == 2){
		carRow++;
	} else {
		carCol--;
	}
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
		sleep(1);
		gameOver();
	} else if((carRow == 7) && (carCol == 7)){
		win();
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
   	while (n < 100) {
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
	usleep(500000);
	stop();
}


// Moves backwards in current direction for 0.5 second.
// The direction variable for the game will also be updated
// Possible displays on LCD
// 1. Current position will be printed out on LCD
// 2. If the tank goes out of the range of internal game map, 
//	  the movement will be rejected and tank will display "Out of board"
// 3. If the tank hit a rock on the game map, LCD displays "!!!Boom!!!"
// 4. If the tank is too close to obstacle in REAL world, LCD displays "Blocked!!"
void moveBackwards() {
	if(direction == 0 ){
		carRow++;
	} else if (direction == 1){
		carCol--;
	} else if (direction == 2){
		carRow--;
	} else {
		carCol++;
	}
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
		gameOver();
	} else if((carRow == 7) && (carCol == 7)){
		win();
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
		while( n < 100) {
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
	usleep(500000);
	stop();
}


// Turn left (for 1 second which is about turning 90 degrees)
// The direction variable for the game will also be updated
void turnLeft() {
	if(direction == 0) {
		direction == 3;
	} else{
		direction--;
	}
	controlLeft(0, 1);
	controlRight(0, 1);
	sleep(1);
}


// Turn right (for 1 second which is about turning 90 degrees)
// The direction variable for the game will also be updated
void turnRight() {
	if (direction == 3) {
		direction = 0;
	} else {
		direction++;
	}
	controlLeft(1, 0);
	controlRight(1, 0);
	sleep(1);
}


// Stop 
void stop() {
	controlLeft(0, 0);
	controlRight(0, 0);
	usleep(1000);
}


// Control the movement of LEFT wheel
void controlLeft(int IN1, int IN2) {
	fprintf(value[0], "%d", IN1); //AIN1 -0
	fflush(value[0]);
	fprintf(value[1], "%d", IN2); //AIN2 -1
	fflush(value[1]);
}


// Control the movement of RIGHT wheel
void controlRight(int IN1, int IN2) {
	fprintf(value[2], "%d", IN1); //BIN1 -0
	fflush(value[2]);
	fprintf(value[3], "%d", IN2); //BIN2 -1
	fflush(value[3]);
}


// Opens the sensor at the designated path and returns the diatance it senses.
int sensorDistance(char * path){
	sensor = fopen(path, "r");
	char buff[255];
	fgets(buff, 255, (FILE*) sensor);
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
	enableVal();

	displayOff();

	fcnSet((unsigned char) 0x06); //Entry Mode Set
	enableVal();

	displayOn();  
	clearDisplay();
}

// Controls the enable pin of LCD
void enableVal() {
	fprintf(value[8], "%d", 1); 
	fflush(value[8]);				 // E: 1
	usleep(1000);
	fprintf(value[8], "%d", 0); 
	fflush(value[8]);				 // E: 0
	usleep(1000);
}


// Sends a command to the LCD
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

// Sends a character to display in LCD
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


// LCD display off
void displayOff() {
	write_com((unsigned char) 0x08);
}


// LCD display on and cursor blinks
void displayOn() {
	write_com((unsigned char) 0x0F);
}


// Clears display on LCD
void clearDisplay() {
	write_com((unsigned char) 0x01);
}


// Controls the RCLK of shift register (to send out data from register)
void switchRCLK() {
	fprintf(value[5], "%d", 0); 
	fflush(value[5]);				 // RCLK: 0

	fprintf(value[5], "%d", 1); 
	fflush(value[5]);				 // RCLK: 1

	fprintf(value[5], "%d", 0); 
	fflush(value[5]);				 // RCLK:0
}


// Controls the SRCLK of shift register	(to shift data in shift register)
void switchSRCLK() {
	fprintf(value[6], "%d", 0); 
	fflush(value[6]);				 // SRCLK: 0

	fprintf(value[6], "%d", 1); 
	fflush(value[6]);				 // SRCLK: 1

	fprintf(value[6], "%d", 0); 
	fflush(value[6]);				 // SRCLK:0
}


// Decodes the hexadecimal version of LCD command/ character to be display into binary bits
// arranges the bits and sends to the shift register
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


// Print out a string on LCD
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


// Controls the process from reading a bluetooth signal to deciding the direction of movement
void signal_handler_IO (int status) { 	
	int signal = readFromKeyboard();
	move(signal);	
	printf("\n");
	
}


// Reads a character from the tty port and returns it
int readFromKeyboard() {
    aval = fopen("/dev/ttyO1","r");
    fseek(aval,0,SEEK_SET);
    if(aval == NULL) {
		unsigned char errText[50] = "Error opening file------";
        info(errText);
    }


    char temp[5];
	temp[0] = fgetc(aval);
	
    fclose(aval);
    int signal = atoi(temp);
	return signal;	
}


// Decides which kind of movement the tank will do with the given signal read from tty port
void move(int signal) {
	 if(signal == 1 ){
		moveForwards(1);

    }else if(signal == 2){
		moveBackwards(1);   

    }else if(signal == 3){
		turnLeft(1);

    }else if(signal == 4){
		turnRight(1);

    }else if(signal == 0){
        stop();
    }
}


// Initials the game with map indication: 0 for road, 1 for rock, 2 for the tank.
// Prints out the map once it has been created
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

		
		unsigned char start[] = "Game Start! Avoid the booms!";
		info(start);
		sleep(1);

		//Set the position to be 1 (aka bomb)
		int i;
	   for (i = 0; i < 15; i++) {
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


// Displays the whole game map on the terminal
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


// Controls the display when user loses the game
void gameOver(){
		unsigned char end[] = "Bye";
		info (end);
		sleep(1);
		unsigned char re[] = "Restarting ...";
		info (re);
		sleep(2);
		playGame();
}


// Controls the display when user wins the game
void win(){
	unsigned char end[] = "You Win!";
	info (end);
	sleep(1);
	unsigned char re[] = "Restarting ...";
	info (re);
	sleep(2);
	playGame();
}


// Closes all file pointers
void closeAll(){
	fclose(sys);
	fclose(sys2);
	fclose(PWMAduty);
	fclose(PWMAperiod);
	fclose(PWMBduty);
	fclose(PWMBperiod);
	fclose(sensor);
}
