// Created by Indraneel on 26/02/21

#include "soft_i2c.h"

/** @note : Both SDA1 and SDA2 should not be in output config at the same time*/

/** @fn I2c1_init
 *  @return void
 *  @arg None
 * 
 * */
void I2c1_Init()
{
	// Set SCL as output
	unsigned long readData = 0;
	readData = read_word(GPIO_DIRECTION_CNTRL_REG);
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData | I2C1_SCL));
	printf("SCL1 set as output\n");

	readData = read_word(GPIO_DIRECTION_CNTRL_REG);
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData | I2C1_SDA));
	printf("SDA1 set as output\n");

} /* I2c1_Init */

/** @fn I2c2_init
 *  @return void
 *  @arg None
 * 
 * */
void I2c2_Init()
{
	// Set SCL slave as input
	unsigned long readData = 0;
	readData = read_word(GPIO_DIRECTION_CNTRL_REG);
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData & ~(I2C2_SCL)));
	printf("SCL2 set as input\n");

	// Set SDA slave as input
	readData = read_word(GPIO_DIRECTION_CNTRL_REG);
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData & ~(I2C2_SDA)));
	printf("SDA2 set as input\n");

} /* I2c2_Init */

/** @fn MasterSelectSlave
 *  @return void
 *  @arg None
 * 
 * */
void MasterSelectSlave(unsigned char slave_address)
{

	printf("\n\tI2C: I2C Start\n");
	// I2C start condition
	i2c_cmd_type = I2C_Start_Cmd;
	// sda, scl = 1
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | I2C1_SCL | I2C1_SDA);
	soft_delay(200, 200);

	// sda = 0
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(I2C1_SDA));
	soft_delay(200, 200);

	// scl = 0
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(I2C1_SCL));
	soft_delay(200, 200);

	// Select slave node
	printf("\n\tI2C: I2C Write\n");
	i2c_cmd_type = I2C_Data_Cmd;
	I2cWriteByteinAdd((slave_address | I2C_WRITE), 200, I2C1_SDA, I2C1_SCL);

	// sda = 1
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | (I2C1_SDA));
	soft_delay(200, 200);

} /* MasterSelectSlave */

/** @fn soft_delay
 *  @return void
 *  @arg None
 * 
 * */
void soft_delay(unsigned int count1, unsigned int count2)
{

	for (int i = 0; i < count1; i++)
	{
		for (int j = 0; j < count2; j++)
		{
			;
		}
	}

} /* soft_delay */

/** @fn I2cWriteByte
 *  @return void
 *  @arg None
 * 
 * */
void I2cWriteByteinAdd(unsigned char byte, unsigned char delay, int sda_address, int scl_address)
{
	unsigned char j = 0;
	while (j < 8)
	{
		unsigned char k = byte;
		// MSB first
		k = (k << j) & 0x80;

		if (k == 0)
		{
			write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(sda_address));
			soft_delay(delay, delay);
		}
		else
		{
			write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | (sda_address));
			soft_delay(delay, delay);
		}

		// SCL clock
		// scl =1
		write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | (scl_address));
		soft_delay(delay, delay);
		// scl =0
		write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(scl_address));
		soft_delay(delay, delay);
		j++;
	}

} /* I2cWriteByte */

/** @fn ReadSlaveAckForWrite
 *  @return void
 *  @arg None
 * 
 * */
bool ReadSlaveAckForWrite()
{

	// Set SDA1 as input
	unsigned long readData = 0;
	readData = read_word(GPIO_DIRECTION_CNTRL_REG);
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData & ~(I2C1_SDA)));
	printf("SDA1 set as input\n");

	// Give Master clock
	//scl = 1
	write_word(GPIO_DATA_REG, (read_word(GPIO_DATA_REG) | I2C1_SCL));
	soft_delay(200, 200);

	// Read bus
	readData = read_word(GPIO_DATA_REG) & I2C1_SDA;

	//scl = 0
	write_word(GPIO_DATA_REG, (read_word(GPIO_DATA_REG) & ~(I2C1_SCL)));
	soft_delay(200, 200);

	if (readData == 0)
		return false;
	else
		return true;

} /* ReadSlaveAckForWrite */

/** @fn SlaveClockHandler
 *  @return void
 *  @arg None
 * 
 * */
void SlaveClockHandler(__attribute__((unused)) uint32_t num)
{

	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(I2C1_SCL));
	//soft_delay(200,200);

	switch (i2c_cmd_type)
	{
	case I2C_Start_Cmd:
	{
		printf("SLV: I2C start cmd received! \n");
		i2c_slave_mode = I2C_Slave_Address_Check;
		slave_bit_pos_iterator = 0;
		slave_read_byte = 0;
		break;
	}
	case I2C_Stop_Cmd:
	{
		printf("SLV: I2C stop cmd received! \n");
		i2c_slave_mode = I2C_Slave_Inactive;
		break;
	}
	case I2C_Data_Cmd:
	{
		printf("SLV: I2C data cmd received! \n");
		i2c_slave_monitor();
		break;
	}
	default:
		printf("SLV: Invalid command received \n");
	}

} /* SlaveClockHandler */

/** @fn i2c_slave_monitor
 *  @return void
 *  @arg None
 * 
 * */
void i2c_slave_monitor()
{
	// Local Variables
	unsigned long readData = 0;
	unsigned char bitValue;

	// Read bit and add to byte
	if (slave_bit_pos_iterator < 8)
	{
		// Ensure SDA2 is input
		readData = read_word(GPIO_DIRECTION_CNTRL_REG);
		write_word(GPIO_DIRECTION_CNTRL_REG, (readData & ~(I2C2_SDA)));

		readData = read_word(GPIO_DATA_REG) & I2C2_SDA;
		if (readData != 0)
		{
			bitValue = 1;
		}
		else
		{
			bitValue = 0;
		}
		if(slave_bit_pos_iterator != 0)
			slave_read_byte = slave_read_byte << 1;
		slave_read_byte = slave_read_byte | bitValue;
		slave_bit_pos_iterator++;
	}
	else
	{
		printf("SLV: Byte transfer complete, sending acknowledgement!\n");
		slave_bit_pos_iterator = 0;
		// Send Acknowledgement
		readData = read_word(GPIO_DIRECTION_CNTRL_REG);
		write_word(GPIO_DIRECTION_CNTRL_REG, (readData | I2C2_SDA));
		printf("SDA2 set as output\n");

		write_word(GPIO_DATA_REG, (read_word(GPIO_DATA_REG) | I2C2_SDA) );

		switch (i2c_slave_mode)
		{
		case I2C_Slave_Address_Check:
		{
			// Check read write mode
			unsigned char read_write_bit = slave_read_byte & 1;
			if (read_write_bit == I2C_WRITE)
			{
				i2c_slave_mode = I2C_Slave_Write;
			}
			else
			{
				i2c_slave_mode = I2C_Slave_Read;
			}

			// Check Slave address by masking LSB
			slave_read_byte = slave_read_byte & ~(1);

			if(slave_read_byte == SLAVE_ADDRESS)
			{
				printf("Slave address matched proceeding  0x%x! \n",slave_read_byte);
			}
			else
			{
				printf("Slave address mismatch  0x%x!\n",slave_read_byte);
				i2c_slave_mode =	I2C_Slave_Inactive;
			}
			break;
		}
		case I2C_Slave_Read:
		{
			printf("Received byte is 0x%x\n",slave_read_byte);
			break;
		}
		case I2C_Slave_Write:
		{

			break;
		}
		default:
			printf("SLV: Incorrect slave state!\n");
		}
	}

} /* i2c_slave_monitor */

/** @fn enable_plic_interrupts
 *  @return void
 *  @arg None
 * 
 * */
void enable_plic_interrupts()
{
	register unsigned int retval;

	// Enable Global (PLIC) interrupts.
	asm volatile("li      t0, 8\t\n"
				 "csrrs   zero, mstatus, t0\t\n");

	// Enable Local (PLIC) interrupts.
	asm volatile("li      t0, 0x800\t\n"
				 "csrrs   zero, mie, t0\t\n");

	asm volatile(
		"csrr %[retval], mstatus\n"
		:
		[retval] "=r"(retval));

	log_debug("mstatus = %x\n", retval);

	asm volatile(
		"csrr %[retval], mie\n"
		:
		[retval] "=r"(retval));

	log_debug("mie = %x\n", retval);

	asm volatile(
		"csrr %[retval], mip\n"
		:
		[retval] "=r"(retval));

	log_debug("mip = %u\n", retval);

} /* enable_plic_interrupts */


/** @fn void custom_clint_handler(uintptr_t int_id, uintptr_t epc)
 * @brief handler for machine timer interrupt
 * @details handler for machine timer interrupt. This handles the timer interrupt and sets mtimecmp to clear timer interrupt.
 * @param unsigned int ptr int_id
 * @param unsigned int ptr epc
 */
void custom_clint_handler( __attribute__((unused)) uintptr_t int_id,  __attribute__((unused)) uintptr_t epc)
{
	printf("\n custom_clint_handler entered\n");

	//set mtimecmp to some value. On appln reqt basis handle timer interrupt
	*mtimecmp = -1;

	log_info("Timer interrupt handled \n");

	printf("custom_clint_handler exited\n");
}

void clint_init()
{
	asm volatile("li      t0, 0x80\t\n"
		     "csrrs   zero, mie, t0\t\n"
		    );

	asm volatile("li      t0, 8\t\n"
		     "csrrs   zero, mstatus, t0\t\n"
		    );

	
	mcause_interrupt_table[MACH_TIMER_INTERRUPT]     = custom_clint_handler;

	printf("Assigned custom_clint_handler to trap id : %d\n", MACH_TIMER_INTERRUPT);
}
