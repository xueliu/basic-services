//
// Created by liu on 01.02.2021.
//
#include <iostream>

#include "serial-device.h"

using namespace basic;

auto main() -> int
{
	SerialDevice::Configuration serial_config{115200, 8, SerialDevice::Parity::kNone, 1, SerialDevice::FlowControl::kNone, 10, true};

	SerialBufferDevice serial_dev{"COM4", serial_config, 4};

//	std::uint8_t buffer[16] = {0};
//	std::uint8_t * head = buffer;

	const std::uint8_t * buffer;

	while (true)
	{
		auto sz = serial_dev.ReadSome(buffer);
		auto head = buffer;
		if(sz > 0)
		{
			std::cout << "Serial Port Received:";

			while(  (*head != 0) )
			{
				std::cout << std::hex << (std::uint8_t)*head;
				++head;
			}
			std::cout << std::endl;

			serial_dev.Remove(sz);
		}
	}

	return 0;

}