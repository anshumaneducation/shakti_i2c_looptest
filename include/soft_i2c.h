// Created by Indraneel on 26/02/2021

#ifndef SOFT_I2C_H
#define SOFT_I2C_H


#include "platform.h"
#include "gpio.h"
#include "gpio_i2c.h"
#include <stdbool.h>
#include <stdio.h>

#include "traps.h"
#include "clint_driver.h"
#include "log.h"


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

#define SLAVE_ADDRESS 0x90

#define CLINT_DIVISOR   256

/*** 
 * 
 *  ////////////////////////    ENUM and STRUCT DEFINITIONS  //////////////////////////////////////////////
 * 
 * */

/** This enum is required since both master and slave are sim on the same side so SDA transitions are not possible*/
typedef enum I2C_Command_type
{
    I2C_Data_Cmd,
    I2C_Start_Cmd,
    I2C_Stop_Cmd
} I2C_Command_type_e;

typedef enum I2C_Slave_Mode
{
    I2C_Slave_Inactive,
    I2C_Slave_Address_Check,
    I2C_Slave_Read,
    I2C_Slave_Write
} I2C_Slave_Mode_e;

/*** 
 * 
 *  ////////////////////////    EXTERN DEFINITIONS  //////////////////////////////////////////////
 * 
 * */

I2C_Command_type_e i2c_cmd_type;
I2C_Slave_Mode_e i2c_slave_mode;
uint8_t slave_bit_pos_iterator;
unsigned char slave_read_byte;

/*** 
 * 
 *  ////////////////////////    FUNCTION PROTOTYPES  //////////////////////////////////////////////
 * 
 * */

void I2c1_Init();
void I2c2_Init();
void clint_Init();
void enable_plic_interrupts();

void custom_clint_handler( __attribute__((unused)) uintptr_t int_id,  __attribute__((unused)) uintptr_t epc);
void SlaveClockHandler(__attribute__((unused)) uint32_t num);
void i2c_slave_monitor();

void MasterSelectSlave(unsigned char slave_address);

void soft_delay(unsigned int count1, unsigned int count2);
void I2cWriteByteinAdd(unsigned char byte, unsigned char delay, int sda_address ,int scl_address);
bool ReadSlaveAckForWrite();

#endif