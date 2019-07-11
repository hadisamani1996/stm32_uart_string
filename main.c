#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"

#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)   (sizeof(a) / sizeof(*(a)))

uint8_t TxBuffer1[] = "this example is for testing DMA , send and receive";
uint8_t RxBuffer1[TxBufferSize1];
uint8_t NbrOfDataToRead = TxBufferSize1;
void	RCC_Configuration( void );
void	GPIO_Configuration( void );
void	DMA_Configuration( void );  
void	USART_Configuration( void );
void uartStartTxDMA( DMA_Channel_TypeDef *txDMAChannel );
int main() {
	RCC_Configuration();
	GPIO_Configuration();
	DMA_Configuration();  
	USART_Configuration();
	while(1)
	{
		 int i=0;
		 while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET)
		 {
		 }

		// Wait until USARTy RX DMA1 Channel Transfer Complete 
		while (DMA_GetFlagStatus(DMA1_FLAG_TC5) == RESET)
		{
		}

		// TxBufferSize1 Sends the same string again after receiving serial input for the length. 
		// Then, wait for the serial input again.
			
		for (i=0; i < TxBufferSize1; ++i)
		{
			TxBuffer1[ i] = RxBuffer1[ i];
		}

			
			DMA_ClearITPendingBit(DMA1_IT_TC4);
			DMA_ClearITPendingBit(DMA1_IT_TC5);
			DMA_Cmd(DMA1_Channel4, DISABLE);
			
			uartStartTxDMA(DMA1_Channel4);
	}
}
void RCC_Configuration() 
{
  // DMA clock enable
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  // Enable GPIO clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
}
void GPIO_Configuration() 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  /* Configure USART1 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  /* Configure USART1 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void DMA_Configuration(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  // USARTy_Tx_DMA_Channel (triggered by USARTy Tx event) Config
  DMA_DeInit(DMA1_Channel4);	
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer1;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = TxBufferSize1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  
  // USARTy_Rx_DMA_Channel (triggered by USARTy Rx event) Config
  DMA_DeInit(DMA1_Channel5);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer1;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = TxBufferSize1;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
}
void USART_Configuration() {
	
	USART_InitTypeDef  USART_InitStructure ;
  USART_InitStructure . USART_BaudRate  =  9600 ;
  USART_InitStructure . USART_WordLength  =  USART_WordLength_8b ;
  USART_InitStructure . USART_StopBits  =  USART_StopBits_1 ;
  USART_InitStructure . USART_Parity  =  USART_Parity_No ;
  USART_InitStructure . USART_HardwareFlowControl  =  USART_HardwareFlowControl_None ;
  USART_InitStructure .USART_Mode  =  USART_Mode_Rx  |  USART_Mode_Tx ;	
	
  USART_Init(USART1, &USART_InitStructure);
	
  // Enable USART1 DMA TX request
  USART_DMACmd(USART1, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);

  // Enable USART1
  USART_Cmd(USART1, ENABLE);

  // Enable USART1 DMA TX, RX Channel
  DMA_Cmd(DMA1_Channel4, ENABLE);
  DMA_Cmd(DMA1_Channel5, ENABLE);
}
/**
 *DMA_Channel_TypeDef is a struct on stm32f10x.h 
**/
void uartStartTxDMA(DMA_Channel_TypeDef *txDMAChannel)
{
		txDMAChannel->CMAR = (uint32_t)TxBuffer1;
		txDMAChannel->CNDTR = TxBufferSize1;
		DMA_Cmd(txDMAChannel, ENABLE);
}
