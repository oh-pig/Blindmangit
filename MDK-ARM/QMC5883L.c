#include "usart.h"
#include "main.h"
#include "stdio.h"
#include "math.h"
#include "QMC5883L.h"



void hmc5883l_init(){
	
		uint8_t data = 0x70;
	
		HAL_I2C_Mem_Write(&hi2c2,HMC5883L_ADDER,CONFIGURATION_A, 1, &data, 1,1000); //配置寄存器A
		
		data = 0x20;
	
		HAL_I2C_Mem_Write(&hi2c2,HMC5883L_ADDER,CONFIGURATION_B, 1, &data, 1,1000); //配置寄存器B
	
		data = 0x0;
	
		HAL_I2C_Mem_Write(&hi2c2,HMC5883L_ADDER,HMC5883L_MODE, 1, &data, 1,1000); //配置模式寄存器，连续模式
	
}


void hmc5883l_rawread(float *GaX, float *GaY){
		
		uint8_t data[6];
		
	  HAL_I2C_Mem_Read(&hi2c2,HMC5883L_ADDER, 0x03,1,data,6 , 1000); //连续读取
	
		int16_t dxra,dyra;
	
		dxra = (data[0] << 8) | data[1]; 
		*GaX = (float)dxra /1090;
	
		dyra = (data[4] << 8) | data[5]; 
		*GaY = (float)dyra /1090 ;

		
/*	dzra = (data[2] << 8) | data[3];	
		float GaZ = (float)dzra /1090 ;		
	*/
	
}


void hmc5883l_selftest(float *Xoffest,float *Yoffest,float *Kx,float *Ky){
	
		uint8_t i=0 ;
	
		float GaX,GaY,GaXmax=0,GaXmin=0,GaYmax=0,GaYmin=0;
	
	printf("start");
		while(i != 100)
	{
		hmc5883l_rawread(&GaX, &GaY);
	
		GaXmax = GaXmax < GaX? GaX:GaXmax;
	
		GaXmin = GaXmin > GaX? GaX:GaXmin;
	
		GaYmax = GaYmax < GaY? GaY:GaYmax;
	
		GaYmin = GaYmin > GaY? GaY:GaYmin;
			
		HAL_Delay(200);
		
		i++;
		
				printf("...");
	}
		printf("end");
	*Xoffest = (GaXmax+GaXmin)/2;
	*Yoffest = (GaYmax+GaYmin)/2;
	*Kx = 2/(GaXmax-GaXmin);
	*Ky = 2/(GaXmax-GaXmin);
	
}

int16_t hmc5883l_read(float Xoffest,float Yoffest,float Kx,float Ky){
	
		float rawGaX,rawGaY;
	
		int16_t Magangle;
	
		hmc5883l_rawread(&rawGaX,&rawGaY);
	
		float GaX = (rawGaX - Xoffest) * Kx;
	
		float GaY = (rawGaY - Yoffest) * Ky;
			
			if((GaX > 0)&&(GaY > 0)) Magangle = atan(GaY/GaX)*57;
			else if((GaX > 0)&&(GaY < 0)) Magangle = 360+atan(GaY/GaX)*57;
			else if((GaX == 0)&&(GaY > 0)) Magangle = 90;
			else if((GaX == 0)&&(GaY < 0)) Magangle = 270;
			else if(GaX < 0) Magangle = 180+atan(GaY/GaX)*57;
			
			return Magangle;
		
}


static void MX_I2C2_InitQM(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}



























