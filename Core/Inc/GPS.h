/**
 ****************************************************************************************************
 * @file        demo.h
 * @author      �����
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS6050ģ��
 * @license     
 ****************************************************************************************************
 * @attention
 ****************************************************************************************************
 */
 
#ifndef __GPS_H
#define __GPS_H
#include "main.h"
#include "atk_mo1218.h"
#include <stdlib.h>

void GPS_init(void);
uint8_t GPS_update(atk_mo1218_position_t position);



#endif



