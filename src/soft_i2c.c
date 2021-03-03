// Created by Indraneel on 26/02/21

#include "soft_i2c.h"


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
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData | I2C1_SCL) );
	printf("SCL1 set as output\n");


	readData = read_word(GPIO_DIRECTION_CNTRL_REG);
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData | I2C1_SDA) );
	printf("SDA1 set as output\n");

} /* I2c1_Init */


/** @fn I2c2_init
 *  @return void
 *  @arg None
 * 
 * */
void I2c2_Init()
{
     // Set SCL as output
	/*
    unsigned long readData = 0;
	readData = read_word(GPIO_DIRECTION_CNTRL_REG);
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData | I2C2_SCL) );
	printf("SCL2 set as output\n");

	*/


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

	// sda, scl = 1
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | I2C1_SCL | I2C1_SDA );
	soft_delay(200,200);

	// sda = 0
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(I2C1_SDA) );
	soft_delay(200,200);

	// scl = 0
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(I2C1_SCL) );
	soft_delay(200,200);

	// Select slave node
	printf("\n\tI2C: I2C Write\n");
	I2cWriteByteinAdd((slave_address | I2C_WRITE), 200,I2C1_SDA,I2C2_SCL);

	// sda = 1
	write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | (I2C1_SDA) );
	soft_delay(200,200);


} /* MasterSelectSlave */


/** @fn soft_delay
 *  @return void
 *  @arg None
 * 
 * */
void soft_delay(unsigned int count1, unsigned int count2)
{

	for(int i = 0;i<count1;i++)
	{
		for(int j = 0;j<count2;j++)
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
void I2cWriteByteinAdd(unsigned char byte, unsigned char delay, int sda_address ,int scl_address)
{
	unsigned char j= 0;
	while(j<8)
	{
		unsigned char k = byte;
		// MSB first 
		k = (k << j) & 0x80;

		if(k == 0)
		{
			write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(sda_address) );
			soft_delay(delay,delay);
		}
		else
		{
			write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | (sda_address) );
			soft_delay(delay,delay);
		}

		// SCL clock
		// scl =1
		write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) | (scl_address) );
		soft_delay(delay,delay);
		// scl =0
		write_word(GPIO_DATA_REG, read_word(GPIO_DATA_REG) & ~(scl_address) );
		soft_delay(delay,delay);
		j++;
	}



} /* I2cWriteByte */


/** @fn ReadSlaveAckForWrite
 *  @return void
 *  @arg None
 * 
 * */
bool ReadSlaveAckForWrite(){

	// Set SDA1 as input
	unsigned long readData = 0;
	readData = read_word(GPIO_DIRECTION_CNTRL_REG);
	write_word(GPIO_DIRECTION_CNTRL_REG, (readData & ~(I2C1_SDA) ));
	printf("SDA1 set as input\n");

	// Give Master clock
	//scl = 1
	write_word(GPIO_DATA_REG, (read_word(GPIO_DATA_REG) | I2C1_SCL) );
	soft_delay(200,200);

	// Read bus
	readData = read_word(GPIO_DATA_REG)  & I2C1_SDA;

	//scl = 0
	write_word(GPIO_DATA_REG, (read_word(GPIO_DATA_REG) & ~(I2C1_SCL) ));
	soft_delay(200,200);

	if(readData == 0)
		return false;
	else
		return false;

} /* ReadSlaveAckForWrite */