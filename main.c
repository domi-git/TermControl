/**
  ******************************************************************************
  * @file    TermControl.h
  * @author  Dominik Muth          
  * @brief   Terminal Application for controlling the uC Boards      
	* @date    2019-02-16
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>

#include "main.h"
#include "serial.h"

/* Defines -------------------------------------------------------------------*/

#define  LINE          "--------------------------------------------------------------------------------"
#define  CENTER_SPACE  "                                  "
#define  CLS           "\x1b[2J\x1b[0;0H"  /* Clear entire Terminal Screen */

/* Colors */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

/* Font Types */
#define BOLD_START         "\x1b[1m"

/* Font Styles */
#define FONT_GB            "\x1b[32;1m"  /* Set Bold and Green Color */
#define FONT_R             "\x1b[0m"     /* Reset Color and Font */
#define FONT_UL            "\x1b[4m"     /* Underline */

/* Background Styles */
#define BG_GREEN           "\x1b[42m"
#define BG_LBLUE           "\x1b[48;5;30m"

/* Transmission States */
#define  STATE_INIT     0
#define  STATE_HEARTB   1
#define  STATE_APPNAME  2
#define  STATE_CVAL     3 
#define  STATE_END      4

/* Number of Control-Variables to transfer */
#define NUM_CV  1

/* Macros --------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

unsigned char cArr[10000];  /* has to be unsigned for Start of Transmission == 0xFF */

char cAppName[17];

int iTransmitState = 0;

unsigned int uAppNameIdx = 0u;

char cHB = 0;  /* Heartbeat of Termin App on the uC */

unsigned char cCtrlVal[40];
unsigned int uCvi = 0u;               /* Index cCtrlVal */
unsigned short ControlVariable[20];

/* Functions -----------------------------------------------------------------*/

int 
	main    
		(int argc, char *argv[])
{	
	InitSerial();

	while (1)
	{
		usleep(1000000);    // 1s refresh 
		printf("%s", CLS);  // Clear Screen 

		unsigned int i = ReadSerChar(cArr);
//		printf("Uart Bytes: %u\r\n", i);     // Debug
		for (unsigned int k=0; k<i; k++)
		{
//			printf("%u ", cArr[k]);  // Debug
//			printf("%c", cArr[k]);   // Debug

			switch (iTransmitState)
			{

				/* Start of Transmission */
				case STATE_INIT:
					if (cArr[k] == 0xFF)
					{
						iTransmitState = STATE_HEARTB;
					}
					break;

				/* Transfer Heartbeat */
				case STATE_HEARTB:
					cHB = cArr[k];
					iTransmitState = STATE_APPNAME;
					uAppNameIdx = 0u;
					break;

				/* Transfer AppName */
				case STATE_APPNAME:
					cAppName[uAppNameIdx] = cArr[k];
					uAppNameIdx++;
					if (uAppNameIdx == 16u)  
					{ 
						cAppName[16] = '\0';  // Null Termination of String
						iTransmitState = STATE_CVAL; 
						uCvi = 0u;
					}
//						printf("%c", cArr[k]);  // Debug
					break;

				/* Transfer Control-Variables */
				case STATE_CVAL:
					if (uCvi < 2*NUM_CV)
					{
						cCtrlVal[uCvi] = cArr[k];
//						printf("\r\nCVAL Byte %u filled!", uCvi);  // Debug
						uCvi++;
					}
					else
					{
						iTransmitState = STATE_INIT;
					}
					break;
			}
		}

		/* Convert 2-Byte Data into uint16 */
		for (int i=0; i<NUM_CV; i++)
		{
			ControlVariable[i] = (cCtrlVal[2*i] << 8u) | (cCtrlVal[2*i+1]);
		}
		
//		printf("\r\nuAppNameIdx: %u\r\n", uAppNameIdx);  // Debug

		/* Print Menu */
		if (uAppNameIdx == 16u)
		{
			printf("%s  %s                                                           %c  %s\r\n", BG_GREEN, cAppName, cHB, FONT_R);  
		}

		/* Print Control Variables */
		printf("%s    Nr   ControlVar      Value       Min       Max                              %s\r\n", BG_LBLUE, FONT_R);  
		printf(  "     1   EscPwm          %u\r\n", ControlVariable[0]);  
//		printf("%u\r\n", cCtrlVal[0]);  // Debug
//		printf("%u\r\n", cCtrlVal[1]);  // Debug
		
	}
}

/* END ************************************************************************/
