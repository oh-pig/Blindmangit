/**
 ****************************************************************************************************
 * @file        US_100.h
 * @author      ÀîÓêì³
 * @version     V1.0
 * @date        2022-06-21
 * @brief       US-100Ä£¿é
 * @license     
 ****************************************************************************************************
 * @attention
 ****************************************************************************************************
 */

#ifndef __US_100_H
#define __US_100_H
#include "main.h"

extern uint8_t US100;

extern uint32_t capture_cnt;
extern uint32_t capture_value1, capture_value2;
extern uint32_t pwm_cycle,hightime;
extern float distance;
void US100_run(void);

#endif
