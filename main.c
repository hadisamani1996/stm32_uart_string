#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
static int i = 0;
_Bool	show = 0;
char uart_rec[ 100 ];
void show_rec( void );
void flush( void );
void send_str(char array[]);
void usart_init( void );
void nvic_init( void );
int main() {
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOF |
													RCC_APB2Periph_GPIOA 	| RCC_APB2Periph_AFIO , ENABLE );
	nvic_init();
	usart_init();
	while( 1 )  {
		show_rec();
	}
		  
}
void usart_init( void ) {
	GPIO_InitTypeDef 	GPIO_InitStruct;
	GPIO_InitTypeDef	GPIO_InitUSART;
	USART_InitTypeDef	USART_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA , &GPIO_InitStruct );
	
	GPIO_InitUSART.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitUSART.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitUSART.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA , &GPIO_InitUSART );
	
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init( USART1 , &USART_InitStruct );
	USART_ITConfig( USART1 , USART_IT_RXNE , ENABLE );
	USART_Cmd( USART1 , ENABLE );
}
void nvic_init( ) {
	NVIC_InitTypeDef	NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init( &NVIC_InitStruct );
}
void flush() {
	int i;
	for( i = 0 ; uart_rec[ i ] ; i ++ );
}
void show_rec() {
	if( show != 0 ) {
		for( int j = 0 ; uart_rec[ j ] ; j ++ )  {
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET){};
			USART_SendData( USART1 , uart_rec[ j ] );
		}
		show = 0;
		flush();
	}
}
void USART1_IRQHandler( void ) {
	if( USART_ReceiveData( USART1 ) != '\n' ) {
		uart_rec[ i ] = USART_ReceiveData( USART1 );
		i ++;
	}
	else if( USART_ReceiveData( USART1 ) == '\n' )  {
		i = 0;
		show = 1;
	}
}