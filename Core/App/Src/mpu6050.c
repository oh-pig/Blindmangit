#include "mpu6050.h"


static void atk_ms6050_hw_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* 使能AD0引脚GPIO的时钟 */
    ATK_MS6050_AD0_GPIO_CLK_ENABLE();
    
    /* 初始化AD0引脚 */
    gpio_init_struct.Pin    = ATK_MS6050_AD0_GPIO_PIN;  /* AD0引脚 */
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;      /* 推挽输出 */
    gpio_init_struct.Pull   = GPIO_PULLUP;              /* 上拉 */
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;     /* 高速 */
    HAL_GPIO_Init(ATK_MS6050_AD0_GPIO_PORT, &gpio_init_struct);
    
    /* 控制ATK-MS6050的AD0引脚为低电平
     * 设置其IIC的从机地址为0x68
     */
    ATK_MS6050_AD0(0);
}

/**
 * @brief       往ATK-MS6050的指定寄存器连续写入指定数据
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              len : 写入的长度
 *              dat : 写入的数据
 * @retval      0 : 函数执行成功
 *              1 : IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *dat)
{
	uint8_t state = HAL_I2C_Mem_Write(&hi2c1,((addr<<1)|0),reg,I2C_MEMADD_SIZE_8BIT,dat,len,1000);
	if(state == HAL_ERROR)
	{
		//printf("Error:\nfilename:%s\nline num:%d\n", __FILE__ , __LINE__);
		printf("I2c Mem Write ERROR!\n");
		return 1;
	}
	else if(state == HAL_BUSY)
	{
		printf("I2c Mem Write BUSY!\n");
		return 1;
	}
	else if(state == HAL_TIMEOUT)
	{
		printf("I2c Mem Write TIMEOUT!\n");
		return 1;
	}
	return 0;
}


/**
 * @brief       往ATK-MS6050的指定寄存器写入一字节数据
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              dat : 写入的数据
 * @retval      0 : 函数执行成功
 *              1 : IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_write_byte(uint8_t addr, uint8_t reg, uint8_t dat)
{
	return Mpu6050_write(addr,reg,1,&dat);
}


/**
 * @brief       连续读取ATK-MS6050指定寄存器的值
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              len: 读取的长度
 *              dat: 存放读取到的数据的地址
 * @retval      1 : 函数执行成功
 *              0 : IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *dat)
{
	uint8_t state = HAL_I2C_Mem_Read(&hi2c1,((addr<<1)|1),reg,I2C_MEMADD_SIZE_8BIT,dat,len,1000);
	if(state == HAL_ERROR)
	{
		//printf("Error:\nfilename:%s\nline num:%d\n", __FILE__ , __LINE__);
		printf("I2c Mem Write ERROR!\n");
		return 1;
	}
	else if(state == HAL_BUSY)
	{
		printf("I2c Mem Write BUSY!\n");
		return 1;
	}
	else if(state == HAL_TIMEOUT)
	{
		printf("I2c Mem Write TIMEOUT!\n");
		return 1;
	}
	return 0;
}


/**
 * @brief       读取ATK-MS6050指定寄存器的值
 * @param       addr: ATK-MS6050的IIC通讯地址
 *              reg : ATK-MS6050寄存器地址
 *              dat: 读取到的寄存器的值
 * @retval      0 : 函数执行成功
 *              1 : IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_read_byte(uint8_t addr, uint8_t reg, uint8_t *dat)
{
	return Mpu6050_read(addr,reg,1,dat);
}


/**
 * @brief       ATK-MS6050软件复位
 * @param       无
 * @retval      无
 */
void Mpu6050_reset(void)
{
	Mpu6050_write_byte(MPU6050_IIC_ADDR,MPU_PWR_MGMT1_REG,0x80);
	HAL_Delay(100);
	Mpu6050_write_byte(MPU6050_IIC_ADDR,MPU_PWR_MGMT1_REG,0x00);
}


/**
 * @brief       ATK-MS6050设置陀螺仪传感器量程范围
 * @param       frs: 0 --> ±250dps
 *                   1 --> ±500dps
 *                   2 --> ±1000dps
 *                   3 --> ±2000dps
 * @retval      0 : 函数执行成功
 *              1 : IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_set_gyro_fsr(uint8_t fsr)
{
	return Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_GYRO_CFG_REG, fsr << 3);
}


/**
 * @brief       ATK-MS6050设置加速度传感器量程范围
 * @param       frs: 0 --> ±2g
 *                   1 --> ±4g
 *                   2 --> ±8g
 *                   3 --> ±16g
 * @retval      0 : 函数执行成功
 *              1 : IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_set_accel_fsr(uint8_t fsr)
{
	return Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_ACCEL_CFG_REG, fsr << 3);
}


/**
 * @brief       ATK-MS6050设置数字低通滤波器频率
 * @param       lpf: 数字低通滤波器的频率（Hz）
 * @retval      0 : 函数执行成功
 *              1 : IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_set_lpf(uint16_t lpf)
{
    uint8_t dat;
    
    if (lpf >= 188)
    {
        dat = 1;
    }
    else if (lpf >= 98)
    {
        dat = 2;
    }
    else if (lpf >= 42)
    {
        dat = 3;
    }
    else if (lpf >= 20)
    {
        dat = 4;
    }
    else if (lpf >= 10)
    {
        dat = 5;
    }
    else
    {
        dat = 6;
    }
    
    return Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_CFG_REG, dat);
}


/**
 * @brief       ATK-MS6050设置采样率
 * @param       rate: 采样率（4~1000Hz）
 * @retval      MPU6050_EOK : 函数执行成功
 *              MPU6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_set_rate(uint16_t rate)
{
	uint8_t ret;
    uint8_t dat;
    
    if (rate > 1000)
    {
        rate = 1000;
    }
    
    if (rate < 4)
    {
        rate = 4;
    }
    
    dat = 1000 / rate - 1;
    ret = Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_SAMPLE_RATE_REG, dat);
    if (ret != 1)
    {
        return ret;
    }
    
    ret = Mpu6050_set_lpf(rate >> 1);
    if (ret != 1)
    {
        return ret;
    }
    
    return 1;
}


/**
 * @brief       ATK-MS6050获取温度值
 * @param       temperature: 获取到的温度值（扩大了100倍）
 * @retval      0 : 函数执行成功
 *              1 : IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_get_temperature(int16_t *temp)
{
	uint8_t dat[2];
    uint8_t ret;
    int16_t raw = 0;
    
    ret = Mpu6050_read(MPU6050_IIC_ADDR, MPU_TEMP_OUTH_REG, 2, dat);
    if (ret == 0)
    {
        raw = ((uint16_t)dat[0] << 8) | dat[1];
        *temp = (int16_t)((36.53f + ((float)raw / 340)) * 100);
    }
    
    return ret;
}


/**
 * @brief       ATK-MS6050获取陀螺仪值
 * @param       gx，gy，gz: 陀螺仪x、y、z轴的原始度数（带符号）
 * @retval      MPU6050_EOK : 函数执行成功
 *              MPU6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_get_gyroscope(int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t dat[6];
    uint8_t ret;
    
    ret =  Mpu6050_read(MPU6050_IIC_ADDR, MPU_GYRO_XOUTH_REG, 6, dat);
    if (ret == 0)
    {
        *gx = ((uint16_t)dat[0] << 8) | dat[1];
        *gy = ((uint16_t)dat[2] << 8) | dat[3];
        *gz = ((uint16_t)dat[4] << 8) | dat[5];
    }
    
    return ret;
}



/**
 * @brief       ATK-MS6050获取加速度值
 * @param       ax，ay，az: 加速度x、y、z轴的原始度数（带符号）
 * @retval      MPU6050_EOK : 函数执行成功
 *              MPU6050_EACK: IIC通讯ACK错误，函数执行失败
 */
uint8_t Mpu6050_get_accelerometer(int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t dat[6];
    uint8_t ret;
    
    ret =  Mpu6050_read(MPU6050_IIC_ADDR, MPU_ACCEL_XOUTH_REG, 6, dat);
    if (ret == 0)
    {
        *ax = ((uint16_t)dat[0] << 8) | dat[1];
        *ay = ((uint16_t)dat[2] << 8) | dat[3];
        *az = ((uint16_t)dat[4] << 8) | dat[5];
    }
    
    return ret;
}


/**
 * @brief       ATK-MS6050初始化
 * @param       无
 * @retval      MPU6050_EOK: 函数执行成功
 *              MPU6050_EID: 获取ID错误，函数执行失败
 */
uint8_t Mpu6050_init(void)
{
    uint8_t id = 0;
   // atk_ms6050_hw_init();
    Mpu6050_reset();                                                  	 /* ATK-MS050软件复位 */
    Mpu6050_set_gyro_fsr(3);                                             /* 陀螺仪传感器，±2000dps */
    Mpu6050_set_accel_fsr(0);                                            /* 加速度传感器，±2g */
    Mpu6050_set_rate(50);                                                /* 采样率，50Hz */
    Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_INT_EN_REG, 0X00);      	 /* 关闭所有中断 */
    Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_USER_CTRL_REG, 0X00);   	 /* 关闭IIC主模式 */
    Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_FIFO_EN_REG, 0X00);     	 /* 打开  关闭FIFO */
    Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_INTBP_CFG_REG, 0X80);    	 /* INT引脚低电平有效 */
    Mpu6050_read_byte(MPU6050_IIC_ADDR, MPU_DEVICE_ID_REG, &id);      	 /* 读取设备ID */
    if (id != MPU6050_IIC_ADDR)
    {
        return 1;
    }
    Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_PWR_MGMT1_REG, 0x01);    	 /* 设置CLKSEL，PLL X轴为参考 */
    Mpu6050_write_byte(MPU6050_IIC_ADDR, MPU_PWR_MGMT2_REG, 0x00);   	 /* 加速度与陀螺仪都工作 */
    Mpu6050_set_rate(50);                                                /* 采样率，50Hz */
    
    return 0;
}


