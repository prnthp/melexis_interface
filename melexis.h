/*
	MELEXIS.h - MELEXIS library

	For interfacing with the MLX90363 triaxis hall effect sensor
	
	by Travis Howse <tjhowse@gmail.com>
	2012.   License, GPL v2 or later

*/

#ifndef MELEXIS_h
#define MELEXIS_h

#include <inttypes.h>


class MELEXIS
{
	public:
		uint16_t get_x();
		uint16_t get_y();
		uint16_t get_z();
		uint8_t get_diag();
		uint16_t get_diag_0();
		uint16_t get_diag_1();
		uint8_t get_roll();
		uint8_t poll();
		uint8_t diag_poll();
		uint8_t do_SPI();

		MELEXIS();
	
	private:
		bool do_checksum(uint8_t* message);

};

#endif

