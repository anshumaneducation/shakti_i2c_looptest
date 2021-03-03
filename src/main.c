// Created by indraneel on 1/3/21

#include <stdio.h>
#include <stdlib.h>
#include "soft_i2c.h"
#include <uart.h>

#define SLAVE_ADDRESS 0x90

int main()
{
    printf("Entered main routine!\n");

    // Set up Master
    I2c1_Init();

    // Set up Slave
    I2c2_Init();

    // Master Check if Slave is alive
    MasterSelectSlave(SLAVE_ADDRESS);

    // Check if Ack is received
    if(ReadSlaveAckForWrite())
    {
        printf("\n\tI2C: I2C Write Ack\n");


    }
    else
    {
        printf("\n\tI2C: Slave is dead\n");
    }
   
    printf("Finished main routine!");
    return 0;
}