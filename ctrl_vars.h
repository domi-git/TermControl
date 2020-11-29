/**
  ******************************************************************************
  * @file    ctrl_vars.h
  * @author  Dominik Muth          
  * @brief   Definition of the Control Variables for exchange with the microcontroller
	* @date    2019-02-20
  ******************************************************************************
**/
#ifndef __CTRL_VARS_H 
#define __CTRL_VARS_H 

/* Includes ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

#define  NUM_CV  3u      /* Number of Control Variables for the Terminal */

/* Typedefs ------------------------------------------------------------------*/

typedef struct ctrl_var_t {
	unsigned char nr;
  char name[16];
  unsigned short value;  
  unsigned short tcMin;  /* TermControl Minimum */
  unsigned short tcMax;  /* TermControl Maximum */
  unsigned short iMin;   /* Internal Minimum */
  unsigned short iMax;   /* Internal Maximum */
  char unit[8];          /* Unit of the Variable */
} ctrl_var_t;

/* Variables -----------------------------------------------------------------*/

ctrl_var_t sCtrlVar[] = {
		/* State of the System */
    { .nr = 0, .name = "State",      .tcMin = 0, .tcMax = 4,    .iMin = 0, .iMax = 4,    .unit = "1" },
    { .nr = 1, .name = "adcVal",     .tcMin = 0, .tcMax = 4095, .iMin = 0, .iMax = 4095, .unit = "1" },
    { .nr = 2, .name = "BatVolt",    .tcMin = 0, .tcMax = 657,  .iMin = 0, .iMax = 657,  .unit = "0.01V" }
};

/* Functions -----------------------------------------------------------------*/

#endif /* __CTRL_VARS_H */
/* END ************************************************************************/
