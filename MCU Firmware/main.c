#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_i2c.h"
#include "stm32f103rbt6.h"
#include "HMC5883L.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_uart(){
	//USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);
}
void init_i2c(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitTypeDef  I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 50000;
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
}
void init_hmc5883l(){
	//bool v = HMC5883L_TestConnection();
	uint8_t tmp = (HMC5883L_AVERAGING_8 << (HMC5883L_CRA_AVERAGE_BIT - HMC5883L_CRA_AVERAGE_LENGTH + 1))
			| (HMC5883L_RATE_15 << (HMC5883L_CRA_RATE_BIT - HMC5883L_CRA_RATE_LENGTH + 1))
			| (HMC5883L_BIAS_NORMAL << (HMC5883L_CRA_BIAS_BIT - HMC5883L_CRA_BIAS_LENGTH + 1));
	HMC5883L_I2C_ByteWrite(HMC5883L_DEFAULT_ADDRESS, &tmp, HMC5883L_RA_CONFIG_A);
	HMC5883L_SetGain(HMC5883L_GAIN_1090);
	HMC5883L_SetMode(HMC5883L_MODE_SINGLE);
}
void print(uint8_t *array, uint16_t length){
	uint16_t i = 0;
	while (i != length){
		USART_SendData(USART1, array[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
		i++;
	}
}

int16_t coordinates[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char buffer[40];
void print_data(){
	//Compass
	HMC5883L_SetMode(HMC5883L_MODE_SINGLE);
	while(HMC5883L_GetReadyStatus()!= true){}
	HMC5883L_GetHeading(coordinates);

	//Print
	print("S ", 2);

	sprintf(buffer, "X=%.4+d ",coordinates[0]);
	print(buffer, 8);
	sprintf(buffer, "Z=%.4+d ",coordinates[1]);
	print(buffer, 8);
	sprintf(buffer, "Y=%.4+d ",coordinates[2]);
	print(buffer, 8);

	print("E\n", 2);
}
void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		if (USART_ReceiveData(USART1)=='R'){
			print_data();
		}
	}
}

void main(){
	init_uart();
	init_i2c();
	init_hmc5883l();
	while(1){}
}
