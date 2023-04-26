#include "demo.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "usart.h"
#include "main.h"


static void demo_usart1_niming_report(uint8_t fun, uint8_t *dat, uint8_t len)
{
    uint8_t send_buf[32];
    uint8_t i;
    
    if (len > 28)
    {
        return;
    }
    
    send_buf[len+4] = 0;            /* 校验位清零 */
    send_buf[0] = 0xAA;             /* 帧头为0xAAAA */
    send_buf[1] = 0xAA;             /* 帧头为0xAAAA */
    send_buf[2] = fun;              /* 功能字 */
    send_buf[3] = len;              /* 数据长度 */
    for (i=0; i<len; i++)           /* 复制数据 */
    {
        send_buf[4 + i] = dat[i];
    }
    for (i=0; i<(len + 4); i++)     /* 计算校验和 */
    {
        send_buf[len + 4] += send_buf[i];
    }
    
    /* 发送数据 */
    HAL_UART_Transmit(&huart1, send_buf, len + 5, HAL_MAX_DELAY);
}


static void demo_niming_report_status(int16_t rol, int16_t pit, int16_t yaw, uint32_t alt, uint8_t fly_mode, uint8_t armed)
{
    uint8_t send_buf[12];
    
    /* 横滚角 */
    send_buf[0] = (rol >> 8) & 0xFF;
    send_buf[1] = rol & 0xFF;
    /* 俯仰角 */
    send_buf[2] = (pit >> 8) & 0xFF;
    send_buf[3] = pit & 0xFF;
    /* 航向角 */
    send_buf[4] = (yaw >> 8) & 0xFF;
    send_buf[5] = yaw & 0xFF;
    /* 飞行高度 */
    send_buf[6] = (alt >> 24) & 0xFF;
    send_buf[7] = (alt >> 16) & 0xFF;
    send_buf[8] = (alt >> 8) & 0xFF;
    send_buf[9] = alt & 0xFF;
    /* 飞行模式 */
    send_buf[10] = fly_mode;
    /* 锁定状态 */
    send_buf[11] = armed;
    
    /* 状态帧的功能字为0x01 */
    demo_usart1_niming_report(0x01, send_buf, 12);
}


static void demo_niming_report_senser(  int16_t  acc_x, int16_t  acc_y, int16_t  acc_z,
                                        int16_t gyro_x, int16_t gyro_y, int16_t gyro_z,
                                        int16_t  mag_x, int16_t  mag_y, int16_t  mag_z)
{
    uint8_t send_buf[18];
    

    send_buf[0] = (acc_x >> 8) & 0xFF;
    send_buf[1] = acc_x & 0xFF;

    send_buf[2] = (acc_y >> 8) & 0xFF;
    send_buf[3] = acc_y & 0xFF;

    send_buf[4] = (acc_z >> 8) & 0xFF;
    send_buf[5] = acc_z & 0xFF;

    send_buf[6] = (gyro_x >> 8) & 0xFF;
    send_buf[7] = gyro_x & 0xFF;

    send_buf[8] = (gyro_y >> 8) & 0xFF;
    send_buf[9] = gyro_y & 0xFF;

    send_buf[10] = (gyro_z >> 8) & 0xFF;
    send_buf[11] = gyro_z & 0xFF;

    send_buf[12] = (mag_x >> 8) & 0xFF;
    send_buf[13] = mag_x & 0xFF;

    send_buf[14] = (mag_y >> 8) & 0xFF;
    send_buf[15] = mag_y & 0xFF;

    send_buf[16] = (mag_z >> 8) & 0xFF;
    send_buf[17] = mag_z & 0xFF;
    
    demo_usart1_niming_report(0x02, send_buf, 18);
}



void demo_run(void)
{
    uint8_t ret;
    uint8_t key;
    uint8_t niming_report = 0;
    float pit, rol, yaw;
    int16_t acc_x, acc_y, acc_z;
    int16_t gyr_x, gyr_y, gyr_z;
    int16_t temp;
    
    ret = Mpu6050_init();
    if (ret == 1)
    {
        printf("ATK-MS6050 init failed!\r\n");
    }
    
    ret = atk_ms6050_dmp_init();
    if (ret == 1)
    {
        printf("ATK-MS6050 DMP init failed!\r\n");
			while(1);
    }
    
    
    while (1)
    {
        ret  = atk_ms6050_dmp_get_data(&pit, &rol, &yaw);
		
        ret += Mpu6050_get_accelerometer(&acc_x, &acc_y, &acc_z);

        ret += Mpu6050_get_gyroscope(&gyr_x, &gyr_y, &gyr_z);

        ret += Mpu6050_get_temperature(&temp);
        if (ret != 0)
        {
            if (niming_report == 0)
            {
                printf("pit: %.2f, rol: %.2f, yaw: %.2f, ", pit, rol, yaw);
                printf("acc_x: %d, acc_y: %d, acc_z: %d, ", acc_x, acc_y, acc_z);
                printf("gyr_x: %d, gyr_y: %d, gyr_z: %d, ", gyr_x, gyr_y, gyr_z);
                printf("temp: %d\r\n", temp);
            }
            else
            {
                demo_niming_report_status((int16_t)(rol * 100), (int16_t)((pit) * 100), (int16_t)(yaw * 100), 0, 0, 0);
                demo_niming_report_senser(acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, 0, 0, 0);
            }
        }
		//HAL_Delay(10);
    }
}
