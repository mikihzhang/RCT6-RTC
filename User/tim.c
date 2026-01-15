/*
 * tim.c
 *
 *  Created on: Jul 8, 2024
 *      Author: guangxin
 */

#include "debug.h"
#include "tim.h"
#include "eth_driver.h"
#include "UDP.h"
void TIM1_Int_Init(u16 psc, u16 arr) {
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseStructure = {0};
	NVIC_InitTypeDef 					NVIC_InitStructure = {0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);       //使能TIM1时钟

	TIM_TimeBaseStructure.TIM_Period = arr;       //指定下次更新事件时要加载到活动自动重新加载寄存器中的周期值。
	TIM_TimeBaseStructure.TIM_Prescaler = psc;                //指定用于划分TIM时钟的预分频器值。
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //时钟分频因子
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM计数模式，向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);        //根据指定的参数初始化TIMx的时间基数单位

	//初始化TIM NVIC，设置中断优先级分组
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;         //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //设置抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //设置响应优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //使能通道1中断
	NVIC_Init(&NVIC_InitStructure);                            //初始化NVIC

	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); //使能TIM1中断，允许更新中断

	TIM_Cmd(TIM1, ENABLE); //TIM1使能
}

void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}










