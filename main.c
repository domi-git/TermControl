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
#include <curses.h>
#include <stdlib.h> // for atexit()

#include "main.h"
#include "serial.h"
#include "ctrl_vars.h"

/* Defines -------------------------------------------------------------------*/

/*** Transmission Control Defines ***/

/* Transmission States */
#define  STATE_INIT     0
#define  STATE_HEARTB   1
#define  STATE_APPNAME  2
#define  STATE_CVAL     3 
#define  STATE_END      4

/* Defines to filter High- or Low-Byte */
#define  U16_HIGH_BYTE  0x0000FF00
#define  U16_LOW_BYTE   0x000000FF

/* Macros --------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

unsigned char cArr[10000];  /* has to be unsigned for Start of Transmission == 0xFF */

char cAppName[17] = "                \0";

int iTransmitState = 0;

unsigned int uAppNameIdx = 0u;

char cHB = 0;  /* Heartbeat of Terminal App on the uC */

/* Bytes for sCtrVar-values */
unsigned char cCtrlVal[40];
unsigned int uCvi = 0u;         /* Index cCtrlVal */

int pressed_key = 0;

unsigned int bKeepRunning = 1u;

char cNewVal[500];  /* Input for new Value */

/* Prototypes ----------------------------------------------------------------*/

void showFalseInputResponse(void);

/* Functions -----------------------------------------------------------------*/

/* Quit curses environment on program exit */
void quit()
{
  endwin();
}

/* Main Program function */
int 
	main    
		(int argc, char *argv[])
{	
	/* Init Curses Library */
  initscr();
  atexit(quit);
  start_color();
	curs_set(0);   /* Turn Cursor-visiblility off */ 
	noecho();
	cbreak();	    /* Line buffering disabled. pass on everything */
	timeout(0);   /* Read keyboard inputs without blocking */

	/* Definition of color pairs */
  init_pair(1, COLOR_WHITE, COLOR_BLACK);   
  init_pair(2, COLOR_BLACK, COLOR_GREEN);   
  init_pair(3, COLOR_BLACK, COLOR_CYAN);   
	bkgd(COLOR_PAIR(1));

	/* Init Serial Connection */
	InitSerial();
	usleep(400000);  /* Wait to see Errors */

	while (bKeepRunning)
	{
		/*** Sleep, to keep CPU-Load low ***/
		usleep(50000);

		/*** Always Clear Screen in every Cycle ***/
		erase();        /* Fill entire Screen with Blanks */

		/*** Read Serial Data ***/
		unsigned int i = ReadSerChar(cArr);
		for (unsigned int k=0; k<i; k++)
		{
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
					break;

				/* Transfer Control-Variables */
				case STATE_CVAL:
					cCtrlVal[uCvi] = cArr[k];
					uCvi++;
					if (uCvi == 2*NUM_CV)
					{
						iTransmitState = STATE_INIT;
					}
					break;
			}
		}

		/* Refresh screen after every complete receive */
		if (iTransmitState == STATE_INIT)
		{
			/* Convert 2-Byte Data into uint16 */
			for (int i=0; i<NUM_CV; i++)
			{
				sCtrlVar[i].value = (cCtrlVal[2*i] << 8u) | (cCtrlVal[2*i+1]);
			}
			
			color_set(2, 0);
			mvprintw(0, 0, "  TermControl                                                                   ");  
			color_set(1, 0);
			/* Print Menu */
			if (uAppNameIdx == 16u)
			{
			  color_set(2, 0);
				mvprintw(0, 34, "%s", cAppName);  
				mvprintw(0, 76, "%c", cHB);  
			  color_set(1, 0);
			}

			/* Print Control Variables */
			color_set(3, 0);
			mvprintw(1, 0, "    Nr   ControlVar       Value       Min       Max     Unit                    ");  
			color_set(1, 0);
			for (int i=0; i<(uCvi/2); i++)
			{
				mvprintw(i+2, 4, "%d", i);                   /* Number */  
				mvprintw(i+2, 9, "%s", sCtrlVar[i].name);    /* Name */  
				mvprintw(i+2, 26, "%u", sCtrlVar[i].value);  /* Value */  
				mvprintw(i+2, 38, "%u", sCtrlVar[i].tcMin);    /* min */  
				mvprintw(i+2, 48, "%u", sCtrlVar[i].tcMax);    /* max */  
				mvprintw(i+2, 56, "%s", sCtrlVar[i].unit);   /* Unit */  
			}

			/* Check User Inputs */
			pressed_key = getch();
			if (pressed_key >= '0' && pressed_key <= '9')
			{
				int pressed_key_int = pressed_key - 48;
				if (pressed_key_int >= 0 && pressed_key_int < uCvi-1)
				{
					int iNewVal = -1;
					timeout(-1);    /* Read keyboard inputs without blocking */
					mvprintw(20, 0, "New Value for %s: ", sCtrlVar[pressed_key_int].name);
					echo();
					nocbreak();	    /* Line buffering enabled */
					getstr(cNewVal);
					if (cNewVal[0] == '0')
					{
						iNewVal = 0;
					}
					else
					{
						iNewVal = atoi(cNewVal);
						if (iNewVal == 0)  { iNewVal = -1; }  /* Input is not a Number */
					}
					if (iNewVal < 0 || iNewVal > 65535)
					{
						showFalseInputResponse();
					}	
					else
					{
						cbreak();
						noecho();
						mvprintw(21, 0, "New Value to send: %d", iNewVal);
						WriteSerChar((unsigned char) 0xFF);
						WriteSerChar((unsigned char) sCtrlVar[pressed_key_int].nr);
						WriteSerChar((unsigned char) (((unsigned int)iNewVal) >> 8u));
						WriteSerChar((unsigned char) (((unsigned int)iNewVal) & U16_LOW_BYTE));
						getch();
					}
					timeout(0);
					cbreak();
					noecho();
					erase();        /* Fill entire Screen with Blanks */
				}
			}
			else
			{
				switch (pressed_key)
				{
					case 'q':
						bKeepRunning = 0u;  /* End Application */
						break;
				}
			}

			/* Refresh screen */
	    refresh();
		}
	}
}

/******************************************************************************/
void
	showFalseInputResponse
		(void)
{
	cbreak();
	noecho();
	erase();        /* Fill entire Screen with Blanks */
	move(0, 0);
 	mvprintw(7, 25, " _    _ _            _____          ");
	mvprintw(8, 25, "| \\ | (_)          |_   _|         ");
	mvprintw(9, 25, "|  \\| |_  ___ ___    | |_ __ _   _ ");
	mvprintw(10, 25, "| . ` | |/ __/ _ \\   | | '__| | | |");
	mvprintw(11, 25, "| |\\  | | (_|  __/   | | |  | |_| |");
	mvprintw(12, 25, "\\_| \\_/_|\\___\\___|   \\_/_|   \\__, |");
	mvprintw(13, 25, "                              __/ |");
	mvprintw(14, 25, "                             |___/ ");
	refresh();
	getch();
}
/* END ************************************************************************/
