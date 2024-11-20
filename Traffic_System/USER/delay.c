#include "delay.h"
#include "stm32f10x.h"                  // Device header

volatile uint32_t tick = 0;

void SysTick_Init(void);
void SysTick_Handler(void);
void Delay(uint32_t time);
uint32_t GetTick(void);

void SysTick_Init(void){
	SysTick_Config(SystemCoreClock/1000);
}

void SysTick_Handler(void){
	tick++;
}

void Delay(uint32_t time){
	uint32_t startTick = tick;
	while(tick - startTick < time);
}

uint32_t GetTick(void){
	return tick;
}
