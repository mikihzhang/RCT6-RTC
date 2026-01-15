#ifndef _GPIO_H
#define _GPIO_H
extern uint16_t stop_flag;

//节点拨码开关
#define BOMA_PORT1   GPIO_Pin_15 //PA15
#define BOMA_PORT2   GPIO_Pin_10 //PC10
#define BOMA_PORT3   GPIO_Pin_11 //PC11
#define BOMA_PORT4   GPIO_Pin_12 //PC12
#define BOMA_PORT5   GPIO_Pin_2  //PD2
#define BOMA_PORT6   GPIO_Pin_3  //PB3
#define BOMA_PORT7   GPIO_Pin_4  //PB4
#define BOMA_PORT8   GPIO_Pin_5  //PB5

//机箱拨码开关
#define BOMA_BOX1   GPIO_Pin_2 //PB2
#define BOMA_BOX2   GPIO_Pin_1 //PA1
#define BOMA_BOX3   GPIO_Pin_0 //PA0
#define BOMA_BOX4   GPIO_Pin_15  //PC15
#define BOMA_BOX5   GPIO_Pin_14  //PC14
#define BOMA_BOX6   GPIO_Pin_13  //PC13

//光电池数量拨码开关
#define BOMA_PD1   GPIO_Pin_9 //PA9
#define BOMA_PD2   GPIO_Pin_10 //PA10
#define BOMA_PD3   GPIO_Pin_11 //PA11
#define BOMA_PD4   GPIO_Pin_12  //PA12

//CS引脚控制MCP41010芯片地址
#define MCP_CS1   GPIO_Pin_7     //PA7
#define MCP_CS2   GPIO_Pin_3     //PC3
#define MCP_CS3   GPIO_Pin_1     //PB1
#define MCP_CS4   GPIO_Pin_12    //PB12

//通断控制
#define BROKE1   GPIO_Pin_6     //PA6
#define BROKE2   GPIO_Pin_2     //PC2
#define BROKE3   GPIO_Pin_0     //PB0
#define BROKE4   GPIO_Pin_13      //PB13
//ADS芯片地址控制
#define ADS1   GPIO_Pin_4       //PA4
#define ADS2   GPIO_Pin_0       //PC0
#define ADS3   GPIO_Pin_4       //PC4
#define ADS4   GPIO_Pin_15       //PB15
//DS18B20
#define DS18B20_PIN1  GPIO_Pin_5 //PA5
#define DS18B20_PIN2  GPIO_Pin_1 //PC1
#define DS18B20_PIN3  GPIO_Pin_5 //PC5
#define DS18B20_PIN4  GPIO_Pin_14 //PB14

//ADC的I2C引脚
#define ADC_SCL GPIO_Pin_10     //PB10
#define ADC_SDA GPIO_Pin_11     //PB11 
//中断定义
#define EXTI0_IRQHandler        EXTI0_IRQHandler
#define EXTI1_IRQHandler        EXTI1_IRQHandler
#define EXTI2_IRQHandler        EXTI2_IRQHandler
#define EXTI9_5_IRQHandler      EXTI9_5_IRQHandler


void MCU_GPIO_Init(void);

#endif