#include "ch32v20x_it.h"
#include "eth_driver.h"
#include "UDP.h"
#include "GPIO.h"
// 删除: #include "BOMA.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void NMI_Handler(void) {}

void HardFault_Handler(void) {
	printf("HardFault_Handler\r\n");
	printf("mcause:%08x\r\n", __get_MCAUSE());
	printf("mtval:%08x\r\n", __get_MTVAL());
	printf("mepc:%08x\r\n", __get_MEPC());
	while (1);
}

void ETH_IRQHandler(void) {
	WCHNET_ETHIsr();
}

void TIM2_IRQHandler(void)
{
  WCHNET_TimeIsr(WCHNETTIMERPERIOD);
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

void TIM1_UP_IRQHandler(void) {
	// 旧的传感器/BOMA上报逻辑已删除
	// 如果没有其他功能使用TIM1，这个函数可以留空，或者在main中不初始化TIM1
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

