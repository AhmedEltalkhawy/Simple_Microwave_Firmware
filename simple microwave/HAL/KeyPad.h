/*
 * KeyPad.h
 *
 * Created: 3/9/2021 10:57:05 PM
 *  Author: EL-talkhawy
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "DIO_Interface.h"


/******************* PIN CONFIG ********************************************/ 

#define  FIRST_IN   PINA4  // must define in DIO_CONFIG as input pull up
#define  FIRST_OUT  PINA0  // must define in DIO_CONFIG as output
// here we config the 4 row and 4 one by one so i need only to pass first ... 

#define  COL 4 
#define  ROW 4
#define  NO_KEY 'N'

/****************************************************************************/

u8 KEYPAD_GetEntry (void) ;





#endif /* KEYPAD_H_ */