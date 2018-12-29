#ifndef __FTDI_IMPL_H
#define __FTDI_IMPL_H

#include <stdint.h>
#include <vector>
#include <string>
#include <memory>
#include "ftdi.h"

/*=====================================================================================*/
/* This H file and its corresponding CPP file are the thin-layer implementation of     */
/* low level operation with FTDI chips. They are central operations for all FTDI chips */
/*=====================================================================================*/

namespace Zeabus_Elec
{

	/* Global constant */
	const uint32_t	ulDefaultBaudRate = 115200;
	const uint16_t	VendorID = 0x0403;	/* VendorID ID for all FTDI chips */
	
	enum ChipType
	{
		FT232 = 0x6001,
		FT2232 = 0x6010,
		FT4232H = 0x6011,
		FT232H = 0x6014
	};
	
	enum ErrorCode
	{
		ERR_INIT_FAILED = -1001,
		ERR_DISCOVERY_FAILED = -1002,
		ERR_OPEN_FAILED = -1003,
		ERR_UNSUPPORTED_DEVICE = -1004,
		ERR_MPSSE_SYNC_FAILED = -1005,
		ERR_GPIO_FAILED = -1006,
		ERR_COMMUNICATION_FAILED = -1007,
		ERR_UNSUPPORTED_FUNCTION = -1008
	};

	/* The base class used for all implementation of FTDI modes */
	class ftdi_impl
	{
	public:
		ftdi_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex = 0 );
		virtual ~ftdi_impl();

		int GetCurrentStatus() { return xCurrentStatus_; };
		void ClearError() { xCurrentStatus_ = 0; };
		
		/* Abstract method */
		/* Send data to communication channel */
		virtual uint32_t Send( const std::vector<uint8_t>& ) = 0;
		/* Receive data from communication channel */
		virtual uint32_t Receive( std::vector<uint8_t>& ) = 0;
		/* Set I/O direction of GPIO pins (if available). The parameter is the mask. 0 = Input, 1 = Output */
		virtual int SetGPIODirection( uint16_t, uint16_t ) = 0;
		/* Out data to high-byte GPIO pins */
		virtual int SetHiGPIOData( uint8_t ) = 0;
		/* Out data to low-byte GPIO pins */
		virtual int SetLoGPIOData( uint8_t ) = 0;
		/* Read data from high-byte GPIO pins */
		virtual uint8_t ReadHiGPIOData() = 0;
		/* Read data from low-byte GPIO pins */
		virtual uint8_t ReadLoGPIOData() = 0;
		
	protected:
		struct ftdi_context* xHwContext_;
		int xCurrentStatus_;
	};
	/*===================================================================*/

	/* Basic implementation class for MPSSE mode without any particular protocol (e.g. SPI, I2C). */
	class ftdi_mpsse_impl : public ftdi_impl
	{
	public:
		ftdi_mpsse_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex = 0 );
		
		/* Send data to communication channel */
		virtual uint32_t Send( const std::vector<uint8_t>& pucData );
		/* Receive data from communication channel */
		virtual uint32_t Receive( std::vector<uint8_t>& pucData );
		/* Set I/O direction of GPIO pins (if available). The parameter is the mask. 0 = Input, 1 = Output */
		virtual int SetGPIODirection( uint16_t usData, uint16_t ucData );
		/* Out data to high-byte GPIO pins */
		virtual int SetHiGPIOData( uint8_t ucData );
		/* Out data to low-byte GPIO pins */
		virtual int SetLoGPIOData( uint8_t ucData );
		/* Read data from high-byte GPIO pins */
		virtual uint8_t ReadHiGPIOData();
		/* Read data from low-byte GPIO pins */
		virtual uint8_t ReadLoGPIOData();
	
	protected:
		uint16_t usIODirection_;
	};
	/*===================================================================*/
	
	/* Implementation class for UART mode */
	class ftdi_uart_impl : public ftdi_impl
	{
	public:
		ftdi_uart_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex = 0 , uint32_t ulBaudRate = ulDefaultBaudRate);
		
		/* Change baud rate */
		int SetBaudRate( uint32_t ulBaudRate );
		
		/* Send data to communication channel */
		virtual uint32_t Send( const std::vector<uint8_t>& pucData );
		/* Receive data from communication channel */
		virtual uint32_t Receive( std::vector<uint8_t>& pucData );
		/* Set I/O direction of GPIO pins (if available). The parameter is the mask. 0 = Input, 1 = Output */
		virtual int SetGPIODirection( uint16_t, uint16_t );
		/* Out data to high-byte GPIO pins */
		virtual int SetHiGPIOData( uint8_t );
		/* Out data to low-byte GPIO pins */
		virtual int SetLoGPIOData( uint8_t );
		/* Read data from high-byte GPIO pins */
		virtual uint8_t ReadHiGPIOData();
		/* Read data from low-byte GPIO pins */
		virtual uint8_t ReadLoGPIOData();
	};
	/*===================================================================*/
	
	/* Generic implementation class for SPI mode. This class only manipulate low byte of the port. 
	   The SPI specifications are:
	     - CPOL0/1 => Idle of clock line is 0 or 1
	     - CPHA0/1 => Data is captured by host at the frist/second edge 
	     	and by device at the second/first edge of clock (rising or falling edge depends on
	     	the clock polarity (CPOL)
	     - MSB/LSB => Data are communicated with MSB first or LSB first. */
	class ftdi_spi_impl : public ftdi_mpsse_impl
	{
	public:
		ftdi_spi_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex = 0 );
		
		/* Send data to communication channel */
		virtual uint32_t Send( const std::vector<uint8_t>& pucData );
		/* Receive data from communication channel */
		virtual uint32_t Receive( std::vector<uint8_t>& pucData );
		/* Set I/O direction of GPIO pins (if available). The parameter is the mask. 0 = Input, 1 = Output */
		virtual int SetGPIODirection( uint16_t usData, uint16_t ucData );
		/* Out data to low-byte GPIO pins */
		virtual int SetLoGPIOData( uint8_t ucData );
	
	protected:
		const uint16_t SPIMaskOut_ = 0xFFF0;
		
		virtual uint16_t GetSPIIOMask() = 0;
		virtual uint8_t GetSPIIdleState() = 0;
		virtual uint8_t GetSPIWriteBlockCmd() = 0;
	};
	/*===================================================================*/
	
	/* Implementation for CPOL1+CHA0+MSB SPI used by the barometer and IMU IC*/
	class ftdi_spi_cpol1_cha0_msb_impl : public ftdi_spi_impl
	{
	public:
		ftdi_spi_cpol1_cha0_msb_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex = 0 );
		
	protected:
		virtual uint16_t GetSPIIOMask() { return( 0x000B ); };
		virtual uint8_t GetSPIIdleState() { return( 0x0B ); };
		virtual uint8_t GetSPIWriteBlockCmd() { return( 0x31 ); };
	};
}

#endif
