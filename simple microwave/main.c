/*
 * simple microwave.c
 *
 * Created: 8/21/2023 8:34:46 PM
 * Author : ELtalkhawy
 */ 

#include "DIO_Interface.h"
#include "EX_Interrupt.h"
#include "Timers.h"
#include "LCD.h"
#include "KeyPad.h"
#include "util/delay.h"

static void(*fptr_timer1)(void);

static void Timer1_AppFunc () ;
void TimingFinished (void) ;
void Timer1_SetInterruptTime_ms (u16 time,void(*LocalFptr)(void)) ;
void StartButton (void) ;
void CheckSensorStatus (void) ;
void StopButton (void) ;




/* by default microwave does not have food */
volatile u8 weight_sensor = 1 ;

/* by default microwave door is closed */

volatile u8 door_sensor = 1;

volatile u16 timing_value = 0 ;
volatile u8  pause_state = 0  , heating_state = 0;

volatile u8 i = 0 , app_flag = 1 ;




int main(void)
{
	
	sei() ;
	DIO_Init() ;
	LCD_Init() ;
	
	EXI_Enable(EX_INT0) ;
	EXI_TriggerEdge(EX_INT0 ,FALLING_EDGE) ;
	EXI_SetCallBack(EX_INT0 , StartButton) ;
	
	EXI_Enable(EX_INT1) ;
	EXI_TriggerEdge(EX_INT1 ,FALLING_EDGE) ;
	EXI_SetCallBack(EX_INT1 , StopButton) ;
	
	/* this function is used to check the status of door & weight sensor */
	Timer0_SetInterruptTime_ms(100 ,CheckSensorStatus) ;
	LCD_GoTo(0,0) ;
	LCD_WriteString("Hello :) ") ;
	
	
	
	
    while (1) 
    {
		if( (heating_state == 0 )  && (pause_state == 0)  )
		{
			
			if (door_sensor == 1)
			{
				/*LCD_GoTo(0,0) ;
				LCD_WriteString("D closed  ") ;*/
				DIO_WritePIN(PINB0 , HIGH) ;
			}
			else
			{
				/*LCD_GoTo(0,0) ;
				LCD_WriteString("D opened ") ;*/
				DIO_WritePIN(PINB0 , LOW) ;
			}
			if (weight_sensor == 1)
			{
				/*LCD_GoTo(0,9) ;
				LCD_WriteString("YESfood     ") ;*/
				DIO_WritePIN(PINB1 , HIGH) ;
			}
			else
			{
				/*LCD_GoTo(0,9) ;
				LCD_WriteString("NOfood") ;*/
				DIO_WritePIN(PINB1 , LOW) ;
			}
			cli() ;
			LCD_GoTo(1,0) ;
			LCD_WriteNumber(timing_value) ;
			sei() ;
			u8 ch = KEYPAD_GetEntry() ;
			if (ch != NO_KEY)
			{
				
				if (ch>= '0' && ch <= '9')
				{
					cli() ;
					LCD_GoTo(1,i++) ;
					LCD_WriteChar(ch);
					sei() ;
					ch -= '0' ;
					timing_value = timing_value *10 + ch ;
				}
				else
				{
					i = 0 ;
				}
				
			}
		}
		
			
    }
}

void StopButton (void)
{
	if( (pause_state == 0) && (heating_state == 1))
	{
		TimingFinished() ;
		pause_state = 1 ;
		LCD_GoTo(0,0) ;
		LCD_WriteString("                                  ") ;
		LCD_GoTo(0,0) ;
		LCD_WriteString("Heating Paused") ;
	}
	else if ( (pause_state == 1 )  )
	{
		heating_state = 0 ;
		pause_state   = 0 ;
		timing_value  = 0 ;
		i = 0 ;
		//LCD_ClearDisplay() ;
		LCD_GoTo(0,0) ; 
		LCD_WriteString("                                   ") ;
		LCD_GoTo(0,0) ;
		LCD_WriteString("Heating canceled ") ;
		LCD_GoTo(1,0) ;		
		LCD_WriteString("                                      ") ;
	}
	else
	{
		LCD_GoTo(1,0) ;
		LCD_WriteString("                                 ") ;
		timing_value = 0 ;
		i = 0 ;
	}

}

 
void StartButton (void)
{
	if( (timing_value != 0) && (door_sensor == 1 ) && (weight_sensor==1) )
	{
		heating_state = 1 ;
		pause_state = 0 ;
		DIO_WritePIN(PINB2 , HIGH) ;
		DIO_WritePIN(PINB3 , HIGH) ;
		LCD_ClearDisplay() ;
		LCD_GoTo(0,0) ;
		LCD_WriteString("M is Heating") ;
		Timer1_SetInterruptTime_ms(1000,  TimingFinished) ;
	}
	else
	{
		heating_state = 0 ;
		LCD_ClearDisplay() ;
		/*LCD_GoTo(0,0) ;
		LCD_WriteString("M Not Heating") ;*/
		LCD_GoTo(0,0) ;
		LCD_WriteString("Check requests") ;
	}
}





void CheckSensorStatus (void)
{
	if(DIO_ReadPIN(PIND6)== 0)
	{
		door_sensor ^= 1 ;
		while(DIO_ReadPIN(PIND6)== 0) ;

	}
	if(DIO_ReadPIN(PIND7)== 0)
	{
		weight_sensor ^= 1 ;
		while(DIO_ReadPIN(PIND7)== 0) ;

	}
}

void Timer1_SetInterruptTime_ms (u16 time,void(*LocalFptr)(void))
{
	Timer1_Init(TIMER1_CTC_OCRA_TOP_MODE,TIMER1_SCALER_1024,OCRA_DISCONNECTED,OCRB_DISCONNECTED);
	OCR1A=(time*7)-1;
	fptr_timer1 = LocalFptr ;
	Timer1_OCA_SetCallBack(Timer1_AppFunc);
	Timer1_OCA_InterruptEnable();
}
static void Timer1_AppFunc ()
{
	DIO_TogglePIN(PINB0) ;
	DIO_TogglePIN(PINB1) ;
	--timing_value ; 
	LCD_GoTo(1,0) ;
	LCD_WriteString("remainingTime ");
	LCD_WriteNumber(timing_value) ;
	
	if (timing_value == 0)
	{
		fptr_timer1() ;
	}
	
}
void TimingFinished (void)
{
	heating_state = 0 ;
	DIO_WritePIN(PINB0 , LOW) ;
	DIO_WritePIN(PINB1 , LOW) ;
	DIO_WritePIN(PINB2 , LOW) ;
	DIO_WritePIN(PINB3 , LOW) ;
	Timer1_OCA_InterruptDisable();	

}