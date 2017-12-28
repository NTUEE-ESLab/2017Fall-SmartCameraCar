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
			return 0;
	}

	int addr = 0x04;          //<<<<<The I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
			//ERROR HANDLING; you can check errno to see what went wrong
			std::cout << "Failed to acquire bus access and/or talk to slave..
	\n";
			return 0;
	}

	//----- WRITE BYTES -----
	while (true)
	{
			std::string tmp;
			std::cout << "Enter mode (1: forward 2: backward 3: right 4: leff
	t 0: stop): ";
			std::cin >> tmp;
			char* buffer = new char[tmp.length()+1];
			std::strcpy(buffer, tmp.c_str());
			if (write(file_i2c, buffer, tmp.length()) != tmp.length())       
	//write() returns the number of bytes actually written, if it doesn't maa
	tch then an error occurred (e.g. no response from the device)
			{
					/* ERROR HANDLING: i2c transaction failed */
					std::cout << "Failed to write to the i2c bus.\n";
			}
	}

	return 0;
}
