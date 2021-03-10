// Created by indraneel on 1/3/21

#include <stdio.h>
#include <stdlib.h>
#include "soft_i2c.h"
#include <uart.h>
#include "plic_driver.h"


int main()
{
    printf("Entered main routine!\n");

    // Configure 1 millisecond Timer
    //uint64_t clint_module_freq = ( CLOCK_FREQUENCY / CLINT_DIVISOR );
    //uint64_t value = 0.001 * clint_module_freq;

    milliseconds = 0;
    printf("Configuring 1 millisecond timer interrupt\n");
	clint_init(MS_TIMER_FREQ);

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
    MasterSelectSlave(SLAVE_ADDRESS, DELAY_COUNT);

    // Check if Ack is received
    printf("\n\tI2C: I2C Write Ack\n");
    byte_it = 0;
    baudrate_print = 0;
    while(ReadSlaveAckForWrite(DELAY_COUNT))
    {
        // Prepare to write data from master to slave
        write_word(GPIO_DIRECTION_CNTRL_REG, (read_word(GPIO_DIRECTION_CNTRL_REG) & ~(I2C2_SDA)));
        printf("SDA2 set as input\n");

        write_word(GPIO_DIRECTION_CNTRL_REG, (read_word(GPIO_DIRECTION_CNTRL_REG) | (I2C1_SDA)));
        printf("SDA1 set as output\n");

        // Send last two digits of milliseconds counter as a byte from master to slave
        master_write_byte = (uint8_t)(milliseconds%100);

        i2c_cmd_type = I2C_Data_Cmd;
        I2cWriteByteinAdd(master_write_byte, DELAY_COUNT, I2C1_SDA, I2C1_SCL);
        byte_it++;
        if(baudrate_print == 1)
        {
            baudrate_print = 0;
            printf("\nCurrent baudrate : %llu \n",byte_it*8);
            byte_it = 0;
            printf("\n");
        }
    }

    printf("\n\tI2C: Slave is dead\n");

    // I2C stop condition

    printf("\n\tI2C: I2C Stop\n");
    write_word(GPIO_DIRECTION_CNTRL_REG, (read_word(GPIO_DIRECTION_CNTRL_REG) & ~(I2C2_SDA)));
    printf("SDA2 set as input\n");

    write_word(GPIO_DIRECTION_CNTRL_REG, (read_word(GPIO_DIRECTION_CNTRL_REG) | (I2C1_SDA)));
    printf("SDA1 set as output\n");
    
    i2c_cmd_type = I2C_Stop_Cmd;
	// scl = 1, sda = 0
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | I2C1_SCL & ~(I2C1_SDA) );
	soft_delay(200, 200);

	// sda = 1
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | (I2C1_SDA));
	soft_delay(200, 200);

	// scl = 0,sda = 0
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(I2C1_SCL)  & ~(I2C1_SDA) );
	soft_delay(200, 200);
    
   
    printf("Finished main routine!");
    return 0;
}