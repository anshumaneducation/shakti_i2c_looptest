// Created by indraneel on 1/3/21

#include <stdio.h>
#include <stdlib.h>
#include "soft_i2c.h"
#include <uart.h>
#include "plic_driver.h"


int main()
{
    printf("Entered main routine!\n");

    // Set up Master
    I2c1_Init();

    // Set up Slave
    I2c2_Init();
    i2c_slave_mode = I2C_Slave_Inactive;

    // Setup GPIO interrupt on slave SCL
    //init plic module
	plic_init();

    //configure interrupt id 7
	configure_interrupt(PLIC_INTERRUPT_9);

	//set the corresponding isr for interrupt id 7
	isr_table[PLIC_INTERRUPT_9] = SlaveClockHandler;

    enable_plic_interrupts();

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