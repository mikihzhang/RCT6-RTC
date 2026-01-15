#include "ch32v20x.h"
#include "GPIO.h"
#include "wchnet.h"
#include "UDP.h"
// #include "BOMA.h"
#include "debug.h"

// 启用时钟
void MCU_GPIO_Init(void)
{   
    
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitStructure = {0};
    // 1. 开启 AFIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    // 启用I2C时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
    //启用ADC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    //MCP41010芯片地址控制引脚
    GPIO_InitStructure.GPIO_Pin = MCP_CS2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = MCP_CS1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = MCP_CS3|MCP_CS4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    // 配置节点序号拨码开关的GPIO口
    GPIO_InitStructure.GPIO_Pin = BOMA_PORT1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    uint16_t pa15_state = GPIOA->INDR & GPIO_Pin_15;
    printf("PA15 raw state: 0x%04X\n", pa15_state);  // 正常应为 0x0000（低）或 0x8000（高）

    GPIO_InitStructure.GPIO_Pin = BOMA_PORT6|BOMA_PORT7|BOMA_PORT8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = BOMA_PORT2|BOMA_PORT3|BOMA_PORT4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = BOMA_PORT5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // 配置机箱序号拨码开关的GPIO口
    GPIO_InitStructure.GPIO_Pin = BOMA_BOX2|BOMA_BOX3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BOMA_BOX1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BOMA_BOX4 | BOMA_BOX5|BOMA_BOX6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    // 光电池数量拨码开关的GPIO口
    GPIO_InitStructure.GPIO_Pin = BOMA_PD1|BOMA_PD2|BOMA_PD3|BOMA_PD4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC芯片地址位
    GPIO_InitStructure.GPIO_Pin = ADS1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ADS2 |ADS3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ADS4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // DS18B20单总线
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN3 | DS18B20_PIN4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //ADC芯片I2C
    GPIO_InitStructure.GPIO_Pin = ADC_SCL|ADC_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0X02;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2,&I2C_InitStructure);
    I2C_Cmd(I2C2,ENABLE);

    

    GPIO_SetBits(GPIOA, MCP_CS1);
    GPIO_SetBits(GPIOC, MCP_CS2);
    GPIO_SetBits(GPIOB, MCP_CS3 | MCP_CS4);

    //SPI
    //GPIOB初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;          // 推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        // 最高支持100MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                    // 应用配置

    // 初始化后置低电平
    GPIO_ResetBits(GPIOB, GPIO_Pin_9 | GPIO_Pin_8);         // PB9, PB8置低
    //BROKE定义
    GPIO_InitStructure.GPIO_Pin = BROKE1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        // 最高支持100MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);                    // 应用配置
    
    GPIO_InitStructure.GPIO_Pin = BROKE2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        // 最高支持100MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);                    // 应用配置
    
    GPIO_InitStructure.GPIO_Pin = BROKE3 | BROKE4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        // 最高支持100MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                    // 应用配置
    
}
