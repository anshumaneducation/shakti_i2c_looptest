// Created by Indraneel on 26/02/2021

#ifndef SOFT_I2C_H
#define SOFT_I2C_H


#include "platform.h"
#include "gpio.h"
#include "gpio_i2c.h"
#include <stdbool.h>
#include <stdio.h>

/*** 
 * 
 *  ////////////////////////    LOCAL DEFINITIONS  //////////////////////////////////////////////
 * 
 * */

#define I2C1_SCL GPIO0 
#define I2C1_SDA GPIO1

#define I2C2_SCL GPIO2 
#define I2C2_SDA GPIO3

#define GPIOD_IS_IN 0
#define GPIOD_IS_OUT 1

#define DELAY_COUNT 200

#define I2C_WRITE 0
#define I2C_READ 1

/*** 
 * 
 *  ////////////////////////    EXTERN DEFINITIONS  //////////////////////////////////////////////
 * 
 * */


/*** 
 * 
 *  ////////////////////////    FUNCTION PROTOTYPES  //////////////////////////////////////////////
 * 
 * */

void I2c1_Init();
void I2c2_Init();

void MasterSelectSlave(unsigned char slave_address);

void soft_delay(unsigned int count1, unsigned int count2);
void I2cWriteByteinAdd(unsigned char byte, unsigned char delay, int sda_address ,int scl_address);
bool ReadSlaveAckForWrite();

#endif