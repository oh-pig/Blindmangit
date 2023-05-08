/**
 ****************************************************************************************************
 * @file        demo.h
 * @author      ÀîÓêì³
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS6050Ä£¿é
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
extern atk_mo1218_position_t position;
void GPS_init(void);
uint8_t GPS_update(void);



#endif



