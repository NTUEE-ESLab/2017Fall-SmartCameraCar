#include <iostream>
#include <cstring>
#include <unistd.h>                             //Needed for I2C port
#include <fcntl.h>                              //Needed for I2C port
#include <sys/ioctl.h>                  //Needed for I2C port
#include <linux/i2c-dev.h>              //Needed for I2C port

int main ()
{
	int file_i2c;


	//----- OPEN THE I2C BUS -----
	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
			//ERROR HANDLING: you can check errno to see what went wrong
			std::cout << "Failed to open the i2c bus";
			//return 0;
	}

	int addr = 0x04;          //<<<<<The I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
			//ERROR HANDLING; you can check errno to see what went wrong
			std::cout << "Failed to acquire bus access and/or talk to slave..\n";
			//return 0;
	}

	//----- WRITE BYTES -----
	//while (true)
	//{
			int a = 1, b = 2;
			unsigned char buffer[2];
			buffer[0] = a;
			buffer[1] = b;
			
	        for(int i = 0 ; i < 2 ; i ++ ){
      			std::cout << (int)buffer[i];
			}
			if (write(file_i2c, buffer, 2) != 2)       
	//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
			{
					/* ERROR HANDLING: i2c transaction failed */
					std::cout << "Failed to write to the i2c bus.\n";
			}
	//}

	return 0;
}
