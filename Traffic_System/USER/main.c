/*===================================================================================*/
#include "stm32f10x.h"                  // Device header
#include "delay.h"
/*===================================================================================*/
#define red_time 31000
#define green_time 28000
#define red_time_pr 100000
#define green_time_pr 97000
/*===================================================================================*/
//7 segment 1
#define A1 GPIO_Pin_0	//Port A
#define B1 GPIO_Pin_1
#define C1 GPIO_Pin_2
#define D1 GPIO_Pin_3
#define E1 GPIO_Pin_4
#define F1 GPIO_Pin_5
#define G1 GPIO_Pin_6
#define DIGIT1V GPIO_Pin_0	//Port B
#define DIGIT2V GPIO_Pin_1
//7 segment 2
#define A2 GPIO_Pin_10	//Port A
#define B2 GPIO_Pin_9
#define C2 GPIO_Pin_8
#define D2 GPIO_Pin_15	//Port B
#define E2 GPIO_Pin_14
#define F2 GPIO_Pin_13
#define G2 GPIO_Pin_12
#define DIGIT1H GPIO_Pin_10	//Port B
#define DIGIT2H GPIO_Pin_11
//Traffic V
#define RedV GPIO_Pin_3	//Port B
#define YellowV GPIO_Pin_4
#define GreenV GPIO_Pin_5
//Traffic H
#define RedH GPIO_Pin_15	//Port A
#define YellowH GPIO_Pin_12
#define GreenH GPIO_Pin_11
//Button
#define BUTTON_DEFAULT GPIO_Pin_6
#define BUTTON_NIGHT GPIO_Pin_7
#define BUTTON_PR GPIO_Pin_8
/*===================================================================================*/
typedef enum {RED_1, YELLOW_1, GREEN_1} Traffic_1;

typedef enum {RED_2, YELLOW_2, GREEN_2} Traffic_2;

/*===================================================================================*/
GPIO_InitTypeDef gpio;
unsigned char led7Hex[]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
int idx1[]={A1, B1, C1, D1, E1, F1, G1};
int idx2[]={A2, B2, C2, D2, E2, F2, G2};
/*===================================================================================*/
void GPIO_Config(void);
void Red_V(void);
void Yellow_V(void);
void Green_V(void);
void Red_H(void);
void Yellow_H(void);
void Green_H(void);
void LED7Write_V(short digit, unsigned char b);
void LED7Write_H(short digit, unsigned char b);
void Off_V(void);
void Off_H(void);
/*===================================================================================*/
void Mode_Default(void);
void Mode_Night(void);
void Mode_Priority(void);
void Default_Init(void);
/*===================================================================================*/
int timeRemaining_V = (red_time-1000)/1000;
int timeRemaining_H = (green_time-1000)/1000;
int yellow_time=red_time-green_time;
Traffic_1 currentState_1 = RED_1;
Traffic_2 currentState_2 = GREEN_2;
uint8_t currentDigit = 1;
uint8_t check_V=1, check_H=1;
uint32_t start_1 = 0;
uint32_t start_2 = 0;
uint32_t startScan = 0;
uint32_t startLED_1 = 0;
uint32_t startLED_2 = 0;
uint8_t onoff = 1;
uint8_t bt_default = 1;
uint8_t bt_night;
uint8_t bt_pr;
/*===================================================================================*/
int main(){
	SystemInit();
	GPIO_Config();
	SysTick_Init();
	start_1 = GetTick();
	start_2 = GetTick();
	startScan = GetTick();
	startLED_1 = GetTick();
	startLED_2 = GetTick();
	Red_V();
	Green_H();
	while(1){
		if(GPIO_ReadInputDataBit(GPIOB, BUTTON_DEFAULT) == 0){
			Delay(20);
			bt_default = 1;
			bt_night = 0;
			bt_pr = 0;
			Default_Init();
		}
		if(GPIO_ReadInputDataBit(GPIOB, BUTTON_NIGHT) == 0){
			Delay(20);
			bt_default = 0;
			bt_night = 1;
			bt_pr = 0;
		}
		if(GPIO_ReadInputDataBit(GPIOB, BUTTON_PR) == 0){
			Delay(20);
			bt_default = 0;
			bt_night = 0;
			bt_pr = 1;
			Default_Init();
		}
		if(bt_default) Mode_Default();
		if(bt_night) Mode_Night();
		if(bt_pr) Mode_Priority();
	}
}
/*===================================================================================*/
void GPIO_Config(void){
	//Enable Clock for GPIOA, GPIOB
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2Periph_AFIO;
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//Configuration for Pins of GPIOA
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Pin = A1 | B1 | C1 | D1 | E1 | F1 | G1 |
									A2 | B2 | C2 | RedH | YellowH | GreenH;
	GPIO_Init(GPIOA, &gpio);
	//Configuration for Pins of GPIOB
	gpio.GPIO_Pin = D2 | E2 | F2 | G2 | DIGIT1V | DIGIT2V |
									DIGIT1H | DIGIT2H | RedV | YellowV | GreenV;
	GPIO_Init(GPIOB, &gpio);
	gpio.GPIO_Pin = BUTTON_DEFAULT | BUTTON_NIGHT | BUTTON_PR;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &gpio);
}

void Red_V(){
	GPIO_WriteBit(GPIOB,RedV,Bit_SET);
	GPIO_WriteBit(GPIOB,GreenV,Bit_RESET);
	GPIO_WriteBit(GPIOB,YellowV,Bit_RESET);
}

void Yellow_V(){
	GPIO_WriteBit(GPIOB,RedV,Bit_RESET);
	GPIO_WriteBit(GPIOB,GreenV,Bit_RESET);
	GPIO_WriteBit(GPIOB,YellowV,Bit_SET);
}

void Green_V(){
	GPIO_WriteBit(GPIOB,RedV,Bit_RESET);
	GPIO_WriteBit(GPIOB,GreenV,Bit_SET);
	GPIO_WriteBit(GPIOB,YellowV,Bit_RESET);
}

void Red_H(){
	GPIO_WriteBit(GPIOA,RedH,Bit_SET);
	GPIO_WriteBit(GPIOA,GreenH,Bit_RESET);
	GPIO_WriteBit(GPIOA,YellowH,Bit_RESET);
}

void Yellow_H(){
	GPIO_WriteBit(GPIOA,RedH,Bit_RESET);
	GPIO_WriteBit(GPIOA,GreenH,Bit_RESET);
	GPIO_WriteBit(GPIOA,YellowH,Bit_SET);
}

void Green_H(){
	GPIO_WriteBit(GPIOA,RedH,Bit_RESET);
	GPIO_WriteBit(GPIOA,GreenH,Bit_SET);
	GPIO_WriteBit(GPIOA,YellowH,Bit_RESET);
}

void LED7Write_V(short digit, unsigned char b){
	if(digit==1){
		GPIO_WriteBit(GPIOB, DIGIT2V, Bit_RESET);
		GPIO_WriteBit(GPIOB, DIGIT1V, Bit_SET);
	}
	else if(digit==2){
		GPIO_WriteBit(GPIOB, DIGIT1V, Bit_RESET);
		GPIO_WriteBit(GPIOB, DIGIT2V, Bit_SET);
	}
	for(int i=0;i<7;i++){
		if(((b>>i)&(0x01))==0) GPIO_WriteBit(GPIOA, idx1[i], Bit_RESET);
		else GPIO_WriteBit(GPIOA, idx1[i], Bit_SET);
	}
}

void LED7Write_H(short digit, unsigned char b){
	if(digit==1){
		GPIO_WriteBit(GPIOB, DIGIT2H, Bit_RESET);
		GPIO_WriteBit(GPIOB, DIGIT1H, Bit_SET);
	}
	else if(digit==2){
		GPIO_WriteBit(GPIOB, DIGIT1H, Bit_RESET);
		GPIO_WriteBit(GPIOB, DIGIT2H, Bit_SET);
	}
	for(int i=0;i<7;i++){
		if(((b>>i)&(0x01))==0){
			if(i<3){
				GPIO_WriteBit(GPIOA, idx2[i], Bit_RESET);
			}
			else{
				GPIO_WriteBit(GPIOB, idx2[i], Bit_RESET);
			}
		}
		else{
			if(i<3){
				GPIO_WriteBit(GPIOA, idx2[i], Bit_SET);
			}
			else{
				GPIO_WriteBit(GPIOB, idx2[i], Bit_SET);
			}
		}
	}
}

void Off_V(){
	for(int i=0;i<7;i++) GPIO_WriteBit(GPIOA, idx1[i], Bit_SET);
}

void Off_H(){
	for(int i=0;i<7;i++){
		if(i<3){
			GPIO_WriteBit(GPIOA, idx2[i], Bit_SET);
		}
		else{
			GPIO_WriteBit(GPIOB, idx2[i], Bit_SET);
		}
	}
}

void Mode_Default(void){
	uint32_t currentTick = GetTick();
		
		switch(currentState_1){
			case RED_1:
				if(currentTick - start_1 >= 1000){
					timeRemaining_V--;
					if(timeRemaining_V<0) timeRemaining_V = (green_time-1000)/1000;
					start_1=currentTick;
				}
				if(currentTick - startLED_1 >= red_time){
					Green_V();
					currentState_1=GREEN_1;
					startLED_1=currentTick;
				}
				break;
			case YELLOW_1:
				if(currentTick - start_1 >= yellow_time){
					check_V=1;
					timeRemaining_V = (red_time-1000)/1000;
					start_1=currentTick;
				}
				if(currentTick - startLED_1 >= yellow_time){
					Red_V();
					currentState_1=RED_1;
					startLED_1=currentTick;
				}
				break;
			case GREEN_1:
				if(currentTick - start_1 >= 1000){
					timeRemaining_V--;
					if(timeRemaining_V<0) check_V=0;
					start_1=currentTick;
				}
				if(currentTick - startLED_1 >= green_time){
					Yellow_V();
					currentState_1=YELLOW_1;
					startLED_1=currentTick;
				}
				break;
		}
		switch(currentState_2){
			case RED_2:
				if(currentTick - start_2 >= 1000){
					timeRemaining_H--;
					if(timeRemaining_H<0) timeRemaining_H = (green_time-1000)/1000;
					start_2=currentTick;
				}
				if(currentTick - startLED_2 >= red_time){
					Green_H();
					currentState_2=GREEN_2;
					startLED_2=currentTick;
				}
				break;
			case YELLOW_2:
				if(currentTick - start_2 >= yellow_time){
					check_H=1;
					timeRemaining_H = (red_time-1000)/1000;;
					start_2=currentTick;
				}
				if(currentTick - startLED_2 >= yellow_time){
					Red_H();
					currentState_2=RED_2;
					startLED_2=currentTick;
				}
				break;
			case GREEN_2:
				if(currentTick - start_2 >= 1000){
					timeRemaining_H--;
					if(timeRemaining_H<0) check_H=0;
					start_2=currentTick;
				}
				if(currentTick - startLED_2 >= green_time){
					Yellow_H();
					currentState_2=YELLOW_2;
					startLED_2=currentTick;
				}
				break;
		}
		
		if(currentTick - startScan >=5){
			short digit1_V = timeRemaining_V/10;
			short digit2_V = timeRemaining_V%10;
			short digit1_H = timeRemaining_H/10;
			short digit2_H = timeRemaining_H%10;
			if(currentDigit==1){
				if(check_V) LED7Write_V(1, led7Hex[digit1_V]);
				else Off_V();
				if(check_H) LED7Write_H(1, led7Hex[digit1_H]);
				else Off_H();
				currentDigit=2;
			}
			else{
				if(check_V) LED7Write_V(2, led7Hex[digit2_V]);
				else Off_V();
				if(check_H) LED7Write_H(2, led7Hex[digit2_H]);
				else Off_H();
				currentDigit=1;
			}
			startScan=currentTick;
		}
}
void Mode_Night(void){
	Off_V();
	Off_H();
	if(GetTick() - start_1 >= 500){
		start_1 = GetTick();
	  switch(onoff){
			case 0:
				GPIO_WriteBit(GPIOB,YellowV,Bit_RESET);
				GPIO_WriteBit(GPIOA,YellowH,Bit_RESET);
				onoff = !onoff;
				break;
			case 1:
				Yellow_V();
				Yellow_H();
				onoff = !onoff;
				break;
		}
	}
}
void Mode_Priority(void){
	uint32_t currentTick = GetTick();
		
		switch(currentState_1){
			case RED_1:
				if(currentTick - start_1 >= 1000){
					timeRemaining_V--;
					if(timeRemaining_V<0) timeRemaining_V = (green_time_pr-1000)/1000;
					start_1=currentTick;
				}
				if(currentTick - startLED_1 >= red_time){
					Green_V();
					currentState_1=GREEN_1;
					startLED_1=currentTick;
				}
				break;
			case YELLOW_1:
				if(currentTick - start_1 >= yellow_time){
					check_V=1;
					timeRemaining_V = (red_time-1000)/1000;
					start_1=currentTick;
				}
				if(currentTick - startLED_1 >= yellow_time){
					Red_V();
					currentState_1=RED_1;
					startLED_1=currentTick;
				}
				break;
			case GREEN_1:
				if(currentTick - start_1 >= 1000){
					timeRemaining_V--;
					if(timeRemaining_V<0) check_V=0;
					start_1=currentTick;
				}
				if(currentTick - startLED_1 >= green_time_pr){
					Yellow_V();
					currentState_1=YELLOW_1;
					startLED_1=currentTick;
				}
				break;
		}
		switch(currentState_2){
			case RED_2:
				if(currentTick - start_2 >= 1000){
					timeRemaining_H--;
					if(timeRemaining_H<0) timeRemaining_H = (green_time-1000)/1000;
					start_2=currentTick;
				}
				if(currentTick - startLED_2 >= red_time_pr){
					Green_H();
					currentState_2=GREEN_2;
					startLED_2=currentTick;
				}
				break;
			case YELLOW_2:
				if(currentTick - start_2 >= yellow_time){
					check_H=1;
					timeRemaining_H = (red_time_pr-1000)/1000;;
					start_2=currentTick;
				}
				if(currentTick - startLED_2 >= yellow_time){
					Red_H();
					currentState_2=RED_2;
					startLED_2=currentTick;
				}
				break;
			case GREEN_2:
				if(currentTick - start_2 >= 1000){
					timeRemaining_H--;
					if(timeRemaining_H<0) check_H=0;
					start_2=currentTick;
				}
				if(currentTick - startLED_2 >= green_time){
					Yellow_H();
					currentState_2=YELLOW_2;
					startLED_2=currentTick;
				}
				break;
		}
		
		if(currentTick - startScan >=5){
			short digit1_V = timeRemaining_V/10;
			short digit2_V = timeRemaining_V%10;
			short digit1_H = timeRemaining_H/10;
			short digit2_H = timeRemaining_H%10;
			if(currentDigit==1){
				if(check_V) LED7Write_V(1, led7Hex[digit1_V]);
				else Off_V();
				if(check_H) LED7Write_H(1, led7Hex[digit1_H]);
				else Off_H();
				currentDigit=2;
			}
			else{
				if(check_V) LED7Write_V(2, led7Hex[digit2_V]);
				else Off_V();
				if(check_H) LED7Write_H(2, led7Hex[digit2_H]);
				else Off_H();
				currentDigit=1;
			}
			startScan=currentTick;
		}
}

void Default_Init(void){
	timeRemaining_V = (red_time-1000)/1000;
	timeRemaining_H = (green_time-1000)/1000;
	yellow_time=red_time-green_time;
	currentState_1 = RED_1;
	currentState_2 = GREEN_2;
	currentDigit = 1;
	check_V=1, check_H=1;
	start_1 = 0;
	start_2 = 0;
	startScan = 0;
	startLED_1 = 0;
	startLED_2 = 0;
	onoff = 1;
	start_1 = GetTick();
	start_2 = GetTick();
	startScan = GetTick();
	startLED_1 = GetTick();
	startLED_2 = GetTick();
	Red_V();
	Green_H();
}
