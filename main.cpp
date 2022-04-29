// main.cpp
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10 in C++

// Last Modified: 1/17/2020 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2017

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "SlidePot.h"
#include "Images.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"

//#include "structures.h"
//#include "presets.h"
#include "topping.h"

SlidePot my(1500,0);

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

// Creating a struct for the Sprite.
typedef enum {dead,alive} status_t;
struct sprite{
  uint32_t x;      // x coordinate
  uint32_t y;      // y coordinate
  const unsigned short *image; // ptr->image
  status_t life;            // dead/alive
};          
typedef struct sprite sprite_t;

/*sprite_t bill={60,9,SmallEnemy20pointB,alive};

uint32_t time = 0;
volatile uint32_t flag;
void background(void){
  flag = 1; // semaphore
  if(bill.life == alive){
    bill.y++;
  }
  if(bill.y>155){
    bill.life = dead;
  }
}
void clock(void){
  time++;
}*/

// **************SysTick_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void SysTick_Init(uint32_t period){
  //*** students write this ****** (copied from Lab 6)
	NVIC_ST_RELOAD_R = period-1;  // reload value
  NVIC_ST_CURRENT_R = 0;       // any write will reload counter and clear count
  NVIC_SYS_PRI3_R =  (NVIC_SYS_PRI3_R&0x00FFFFFF)|(2<<29); 
  NVIC_ST_CTRL_R = 0x07;
	
}

uint32_t Data;      // 12-bit ADC



	int D3last = 0;
	int D2last = 0;
	int D1last = 0;
	int D0last = 0;
	int D3now = 0;
	int D2now = 0;
	int D1now = 0;
	int D0now = 0;


void PortD_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x08;
	__nop();
	__nop();
	__nop();
	GPIO_PORTD_DEN_R |= 0x0F;
	GPIO_PORTD_DIR_R &= ~0x0F;
}

uint16_t preimage[100];

int main(void){
	
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB); 
	ST7735_InvertDisplay(1);
	SysTick_Init(8000000);
	
	PortD_Init();
	
	EnableInterrupts();
	
	//ST7735_DrawTransparent(20, 80 + 20, test, 64, 80);
	//station = BUILDING;
	//topping_default();
	//while(1){}
	
	ST7735_DrawBitmap(0 + 20, 80 + 20, test, 64,80, (uint16_t) 0x0000);
	uint8_t xold = 0;
	uint8_t yold =0;
	uint8_t x = 20;
	uint8_t y = 20;
	
	int increment = 1;
	
	while(1){
		if(D3last > D3now){
			yold = y;
			y++;
			D3last = D3now;
		}
		if(D2last > D2now){
			xold = x;
			x++;
			D2last = D2now;
		}
		if(D1last > D1now){
			xold = x;
			x--;
			D1last = D1now;
		}
		if(D0last > D0now){
			yold = y;
			y--;
			D0last = D0now;
		}
		
		/*x += increment;
		if(x == 54 ){ increment = 0; y++; if(y%10 == 0){ 
		increment = -1; 
		} }
		if(x == 0) { increment = 1; }*/

		
		for(int i = 0; i < 10; i++){
			for(int j = 0;j < 10; j++){
				preimage[(9 - i)*10 + j] = test[((79 - ((y - 10 - 20) + i)))*64 + (x - 20+ j)];
				//preimage[(9 - i)*10 + (9 - j)] = test[((79 - ((y - 10) + i)))*64 + (x - 10 + j)];
				//preimage[i*9 + j] = test[((y - 10) + j)*63 + i];
				/*if(!Pointer[i*9 + j]){
					ST7735_DrawPixel(x + (9 - i), y - 10 + (9 - j), 0x1234); //PizzaBase[x+i*99 + (y + 9 - j)]);
				}*/
			}
		}
		
		ST7735_DrawBitmap(x, y, Pointer, 10, 10, preimage);

		
		
	}
}

/*int main(void){



  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  // TExaS_Init();
  Random_Init(1);
  Output_Init();
  Timer0_Init(&background,1600000); // 50 Hz
  Timer1_Init(&clock,80000000); // 1 Hz
  EnableInterrupts();
  ST7735_DrawBitmap(52, 159, PlayerShip0, 18,8); // player ship middle bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);
  while(bill.life == alive){
    while(flag==0){};
    flag = 0;
    ST7735_DrawBitmap(bill.x,bill.y,bill.image,16,10);
  }

  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString((char*)"GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_SetTextColor(ST7735_WHITE);
  ST7735_OutString((char*)"Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString((char*)"Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_SetTextColor(ST7735_WHITE);
  while(1){
    while(flag==0){};
    flag = 0;
    ST7735_SetCursor(2, 4);
    ST7735_OutUDec(time);
  }

}*/




void SysTick_Handler(void){
	D3last = D3now;
	D2last = D2now;
	D1last = D1now;
	D0last = D0now;
	if((GPIO_PORTD_DATA_R & 0x01) != 0){
		D0now = 1;
	}
	else{
		D0now = 0;
	}
	if((GPIO_PORTD_DATA_R & 0x02) != 0){
		D1now = 1;
	}
	else{
		D1now = 0;
	}
	if((GPIO_PORTD_DATA_R & 0x04) != 0){
		D2now = 1;
	}
	else{
		D2now = 0;
	}
	if((GPIO_PORTD_DATA_R & 0x08) != 0){
		D3now = 1;
	}
	else{
		D3now = 0;
	}
	//Sensor.Save(ADC_In());
}



