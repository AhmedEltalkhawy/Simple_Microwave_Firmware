/*
 * KeyPad.c
 *
 * Created: 3/9/2021 10:57:32 PM
 *  Author: EL-talkhawy
 */ 
#include "KeyPad.h"

u8 keys [ROW][COL] = {{'7' , '8' , '9' , '/'} ,
                      {'4' , '5' , '6' , '*'} ,
					  {'1' , '2' , '3' , '-'} ,
					  {'c' , '0' , '=' , '+'} ,} ;
						  
/*u8 keys [ROW][COL] = {{'7' , '8' , '9' , 'A'} ,
                      {'4' , '5' , '6' , 'B'} ,
                      {'1' , '2' , '3' , 'C'} ,
                      {'c' , '0' , '=' , 'D'} ,} ;*/

						
						  
u8 KEYPAD_GetEntry (void) 
{
	u8 r , c , key = NO_KEY  ;
	//first i put to all output pins 1 
	DIO_WritePIN(FIRST_OUT     , HIGH) ;
	DIO_WritePIN(FIRST_OUT + 1 , HIGH) ;
	DIO_WritePIN(FIRST_OUT + 2 , HIGH) ;
	DIO_WritePIN(FIRST_OUT + 3 , HIGH) ;
	
	for(r=0 ; r < ROW ; ++r)
	{
		DIO_WritePIN(FIRST_OUT+r , LOW) ;
		for(c=0 ; c < COL ; ++c)
		{
			if(! DIO_ReadPIN(FIRST_IN+c))
			{
				key = keys[r][c] ; 
				while(!DIO_ReadPIN(FIRST_IN+c)) ;
			}
			
		}
		DIO_WritePIN(FIRST_OUT+r , HIGH) ;

	}
	return key ;

	
}
