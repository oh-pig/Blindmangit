#include "i2c.h"
#include "stdbool.h"

/* ATK-MS6050的IIC通讯从机地址
 * 如果ATK-MS6050的AD0引脚被拉低，则其IIC通讯的地址为0x68
 * 如果ATK-MS6050的AD0引脚被拉高，则其IIC通讯的地址为0x69
 */
 
#ifndef __QMC5883L_H
#define __QMC5883L_H

#define HMC5883L_ADDER 0x3c
#define CONFIGURATION_A  0x00
#define CONFIGURATION_B  0x01
#define HMC5883L_MODE  0x02
#define X_MSB 0x03



#endif


