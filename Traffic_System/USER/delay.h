#ifndef _DELAY_
#define _DELAY_
#include "stm32f10x.h"                  // Device header

void SysTick_Init(void);
void SysTick_Handler(void);
void Delay(uint32_t time);
uint32_t GetTick(void);

#endif
