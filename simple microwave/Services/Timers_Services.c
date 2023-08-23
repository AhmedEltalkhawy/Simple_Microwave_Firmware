
#include "Timers_Services.h"
#include "LCD.h"

static volatile u16 t1,t2,t3,flag=0;


/**************************************** TIMER 0 *************************************************/
static void(*fptr_timer0)(void);
static u32 Timer0_NofOverFlow=0;


static void Timer0_Func(void)
{
	TCNT0 = 6 ;
	static u32 c = 0  ;
	c++ ;
	if (c == Timer0_NofOverFlow)
	{
		c= 0 ;
		fptr_timer0() ;
	}
	
}

void Timer0_SetInterruptTime_ms (u16 time,void(*LocalFptr)(void))
{
	Timer0_Init(TIMER0_NORMAL_MODE,TIMER0_SCALER_8,OCO_DISCONNECTED);
	TCNT0 = 6 ;
	Timer0_NofOverFlow = time *4 ;
	fptr_timer0 = LocalFptr ;
	Timer0_OVF_SetCallBack(Timer0_Func) ;
	Timer0_OV_InterruptEnable() ;
}



/**********************************************PWM*********************************************************/
/*void PWM_Init(void)
{
	Timer1_Init(TIMER1_FASTPWM_ICR_TOP_MODE,TIMER1_SCALER_8,OCRA_NON_INVERTING,OCRB_DISCONNECTED);
	
}*/
void PWM_Freq_KHZ(u16 freq)
{
	if (freq==0);
	else
	{
		u16 Ttotal=1000/freq;
		if (Ttotal>=1)
		{
			ICR1=Ttotal-1;
		}
		else ICR1=0;
		
	}
}
void PWM_Freq_HZ(u16 freq)
{
	if (freq==0);
	else
	{
		u16 Ttotal=(u32)1000000/freq;
		if (Ttotal>=1)
		{
			ICR1=Ttotal-1;
		}
		else ICR1=0;
	}



}
void PWM_Duty(u16 duty)
{
	 if (duty<=100)
	{
		u16 Ton=((u32)duty*(ICR1+1))/100;
		if (Ton>1)
		{
			OCR1A=Ton-1;	
		}
		else
		OCR1A=0;	
	}
}
/******************************************Set Interrupt Time******************************************************/

/*void Timer1_SetInterruptTime_s (u16 time,void(*LocalFptr)(void))
{
	Timer1_Init(TIMER1_CTC_OCRA_TOP_MODE,TIMER1_SCALER_1024,OCRA_DISCONNECTED,OCRB_DISCONNECTED);
	OCR1A=(time*7812)-1;
	Timer1_OCA_SetCallBack(LocalFptr);
	Timer1_OCA_InterruptEnable();
}*/

/*void Timer1_SetInterruptTime_ms (u16 time,void(*LocalFptr)(void))
{
	Timer1_Init(TIMER1_CTC_OCRA_TOP_MODE,TIMER1_SCALER_8,OCRA_DISCONNECTED,OCRB_DISCONNECTED);
	OCR1A=(time*1000)-1;
	Timer1_OCA_SetCallBack(LocalFptr);
	Timer1_OCA_InterruptEnable();	
}*/
void Timer1_SetInterruptTime_us (u16 time,void(*LocalFptr)(void))
{
	Timer1_Init(TIMER1_CTC_OCRA_TOP_MODE,TIMER1_SCALER_8,OCRA_DISCONNECTED,OCRB_DISCONNECTED);
	OCR1A=(time)-1;
	Timer1_OCA_SetCallBack(LocalFptr);
	Timer1_OCA_InterruptEnable();
}

/**********************************************Measure PWM*********************************************************/
void PWM_Measure2(u32* Pfreq,u8* Pduty)
{
	u16 Ton,Toff;
	flag=0;
	TCNT1=0;
	while (DIO_ReadPIN(PIND6));
	while (!DIO_ReadPIN(PIND6));
	TCNT1=0;
	while (DIO_ReadPIN(PIND6));
	Ton=TCNT1;
	TCNT1=0;
	while (!DIO_ReadPIN(PIND6));
	Toff=TCNT1;
	*Pduty=((u32)Ton*100)/(Ton+Toff);
	*Pfreq=(u32)1000000/((u32)Toff+Ton);
}


void PWM_Measure(u32* Pfreq,u8* Pduty)
{
	u16 Ton,Toff;
	
	TCNT1=0;
	Timer1_ICU_SetCallBack(Func_ICU);
	Timer1_InputCaptureEdge(RISING);
	Timer1_ICU_InterruptEnable();
	flag=0;
	while(flag<3);
	Ton=t2-t1;
	Toff=t3-t2;

	*Pduty=((u32)Ton*100)/((u32)Ton+Toff);
	*Pfreq=(u32)1000000/((u32)Toff+Ton);

}

void f1(void)
{
	if (flag==0)
	{
	TCNT1=0;
	Timer1_ICU_SetCallBack(Func_ICU);
	Timer1_InputCaptureEdge(RISING);
	Timer1_ICU_InterruptEnable();

	}
}

u8 f2(u32* Pfreq,u8* Pduty)
{
	u16 Ton,Toff;
	
	if (flag==3)
	{
	Ton=t2-t1;
	Toff=t3-t2;

	*Pduty=((u32)Ton*100)/((u32)Ton+Toff);
	*Pfreq=(u32)1000000/((u32)Toff+Ton);
	flag=0;
	return 1;
	
	}
	return 0;
}



static void Func_ICU(void)
{
	if (flag==0)
	{
		t1=ICR1;
		Timer1_InputCaptureEdge(FALLING);
		flag=1;
	}
	else if (flag==1)
	{
		t2=ICR1;
		Timer1_InputCaptureEdge(RISING);
		flag=2;		
	}
	else if (flag==2)
	{
		t3=ICR1;
		Timer1_ICU_InterruptDisable();
		flag=3;
	}	
	
	
	
}
static void Timer_func(void);
static u32 NofOverFlow=0;
static void(*fptr_timer)(void);
void Timer1_SetInterruptTime_s (u16 time,void(*LocalFptr)(void))
{
	Timer1_Init(TIMER1_CTC_OCRA_TOP_MODE,TIMER1_SCALER_8,OCRA_DISCONNECTED,OCRB_DISCONNECTED);
	//Timer1_Init(TIMER1_CTC_OCRA_TOP_MODE,TIMER1_SCALER_8 ); // error here 
	OCR1A=999;
	NofOverFlow=time*1000;
	fptr_timer=LocalFptr;
	Timer1_OCA_SetCallBack(Timer_func);
	Timer1_OCA_InterruptEnable();
}

static void Timer_func(void)
{
	static u32 c=0;
	c++;
	if (c==NofOverFlow)
	{
		c=0;
		fptr_timer();
	}
}