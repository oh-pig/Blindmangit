#include "US_100.h"
#include "inv_mpu.h"
#include "usart.h"
#include "mpu6050.h"
#include "tim.h"
/* Private variables ---------------------------------------------------------*/

//TIM_HandleTypeDef htim3;
//TIM_HandleTypeDef htim4;

uint8_t US100=0;

uint32_t capture_cnt = 0;
uint32_t capture_value1, capture_value2;
uint32_t pwm_cycle,hightime;
float distance = 0;

void  US100_run()
{
	switch(capture_cnt)
		{
			case 0:
				capture_cnt++; 
			  //����TIM3ͨ��1����������
			  __HAL_TIM_SET_CAPTUREPOLARITY( &htim3 , TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		    //����TIM3�����ж�ģʽ
			  HAL_TIM_IC_Start_IT( &htim3 , TIM_CHANNEL_1 );
				break;
			case 3:
				//����ߵ�ƽ����ֵ�T(TIM3������Ƶ��1MHz)
				if(capture_value2 >= capture_value1)
				hightime = capture_value2 - capture_value1;
			  if(capture_value2 < capture_value1)
				{
					hightime = capture_value2 + (0xffff- capture_value1);
				}
				//���ݹ�ʽ������� hightime/1000000.0Ϊ�ߵ�ƽʱ��
				distance = hightime/10000.0*340/2.0;
				//	UartPutNum(distance);
//				HAL_UART_Transmit_IT(&huart2,(uint8_t *)TxDistance,sizeof(TxDistance));
//				HAL_UART_Transmit_IT(&huart2,(uint8_t *)TxDistance,sizeof(TxDistance));
				printf("%.3f\r\n",distance);
				capture_cnt = 0;
				break;
		}
		if(US100==1){
		HAL_GPIO_WritePin( GPIOA, GPIO_PIN_12,GPIO_PIN_SET);
  	HAL_Delay(1);
  	HAL_GPIO_WritePin( GPIOA, GPIO_PIN_12,GPIO_PIN_RESET);
		US100=0;
		}
		
}
