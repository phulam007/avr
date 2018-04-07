/*
 * main.c
 *
 *  Created on: May 13, 2016
 *      Author: Administrator
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "global.h"
#include "board.h"
#include "uart.h"
#include "lcd20x4.h"
#include <string.h>


uint8_t tag_index();
void updateTagid();

const unsigned char defPin[11] = "123456789\0";
unsigned char password[11];
unsigned char str[20];
unsigned char buffer[11];
unsigned char tagId[50][11];
uint8_t doorState=0;   // 0 : close			1: open
volatile uint16_t counterTimer;	// dem thoi gian mo cua	
volatile uint8_t mode=0;		// che do hoat dong 		tu dong || quet the
volatile uint8_t timeClose;
uint8_t timeClose_;
uint8_t timeClose_t;
uint8_t distanceIn;
uint8_t distanceOut;
uint8_t minDistancein;
uint8_t minDistanceout;
uint8_t tag = 0;		// tag .... no tag
uint8_t tagNum;			// so luong tag luu trong eeprom
uint8_t tagCompare; 	// doc xong the tag bat dau so sanh.
uint8_t tagIndex[50];//
const unsigned char menu1[4][20] = {"   TRUONG DAI HOC   ",
									" GIAO THONG VAN TAI ",
									"       TP. HCM      ",
									"KHOA: DIEN - DTVT   "};
const unsigned char menu2[4][20] = {"DO AN: CUA TU DONG  ",
									"GVHD: PHAM THUY OANH",
									"SV: PHAM ANH DUY    ",
									"NGUYEN THI XUAN THUY"
									};
const unsigned char menu3[4][20] = {"WELCOME TO AUTOMATIC",
									"     DOOR SYSTEM    ",
									"                    ",
									"                    "};
const unsigned char menu4[4][20] = {"     SETTING MODE   ",
									"PASSWORD:           ",
									"                    ",
									"                    ",};
const unsigned char menu5[10][20] = {"     SETTING MODE   ",
									"1. SET SENSOR 1     ",
									"2. SET SENSOR 2     ",
									"3. SET MODE         ",
									"4. SET TIME         ",
									"5. ADD TAG ID       ",
									"6. REMOTE TAG ID    ",
									"7. LIST TAG ID      ",
									"8. CHANGE PASSWORD  ",
									"9. TEST SENSOR      "};

static FILE uart_ = FDEV_SETUP_STREAM(sendByte, NULL, _FDEV_SETUP_WRITE);
static FILE lcd_ = FDEV_SETUP_STREAM(lcd_chr, NULL, _FDEV_SETUP_WRITE);
#define TIMEKEY	15000


int main()
{
	u08 distance, distance1;
	u08 key, menu=0, ct= 0;
	u08 tmp=0;
	u08 idraw = 0;
	eeprom_busy_wait();
	ct = eeprom_read_byte(FIRSTINIT);
	if(ct!=1){
		for(u08 i = 0; i< PIN_LEN; i++)
		{
			eeprom_write_byte(PIN_EEPROM+i,defPin[i]);
		}
		eeprom_write_byte(FIRSTINIT,1);
		}
		// doc ve ma pin trong eeprom
		for(u08 i = 0; i< PIN_LEN; i++)
			{
				password[i] = eeprom_read_byte(PIN_EEPROM+i);
				password[i+1] = '\0';
			}
		//	eeprom_write_byte(TAGNUM,0);
		// doc ve thong so 
		distanceIn = eeprom_read_byte(MAXDIS1);			// max distance1
		distanceOut = eeprom_read_byte(MAXDIS2);			// max distance2
		minDistancein = eeprom_read_byte(MINDIS1);		// min distance1
		minDistanceout = eeprom_read_byte(MINDIS2);		// min distance2
		mode = eeprom_read_byte(MODE);					// mode
		tagNum = eeprom_read_byte(TAGNUM);				// tag number
		timeClose_ = eeprom_read_byte(TIMECLOSE);
		timeClose_t = eeprom_read_byte(TIMECLOSE1);
		for(u08 i=0; i<50; i++)
		{
			tagIndex[i] = eeprom_read_byte(TAGINDEX+i);
		}
	updateTagid();
    ct = 0;
	gpioInit();
	uartInit();
	lcd_init();
	timeClose = timeClose_;
	doorState = 2;
	// timer 1 init
	TCCR1B |= (1 << CS12);
	TCNT1 = 0;
	// timer 0 init
	TCNT2 = 40;
	TCCR2 |= (1<<CS22)  ; // 1/256
	TIMSK |= (1 << TOIE2);
	sei();
	draw_menu(menu1[0],0);
	draw_menu(menu1[1],1);
	draw_menu(menu1[2],2);
	draw_menu(menu1[3],3);
	_delay_ms(1000);
	lcd_cmd(0x01);
	lcd_gotoxy(0,0);
	draw_menu(menu2[0],0);
	draw_menu(menu2[1],1);
	draw_menu(menu2[2],2);
	draw_menu(menu2[3],3);
	_delay_ms(1000);
	idraw = 1;
	while(1)
	{
		distance = getDistance(1000);
		distance1 = getDistance1(1000);
		switch(menu)
		{
		case 0 : if(idraw){
				 draw_menu(menu3[0],0); // MAN HINH WELCOME
				 draw_menu(menu3[1],1);
				 draw_menu(menu3[2],2);
				 draw_menu(menu3[3],3);
				 idraw = 0;
				 mode = eeprom_read_byte(MODE); 
				}
				 key = keyScan(TIMEKEY);	// mode button
				 if(key == 14) menu = 1;	// man hinh dang nhap
				 break;
		case 1 : 	// MAN HINH DANG NHAP
						lcd_cmd(0x0c);
						draw_menu(menu4[0],0);
						draw_menu(menu4[1],1);
						draw_menu(menu4[2],2);
						draw_menu(menu4[3],3);
						menu = 2;
						lcd_gotoxy(9,1);
						lcd_cmd(0x0e);  //
						break;
		case 2 :	{// NHAP MAT KHAU
					 key = keyScan(TIMEKEY);
					 if(keyScanlcd(key,9,1,&str,&ct,1,PIN_LEN))
					 	 {
						 	 lcd_gotoxy(0,2);
						 	 if(strcmp(str, password)==0)
						 	 {
						 		 ct = 0;
						 	 	 menu = 3;
						 	 	 lcd_cmd(0x01); // CLEAR SCREEN
								 if(mode==2) mode = 3;
						 	 }
						 	 else
						 	 {
						 		 fprintf(&lcd_,"PASSWORD IS WRONG!");
						 	 	 ct = 0;
						 	 	 menu = 0;
						 	 	 _delay_ms(500);
						 	 }
					 	 }
					 if(key==15){menu=0; ct=0;idraw = 1;}
					 }
				 break;
		case 3 :	// MENU SETTING
					lcd_cmd(0x0c);
					draw_menu(menu5[0],0);
					for(uint8_t i = ct+1; i < 4 + ct; i++)
						draw_menu(menu5[i],i-ct);
					menu = 4;
		case 4 :	// SETTING
					key = keyScan(TIMEKEY);
					if(key == 12) if(ct<6){ct++;menu=3;}
					if(key == 11) if(ct>0){ct--;menu=3;}
					if(key == '1'){ 
									menu=5;	
									idraw = 1;
									ct = 0;
								  }
					if(key == '2'){						// THIET LAP KHOANG CACH NHO NHAT, CAO NHAT CUA SENSOR 2
									menu=6;		
									idraw = 1;
									ct = 0;
								   }
					if(key == '3'){						// THIET LAP CHE DO DIEU KHIEN... 1. QUET THE TAG. 2. TU DONG MO CUA
									menu=7;	
									idraw = 1;
									ct = 0;
								   }
					if(key == '4'){						// 
									menu=8;	
									idraw=1;
									ct=0;
								   }
					if(key == '5'){
									menu = 9;
									idraw=1;
									ct = 0;
									}
					if(key=='7'){
									menu = 10;
									idraw=1;
									ct = 0;
								}		
					if(key=='6'){
									menu = 11;
									idraw=1;
									ct=0;
					}
					if(key=='8'){
									menu = 12;
									idraw=1;
									ct=0;
					}
					if(key=='9'){
									menu = 13;
									idraw = 1;
									ct = 0;
					}
					if(key==15){menu=0; ct=0; idraw = 1;}
					break;
		case 5 : // 
					if(idraw)
					{
						lcd_cmd(0x01); // THIET LAP KHOANG CACH NHO NHAT, CAO NHAT CUA SENSOR 1
						lcd_gotoxy(0,0);
						lcd_cmd(0x0c);
						fprintf(&lcd_,"   SET SENSOR 1  ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"1. MIN DISTANCE");
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"2. MAX DISTANCE");
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(key=='1')
					{
						lcd_cmd(0x01); // clear
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"  MIN DISTANCE 1  ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"MIN:%d cm", minDistancein);
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"MIN:");
						menu = 51;
					}
					if(key=='2')
					{
						lcd_cmd(0x01); // clear
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"  MAX DISTANCE 1  ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"MAX:%d cm", distanceIn);
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"MAX:");
						menu = 52;
					}
					if(key==15) menu = 3;
					break;
		case 51 : 	key = keyScan(TIMEKEY);
					if(keyScanlcd(key,4,2,&str,&ct,0,2))
					{
						if(ct>1)
							minDistancein = (str[0] - 48)* + (str[1] - 48);
						else
							minDistancein = (str[0] - 48);
						eeprom_write_byte(MINDIS1, minDistancein);		// ghi vao eeprom
							menu = 5;
							idraw = 1; 
							ct = 0;
					}
					if(key==15) {menu = 5; ct = 0; idraw = 1;}
					break;
		case 52 : 	key = keyScan(TIMEKEY);
					if(keyScanlcd(key,4,2,&str,&ct,0,2))
					{
						if(ct>1)
							distanceIn = (str[0] - 48)*10 + (str[1] - 48);
						else
							distanceIn = (str[0] - 48);
						eeprom_write_byte(MAXDIS1, distanceIn);
							menu = 5;
							idraw = 1;
							ct = 0;
					}
					if(key==15) {menu = 5; ct = 0;idraw = 1;}
					break;
		case 6 :    
					if(idraw)
					{
						lcd_cmd(0x01); // THIET LAP KHOANG CACH NHO NHAT, CAO NHAT CUA SENSOR 1
						lcd_gotoxy(0,0);
						lcd_cmd(0x0c);
						fprintf(&lcd_,"   SET SENSOR 2  ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"1. MIN DISTANCE");
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"2. MAX DISTANCE");
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(key=='1')
					{
						lcd_cmd(0x01); // clear
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"  MIN DISTANCE 2  ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"MIN:%d cm", minDistanceout);
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"MIN:");
						menu = 61;
					}
					if(key=='2')
					{
						distanceOut = eeprom_read_byte(MAXDIS2);
						lcd_cmd(0x01); // clear
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"  MAX DISTANCE 2  ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"MAX:%d cm", distanceOut);
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"MAX:");
						menu = 62;
					}
					if(key==15) menu = 3;
					break;
		case 61 : 	key = keyScan(TIMEKEY);
					if(keyScanlcd(key,4,2,&str,&ct,0,2))
					{
							if(ct>1)
									minDistanceout = (str[0] - 48)*10 + (str[1] - 48);
							else
									minDistanceout = (str[0] - 48);
							eeprom_write_byte(MINDIS2, minDistanceout);
							menu = 6;
							idraw = 1;
							ct = 0;
					}
					if(key==15) {menu = 6; ct = 0;idraw = 1;}
					break;
		case 62 : 	key = keyScan(TIMEKEY);
					if(keyScanlcd(key,4,2,&str,&ct,0,2))
					{
							if(ct > 1)
								distanceOut = (str[0] - 48)*10 + (str[1] - 48);
							else
								distanceOut = (str[0] - 48);
							eeprom_write_byte(MAXDIS2, distanceOut);
							menu = 6;
							idraw = 1;
							ct = 0;
					}
					if(key==15) {menu = 6; ct = 0;idraw = 1;}
					break;
		case 7: 	if(idraw){
					lcd_cmd(0x01); // clear
					lcd_gotoxy(0,0);
					lcd_cmd(0x0c);
					fprintf(&lcd_,"    SET MODE   ");
					lcd_gotoxy(0,1);
					if(mode == 1)
					fprintf(&lcd_,"MODE: TAG ID");
					if(mode == 2)
					fprintf(&lcd_,"MODE: LOCK  ");
					if(mode == 0)
					fprintf(&lcd_,"MODE: AUTO  ");
					lcd_gotoxy(0,2);
					fprintf(&lcd_,"1. TAG ID  2. AUTO  ");
					lcd_gotoxy(0,3);
					fprintf(&lcd_,"3. LOCK AND ALARM   ");
					idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(key=='1')
						{
							mode = 1; // tag id 
							menu = 3;
							eeprom_write_byte(MODE, mode);
						}
					if(key=='2')
						{
							mode = 0; // auto
							menu = 3;
							eeprom_write_byte(MODE, mode);
						}
					if(key=='3')
						{
							mode = 2; // lock and alarm
							menu = 3;
							eeprom_write_byte(MODE, mode);
						}
					if(key == 15){menu = 3;}
					break;
		case 8:		if(idraw)
					{
						lcd_cmd(0x01); // clear
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"    SET TIME       ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"1. AUTO MODE");
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"2. TAG ID MODE");
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(key == '1'){menu = 81; idraw=1; ct = 0;}
					if(key == '2'){menu = 82; idraw=1; ct = 0;}
					if(key == 15){menu = 3;}
					break;
		case 81:	if(idraw)
					{
						lcd_cmd(0x01); // clear
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"    AUTO MODE       ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"TIME:%d", timeClose_);
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"TIME:");
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(keyScanlcd(key,5,2,&str,&ct,0,2))
					{
						if(ct>1)
									timeClose_= (str[0] - 48)*10 + (str[1] - 48);
							else
									timeClose_ = (str[0] - 48);
						eeprom_write_byte(TIMECLOSE, timeClose_);
						menu = 8;
						idraw = 1;
						ct = 0;
					}
					if(key == 15){menu = 8; idraw =1; ct = 0;}
					break;
		case 82: if(idraw)
					{
						lcd_cmd(0x01); // clear
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"    TAG ID MODE       ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"TIME:%d", timeClose_t);
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"TIME:");
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(keyScanlcd(key,5,2,&str,&ct,0,2))
					{
						if(ct>1)
									timeClose_t= (str[0] - 48)*10 + (str[1] - 48);
							else
									timeClose_t = (str[0] - 48);
						eeprom_write_byte(TIMECLOSE1, timeClose_t);
						menu = 8;
						idraw = 1;
						ct = 0;
					}
					if(key == 15){menu = 3; idraw = 1; ct = 0;}
					break;
		case 9:    if(idraw)
					{
						lcd_cmd(0x01); // clear
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"    ADD TAG ID       ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"TAG ID:              ");
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"F2. ADD TAG         ");
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(tagCompare)
					{
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"TAG ID:%s",buffer);
					}
					if(key==12)
						{
							uint8_t k;
							for(k =0; k < tagNum; k++) // kiem tra the tag
							{
								if(strncmp(buffer, tagId[k],10) == 0) break;
							}
							if(k >= tagNum) // neu the dung....
								{
								
									tmp = tag_index();
									for(u08 i =0; i<10; i++)
									{
										eeprom_write_byte(TAGID+11*tmp+i,buffer[i]);
									}
									tagIndex[tmp] = 1;
									eeprom_write_byte(TAGINDEX+tmp,1);
									tagNum++;
									eeprom_write_byte(TAGNUM,tagNum);
									updateTagid();
									menu = 3;
									ct = 0;
									tagCompare = 0;
								}
							else
							{
								lcd_gotoxy(0,3);
								fprintf(&lcd_,"TAG ID IN LIST..");
								_delay_ms(500);
								idraw = 1;
							}
								
						}
					if(key==15){menu=3;ct=0;}
					break;
		case 10:  	
					if(idraw)
					{
					lcd_cmd(0x01);
					fprintf(&lcd_,"        TAG ID         ");
					for(u08 i = 0; i < 3; i++)
						{
							if(tagIndex[i+ct])
							{
								lcd_gotoxy(0,i+1);
								fprintf(&lcd_,"%d. %s",i+ct,tagId[i+ct]);
							}
						}
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(key==12){if((ct+3)<tagNum)ct++;idraw=1;}
					if(key==11){if(ct>0)ct--;idraw=1;}
					if(key==15){menu=3; ct=0;}
					break;
		case 11:  	
					if(idraw)
					{
						lcd_cmd(0x01);
						lcd_gotoxy(0,0);
						fprintf(&lcd_,"   REMOVE TAG ID     ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"TAG ID:              ");
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(keyScanlcd(key,7,1,&str,&ct,0,10))
					{
						uint8_t i;
						for(i =0; i < tagNum; i++) // kiem tra the tag
							{
								if(strncmp(str, tagId[i],10) == 0) break;
							}
						if(i < tagNum) // neu the dung....
							{
								tagIndex[i] = 0;
								eeprom_write_byte(TAGINDEX+i,0);
								tagNum--;
								eeprom_write_byte(TAGNUM,tagNum);
								_delay_ms(10);
								for(u08 k=0; k<50; k++)
										{
											tagIndex[k] = eeprom_read_byte(TAGINDEX+k);
										}
								updateTagid();
								lcd_gotoxy(0,2);
								fprintf(&lcd_,"TAG ID REMOVED..");
								_delay_ms(500);
								idraw = 1;
							}
						else
						{
								lcd_gotoxy(0,2);
								fprintf(&lcd_,"NO TAG ID IN LIST..");
								_delay_ms(500);
								idraw = 1;
						}
					}
					if(key==15){menu=3; ct=0;}
					break;
		case 12: 	if(idraw)
					{
						lcd_cmd(0x01); // THIET LAP KHOANG CACH NHO NHAT, CAO NHAT CUA SENSOR 1
						lcd_gotoxy(0,0);
						lcd_cmd(0x0c);
						fprintf(&lcd_,"     CHANGE PASSWORD  ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"PASSWORD:");
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(keyScanlcd(key,9,1,&str,&ct,0,10))
					{
						for(u08 i =0 ; i < ct; i++)
						{
							eeprom_write_byte(PIN_EEPROM+i,str[i]);
							password[i] = str[i];
							password[i+1] = 0;
						}
						eeprom_write_byte(PIN_EEPROM+ct,0);
						menu = 3;
						ct = 0;
					}
					if(key==15){menu=3; ct=0;}
					break;
		case 13: 	if(idraw)
					{
						lcd_cmd(0x01); // THIET LAP KHOANG CACH NHO NHAT, CAO NHAT CUA SENSOR 1
						lcd_gotoxy(0,0);
						lcd_cmd(0x0c);
						fprintf(&lcd_,"     TEST SENSOR    ");
						lcd_gotoxy(0,1);
						fprintf(&lcd_,"SENSOR 1:%d", distance);
						lcd_gotoxy(0,2);
						fprintf(&lcd_,"SENSOR 2:%d", distance1);
						idraw = 0;
					}
					key = keyScan(TIMEKEY);
					if(key==16){idraw=1;}
					if(key==15){menu=3; ct=0;}
					break;
		
		}
		if(mode==0) // che do mo cua tu dong
		{
			if((distance > minDistancein)&&(distance<distanceIn))// kiem tra cam bien cua vao
			{
				if((doorState==0)||(doorState == 3)) // close .... closing
				{
						timeClose=0;
						doorState = 1;		// opening
				}
			}
			if((distance1 > minDistanceout)&&(distance1<distanceOut)) // kiem tra cam bien cua ra
			{
				if((doorState == 0)||(doorState==3))	// close... closing
					{
					doorState = 1;		// opening
					timeClose=0;
					}
			}
			if(tagCompare) { tagCompare = 0;distanceOut = eeprom_read_byte(MAXDIS2);}
		}
		if(mode == 1)		// che do quet the tag
		{
			uint8_t i;
			if(tagCompare)
			{
				for(i =0; i < tagNum; i++) // kiem tra the tag
				{
					if(strncmp(buffer, tagId[i],10) == 0) break;
				}
				if(i < tagNum) // neu the dung....
				{
					doorState = 1; 			// mo cua..
					//lcd_gotoxy(0,2);
					//fprintf(&lcd_,"OPEN THE DOOR..!");
					//_delay_ms(1000);
				}
				else
				{
					
				}
				tagCompare = 0;
				distanceOut = eeprom_read_byte(MAXDIS2);
			}
			if((distance1 > minDistanceout)&&(distance1<distanceOut)) // kiem tra cam bien cua ra
			{
				if((doorState == 0)||(doorState==3))	// close... closing
					{
					doorState = 1;		// opening
					}
			}
		}
		if(mode == 2)		// che do quet the tag
		{
			sbi(PORTE, COIL);
			if( distance < distanceIn )// kiem tra cam bien cua vao
			{
					sbi(PORTE, BUZZER);
			}
			if(tagCompare)
						{
								if(strncmp(buffer, tagId[0],10) == 0)
									cbi(PORTE, BUZZER);
								tagCompare = 0;
						}
		}
		else
		{
			cbi(PORTE, COIL);
			cbi(PORTE, BUZZER);
		}
	}
	return 0;
}



ISR(TIMER2_OVF_vect)// 5ms interrupt
{
	if(doorState==1)	// opening door
	{
		openDoor();
	}
	if(!(ISWITCH1)&&(doorState==1))
	{
		counterTimer=0;
		stopDoor();
		doorState = 2;
	}
	if(mode==0){
	if((doorState==2)&&(timeClose >= timeClose_))	// open
	{
		closeDoor();
		doorState = 3;
		timeClose = 0;
	}
	}
	if(mode == 1){
		if((doorState==2)&&(timeClose >= timeClose_t))	// open
	{
		closeDoor();
		doorState = 3;
		timeClose = 0;
	}
	}
	if(!(ISWITCH2)&&(doorState==3))
	{
		stopDoor();
		doorState = 0;
	}
	if(doorState==2)
	counterTimer+=1;
	if(counterTimer > 200) {timeClose++;counterTimer=0;}
	TCNT0 = 40; // reset timer0
}

uint8_t counter;
ISR(USART0_RX_vect)
{
	unsigned char c;
	c = UDR0;
	if(tag)
	{
		buffer[counter++] = c;
		buffer[counter] = 0;
	}
	if((c == 0x02)&&(!tag))			// kí tự đầu chuỗi tag
	{
		tag = 1;
	}
	if((c==0x03)&&(tag))
	{
		tag = 0;
		counter = 0;
		tagCompare = 1;			// so sanh id tag
	}
}


uint8_t tag_index()
{
	uint8_t i;
	for(i=0; i<50; i++)
	{
		if(tagIndex[i]==0) break;
	}
	return i;
}

void updateTagid()
{
	uint8_t ct=0;
	uint8_t tmp = 0;
	uint8_t tmp1 = 0;
	tmp1 = tagNum;
	while((tmp1)&&(tmp<50))
		{
			if(tagIndex[tmp]==1){
				for(u08 i = 0; i < 10; i++)
					{
						tagId[ct][i]= eeprom_read_byte(TAGID+11*tmp+i);
						eeprom_write_byte(TAGID+ct*11+i,tagId[ct][i]);
					}
				tagIndex[tmp] = 0;
				eeprom_write_byte(TAGINDEX+tmp,tagIndex[tmp]);
				tagIndex[ct] = 1;
				eeprom_write_byte(TAGINDEX+ct,tagIndex[ct]);
					tmp1--;
					ct++;
			}
			tmp++;
		}
}
