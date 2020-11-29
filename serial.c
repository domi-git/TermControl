/******************************************************************************/
/* @file    serial.c                                                          */
/* @author  Dominik Muth                                                      */
/* @brief   Serial-Module                                                     */
/******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* Variables -----------------------------------------------------------------*/
static int fd;  /* File-deskriptor */

/* Functions -----------------------------------------------------------------*/
void
	InitSerial
		(void)
{
	const char *device = "/dev/ttyUSB0";
	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	struct termios config;

	if (fd == -1) {
 		printf( "failed to open port\n" );
	}

	if (!isatty(fd)) {
 		printf( "File deskriptor is not a tty-device\n" );
	} 
	
	if(tcgetattr(fd, &config) < 0) { 
 		printf( "No Configuration File available\n" );
	}

	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	config.c_oflag = 0;
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	
	config.c_cflag &= ~(CSIZE | CSTOPB);
	config.c_cflag |= (CS8 | PARODD | CLOCAL | PARENB);
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 0;
	if (cfsetispeed(&config, B9600) < 0 || cfsetospeed(&config, B9600) < 0) {
 		printf( "Problem configuring the Baudrate\n" );
	}
	if (tcsetattr(fd, TCSAFLUSH, &config) < 0) {
 		printf( "Problem applying the serial configuration\n" );
	}
}

/*----------------------------------------------------------------------------*/
unsigned int
  ReadSerChar
		(unsigned char* cArr)
{ 
	unsigned char c;
	unsigned int i=0;
	while (read(fd,&c,1)>0) {
		cArr[i] = c;
		i++;
	}
	return i;
}

/*----------------------------------------------------------------------------*/
void 
  WriteSerChar
		(unsigned char c)
{ 
	write(fd,&c,1);
}
/* END -----------------------------------------------------------------------*/
