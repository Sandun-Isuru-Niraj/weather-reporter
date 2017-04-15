/******************************************************************************

A basic demo program showing sms functions.

                                     NOTICE
									--------
NO PART OF THIS WORK CAN BE COPIED, DISTRIBUTED OR PUBLISHED WITHOUT A
WRITTEN PERMISSION FROM EXTREME ELECTRONICS INDIA. THE LIBRARY, NOR ANY PART
OF IT CAN BE USED IN COMMERCIAL APPLICATIONS. IT IS INTENDED TO BE USED FOR
HOBBY, LEARNING AND EDUCATIONAL PURPOSE ONLY. IF YOU WANT TO USE THEM IN 
COMMERCIAL APPLICATION PLEASE WRITE TO THE AUTHOR.


WRITTEN BY:
AVINASH GUPTA
me@avinashgupta.com

*******************************************************************************/
 #define F_CPU 1000000
  #define BAUD 9600                                   // define baud
 #define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "lib/lcd/lcd.h"
#include "lib/sim300/sim300.h"
#include "lib/dht.h"




 #include<string.h>
 void Setup(void);
 void Loop(void);
 void serialSetup(void);
 void serialSend(char* sendString);




void Halt();

 //Global variables.
 char ar[]= "hello";
 
 // void usartinit();
 uint16_t ReadADC(uint8_t ch);
 void InitADC();
 
 void TempControl(int tem);
 void TempControl1();
 
 unsigned char rxvalue(void);
 unsigned char rxdata,a,cmd,b;
 unsigned int z;
 unsigned char message[15];
 
 
 
 
 

 
 
 void InitADC();
 uint16_t ReadADC(uint8_t ch);
 uint16_t adc_result0;
 void TempControl(int tem);
 void TempControl1();
 int8_t humidity = 0;
 int8_t temperature = 0;
 char Humstr[10];
 char Tempstr[10];
 
 
 
 
 void uart_init (void)
 {
	 UBRRH = (BAUDRATE>>8);                      // shift the register right by 8 bits
	 UBRRL = BAUDRATE;                           // set baud rate
	 UCSRB|= (1<<TXEN)|(1<<RXEN);                // enable receiver and transmitter
	 UCSRC|= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);   // 8bit data format
 }
 void uart_transmit_char (unsigned char data)
 {
	 while (!( UCSRA & (1<<UDRE)));                // wait while register is free
	 UDR = data;                                   // load data in the register
 }
 void uart_transmit_string (const unsigned char* st)
 {
	 
	 
	 while(*st)
	 uart_transmit_char(*st++);
	 // load data in the register
                                 // load data in the register
}


	
int main(void)
{
	
	
	//Initialize LCD Module
	LCDInit(LS_NONE);
	
	
	
	DDRA &=~(1<<PINA0);//initialize port A0 as input
	
	//PORTA |= (1<<PINA0);//remove if rain sensor does not work well
	
	InitADC();
	
	LCDClear();
	LCDWriteString("Team 4explorers");
	_delay_ms(3000);
	LCDClear();
	LCDWriteString("Loading....");
	_delay_ms(2000);
	
	
	LCDClear();
	
	_delay_ms(3000);
	/*	
	//Intro Message
	LCDWriteString("MATRIX MEDICS ");
	LCDWriteStringXY(0,1,"By GROUP MATRIX");
	
	_delay_ms(1000);
	
	LCDClear();
	
	*/
	
	
	
	//Initialize SIM300 module
	LCDWriteString("GSM Initializing ...");
	int8_t r= SIM300Init();
	
	_delay_ms(1000);
	
	/*
	//Check the status of initialization
	switch(r)
	{
		case SIM300_OK:
			LCDWriteStringXY(0,1,"OK !");
			break; 
		case SIM300_TIMEOUT:
			LCDWriteStringXY(0,1,"No response");
			Halt();
		case SIM300_INVALID_RESPONSE:
			LCDWriteStringXY(0,1,"Inv response");
			Halt();
		case SIM300_FAIL:
			LCDWriteStringXY(0,1,"Fail");
			Halt();
		default:
			LCDWriteStringXY(0,1,"Unknown Error");
			Halt();	
	}
	
	_delay_ms(1000);
	
	//IMEI No display
	LCDClear();
	
	char imei[16];
	
	r=SIM300GetIMEI(imei);
	
	if(r==SIM300_TIMEOUT)
	{
		LCDWriteString("Comm Error !");
		Halt();
	}
	
	LCDWriteString("Device IMEI:");
	LCDWriteStringXY(0,1,imei);
	
	_delay_ms(1000);
	
	//Manufacturer ID
	LCDClear();
	
	char man_id[48];
	
	r=SIM300GetManufacturer(man_id);
	
	if(r==SIM300_TIMEOUT)
	{
		LCDWriteString("Comm Error !");
		Halt();
	}
	
	LCDWriteString("Manufacturer:");
	LCDWriteStringXY(0,1,man_id);
	
	_delay_ms(1000);
	
	//Manufacturer ID
	LCDClear();
	
	char model[48];
	
	r=SIM300GetModel(model);
	
	if(r==SIM300_TIMEOUT)
	{
		LCDWriteString("Comm Error !");
		Halt();
	}
	
	LCDWriteString("Model:");
	LCDWriteStringXY(0,1,model);
	
	_delay_ms(1000);
	

	
	//Check Sim Card Presence
	LCDClear();
	LCDWriteString("Checking SIMCard");
	
	_delay_ms(1000);
	
	r=SIM300IsSIMInserted();
	
	if (r==SIM300_SIM_NOT_PRESENT)
	{
		//Sim card is NOT present
		LCDWriteStringXY(0,1,"No SIM Card !");
		
		Halt();
	}
	else if(r==SIM300_TIMEOUT)
	{
		//Communication Error
		LCDWriteStringXY(0,1,"Comm Error !");
		
		Halt();
	}
	else if(r==SIM300_SIM_PRESENT)
	{
		//Sim card present
		LCDWriteStringXY(0,1,"SIM Card Present");
			
		_delay_ms(1000);
	}
	
	//Network search
	LCDClear();
	LCDWriteStringXY(0,0,"SearchingNetwork");
	
	uint8_t		nw_found=0;
	uint16_t	tries=0;
	uint8_t		x=0;
	
	while(!nw_found)
	{
		r=SIM300GetNetStat();
		
		if(r==SIM300_NW_SEARCHING)
		{
			LCDWriteStringXY(0,1,"%0%0%0%0%0%0%0%0%0%0%0%0%0%0%0%0");
			LCDWriteStringXY(x,1,"%1");
			LCDGotoXY(17,1);
			
			x++;
			
			if(x==16) x=0;
			
			_delay_ms(50);
			
			tries++;
			
			if(tries==600)	
				break;
		}
		else
			break;	
		 
	}
	LCDClear();
	
	if(r==SIM300_NW_REGISTERED_HOME)
	{
		LCDWriteString("Network Found");
	}
	else
	{
		LCDWriteString("Cant Connt to NW!");
		Halt();
	}
	
	_delay_ms(1000);
	
	LCDClear();
	
	//Show Provider Name
	char pname[32];
	r=SIM300GetProviderName(pname);
	
	if(r==0)
	{
		LCDWriteString("Comm Error !");
		Halt();
	}
	
	LCDWriteString(pname);
		
	_delay_ms(1000);
	*/
	uint8_t		x=0;
	LCDClear();
	LCDWriteString("Waiting");
	
	//Wait for MSG
	uint8_t id;
	
	UFlushBuffer();
	
	while(1)
	{
		
		LCDClear();
	
		LCDWriteStringXY(0,0,"Waiting for msg");
	
		x=0;
		int8_t vx=1;
		
		while(SIM300WaitForMsg(&id)!=SIM300_OK)
		{
			//LCDWriteStringXY(0,1,"%0%0%0%0%0%0%0%0%0%0%0%0%0%0%0%0");
			//LCDWriteStringXY(x,1,"%1");
			//LCDGotoXY(17,1);
			
			//x+=vx;
			
			//if(x==15 || x==0) vx=vx*-1;
			
			TempControl(30);
		}
	
		LCDWriteStringXY(0,1,"MSG Received    ");
		
		_delay_ms(1000);
		
		//Now read and display msg
		LCDClear();
		char msg[300];
		
		r=SIM300ReadMsg(id,msg);
		
	
		if(r==SIM300_OK)
		{
			
			LCDWriteStringXY(0,0,msg);
			
			_delay_ms(3000);
			
		}	
		else
		{
			LCDWriteString("Err Reading Msg !");
			
			_delay_ms(3000);
			
		}
	
		
	//Send MSG
	
	LCDClear();
	LCDWriteString("Sending Msg");
	_delay_ms(2000);
	uint8_t ref;
	
	char msgSend[60] = "";
		
	strcat(msgSend,"Humidity :");
	strcat(msgSend,Humstr);
	strcat(msgSend,"%");
	strcat(msgSend," , ");
	strcat(msgSend,"Temperature :");
	strcat(msgSend,Tempstr);
	strcat(msgSend,"C ");
	if(bit_is_clear(PINA,0)) //If rain is came
	{
		strcat(msgSend,"Raining");
		//PORTD &=~(1<<PIND7); //Turns ON buzzer
		//_delay_ms(1000); //3 second delay
		//PORTD |=(1<<PIND7); //Turns OFF buzzer
		//_delay_ms(1000);
	}else
	{
		strcat(msgSend,"Not Raining");
	}
	LCDClear();
	//LCDWriteString(msgSend);
	//_delay_ms(5000);

	
	r=SIM300SendMsg("Enter Your Number Here",msgSend,&ref);//Change phone number to some valid value!
	_delay_ms(2000);
	
	LCDWriteStringXY(0,1,"Success");
	LCDWriteIntXY(9,1,ref,3);
	
	
	if(r==SIM300_OK)
	{
		LCDWriteStringXY(0,1,"Success");
		LCDWriteIntXY(9,1,ref,3);
	}
	else if(r==SIM300_TIMEOUT)
	{
		LCDWriteStringXY(0,1,"Time out !");
	}
	else
	{
		LCDWriteStringXY(0,1,"Fail  !");	
	}
	
	_delay_ms(2000);

		
		//Finally delete the msg
		if (SIM300DeleteMsg(id)!=SIM300_OK)
		{
			LCDWriteString("Err Deleting Msg !");
			
			_delay_ms(3000);	
		}
	
	}
		
	
	Halt();
}

void Halt()
{
	while(1);
}


unsigned char rxvalue(void){
	while(!(UCSRA&(1<<RXC)));
	{
		rxdata=UDR;
		return rxdata;
	}
}
 




void InitADC()
{
	ADMUX=(1<<REFS0);                       // For Aref=AVcc;
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Prescalar div factor =128
}

uint16_t ReadADC(uint8_t ch)
{
	//Select ADC Channel ch must be 0-7
	ch=ch&0b00000111;
	ADMUX|=ch;
	//Start Single conversion
	ADCSRA|=(1<<ADSC);
	//Wait for conversion to complete
	while(!(ADCSRA & (1<<ADIF)));
	//Clear ADIF by writing one to it
	//Note you may be wondering why we have write one to clear it
	//This is standard way of clearing bits in io as said in data sheets.
	//The code writes '1' but it result in setting bit to '0' !!!
	ADCSRA|=(1<<ADIF);
	_delay_ms(100);
	return(ADC);
}

void TempControl(int tem){
	dht_gettemperaturehumidity(&temperature, &humidity);
	LCDClear();
	
	LCDWriteStringXY(0,0,"Hum:");
	
	LCDWriteStringXY(0,1,"Temp:");
	//while(1)
	{
		
		if(dht_gettemperaturehumidity(&temperature, &humidity) != -1) {
			
			itoa(humidity,Humstr, 10);
			dtostrf(temperature,4,1,Tempstr);
			
			LCDWriteStringXY(6,0,Humstr);
			LCDWriteStringXY(9,0,"% RH");
			
			LCDWriteStringXY(6,1,Tempstr);
			//Lcd4_Write_Char(223);
			LCDWriteString("C");
			//_delay_ms(1000);
			
			
		}
		else{
			LCDClear();
			
			LCDWriteStringXY(1,0,"Internal Error");
			
			LCDWriteStringXY(2,0,"Process Terminated");
			_delay_ms(1000);
			//break;
		}
		//_delay_ms(1000);//check humidity every second
	}
	
}

void TempControl1(){
	char t[5];
	LCDClear();
	while(1)
	{
		ReadADC(0);
		adc_result0=ADC;
		double temperature=(adc_result0/1024.0*500)-6;
		dtostrf(temperature,5,0,t);
		
		LCDClear();
		
		LCDWriteStringXY(1,0,"Temperature");
		
		LCDWriteStringXY(2,1,t);
		//Lcd4_Write_Char(223);
		LCDWriteString("C");
		_delay_ms(500);
		
		if (temperature>60.0&&temperature<80.0)
		{
			PORTC &=~(1<<PINC0);
			PORTC &=~(1<<PINC1);
		}
		else if (temperature>80.0)
		{
			PORTC |=(1<<PINC0);
			PORTC |=(1<<PINC1);
		}
		else
		{
            PORTC |= (1<<PINC0); 
			PORTC &=~(1<<PINC0);  
		}
		
		/*if(t<30){
		PORTB |=1<<PINB0;  //output valve
			_delay_ms(5000);

			PORTB &=~(1<<PINB0);
			_delay_ms(5000);

			PORTB^=1<<PIN1;  //input valve
			_delay_ms(5000);

		}*/
	}

	

	
	
}
