#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "ftdi_impl.h"

using namespace Zeabus_Elec;

/* ===================================================
 * Constants
 * ===================================================
 */
#define USB_BULK_SIZE	512

/* FTDI Command */
#define MSB_RISING_EDGE_BYTE_IN		(0x20)
#define SET_DATA_LOW_BYTE			(0x80)
#define GET_DATA_LOW_BYTE			(0x81)
#define SET_DATA_HIGH_BYTE			(0x82)
#define GET_DATA_HIGH_BYTE			(0x83)
#define SET_CLK_RATE				(0x86)
#define DISABLE_CLK_DIV				(0x8A)
#define DISABLE_THREE_PHASE			(0x8D)
#define DISABLE_ADAPTIVE_CLK		(0x97)


/*=====================================================================================*
 * The base class used for all implementation of FTDI modes							   *
 *=====================================================================================*/
ftdi_impl::ftdi_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex )
{
	auto numDevs = 0;
	struct ftdi_device_list *devInfo;
	
	/* Firstly, we must create the device context */
	xHwContext_ = ftdi_new();
	if( xHwContext_ == NULL )
	{
		fprintf( stderr, "Device-context creation failed\n" );
		xCurrentStatus_ = ERR_INIT_FAILED;
		return;
	}
	
	/* Scan whether there are any FTDI chips existing */
	numDevs = ftdi_usb_find_all( xHwContext_, &devInfo, 0, 0 );
	ftdi_list_free(&devInfo);
	if ( numDevs <= 0 )
	{
		fprintf( stderr, "No FTDI chip found\n" );
		xCurrentStatus_ = ERR_DISCOVERY_FAILED;
		return;
	}
	
	/* Set the interface instance of the chip */
	xCurrentStatus_ = ftdi_set_interface( xHwContext_, (ftdi_interface)( iIndex ) );
	if( xCurrentStatus_ < 0 )
	{
		fprintf( stderr, "Unable to select FTDI unit with error = %d\n", xCurrentStatus_ );
		xCurrentStatus_ = ERR_DISCOVERY_FAILED;
		return;
	}
	
	/* Then open the specified port */
	xCurrentStatus_ = ftdi_usb_open_desc( xHwContext_, VendorID, xChipID, stDeviceDesc.c_str(), NULL );
	if( xCurrentStatus_ < 0 )
	{
		fprintf( stderr, "Failed to open FTDI device\n" );		
		xCurrentStatus_ = ERR_OPEN_FAILED;
		return;
	}

	/* Sequence of initialization */
	/* Reset USB device */
	xCurrentStatus_ = ftdi_usb_reset( xHwContext_ );
	/* Purge USB receive and send buffer first */
	xCurrentStatus_ |= ftdi_usb_purge_buffers( xHwContext_ );

	/* Set USB request transfer sizes to 64K */
	xCurrentStatus_ |= ftdi_read_data_set_chunksize( xHwContext_, 65535 );
	xCurrentStatus_ |= ftdi_write_data_set_chunksize( xHwContext_, 65535 );

	/* Disable event and error characters */
	xCurrentStatus_ |= ftdi_set_event_char( xHwContext_, 0, 0 );
	xCurrentStatus_ |= ftdi_set_error_char( xHwContext_, 0, 0 );

	/* Set the latency timer to 1mS (default is 16mS) */
	xCurrentStatus_ |= ftdi_set_latency_timer( xHwContext_, 1 );

	/* Disable Flow Control */
	xCurrentStatus_ |= ftdi_setflowctrl( xHwContext_, SIO_DISABLE_FLOW_CTRL );
	
	/* End of the sequence, then check the error */
	if( xCurrentStatus_ != 0 )
	{
		fprintf( stderr, "Failed to initialize the specified FTDI module\n");
		xCurrentStatus_ = ERR_INIT_FAILED;
	}
	
	/* All basic preparation done */
}

ftdi_impl::~ftdi_impl()
{
	/* We should clear all context before exit */
	if( xCurrentStatus_ <= ERR_INIT_FAILED )
	{
		/* If xCurrentStatus_ > ERR_INIT_FAILED, it means that we failed before 
		successfully open the module. Therefore, we don't need to close it */
		ftdi_usb_close( xHwContext_ );
	}
	
	if( xHwContext_ != NULL )
	{
		ftdi_free ( xHwContext_ );
	}
}

/*=====================================================================================*
 * Basic implementation for MPSSE mode without any particular protocol (e.g. SPI, I2C).*
 *=====================================================================================*/
 
/* Initialize a device unit specified by xHandle to MPSSE mode (either I2C, SPI, or JTAG) */
ftdi_mpsse_impl::ftdi_mpsse_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex )
	: ftdi_impl( xChipID, stDeviceDesc, iIndex )
{
	int i;
	uint8_t aucUSBDataBuffer[2];
	
	/* Check whether the basic initialization was failed */
	if( xCurrentStatus_ != 0 )
	{
		/* If failed, we don't need to step further */
		return;
	}
	
	/* Check for the supported device. MPSSE exists only in FT2232D, FT2232H, FT232H, and FT4232H only */
	if( xHwContext_->type != TYPE_2232C &&		/* FT2232C/D/L */
		xHwContext_->type != TYPE_2232H &&		/* FT2232H */
		xHwContext_->type != TYPE_4232H &&		/* FT4232H */
		xHwContext_->type != TYPE_232H )		/* FT232H */
	{
		fprintf(stderr, "The specified device module does not support MPSSSE mode\n" );
		xCurrentStatus_ = ERR_UNSUPPORTED_DEVICE;
		return;
	}

	/*=== Entering MPSSE mode ===*/
	/* Reset controller */
	xCurrentStatus_ = ftdi_set_bitmode( xHwContext_, 0x0, BITMODE_RESET );
		
	/* Start MPSSE mode */
	xCurrentStatus_ |= ftdi_set_bitmode( xHwContext_, 0x0, BITMODE_MPSSE );
	
	if( xCurrentStatus_ != 0 )
	{
		fprintf( stderr, "Failed to initialize the specified FTDI module\n");
		xCurrentStatus_ = ERR_INIT_FAILED;
		return;
	}

	/* Wait 50ms for all operation to complete */
	(void)usleep( 50000 );
	
	/* If all above operations is successful, then synchronize MPSSE module 
	by sending a bogus command (0xAA). The device must response with "Bad Command" (0xFA)
	follow by the bogus command itself. */
	aucUSBDataBuffer[0] = 0xAA;
	/* Send off the BAD command */
	xCurrentStatus_ = ftdi_write_data ( xHwContext_, aucUSBDataBuffer, 1 );
	if( xCurrentStatus_ <= 0 )
	{
		/* Fail to write a bogus command to the device */
		fprintf( stderr, "Failed to write a bogus command 0xAA\n" );
		xCurrentStatus_ = ERR_MPSSE_SYNC_FAILED;
		return;
	}

	/* Wait for response with a finite loop. */
	i = 0;
	do
	{
		xCurrentStatus_ = ftdi_read_data( xHwContext_, aucUSBDataBuffer, 2);
		i++;
	}while( ( xCurrentStatus_ == 0 ) && ( i < 32767 ) );

	/* xCurrentStatus_ also indicates the total byte received. So, we check
	it against the required value or the loop has reached its maximum iteration */
	if( ( xCurrentStatus_ != 2 ) || ( i == 32767 ) )
	{
		/* Fail to write a bogus command to the device */
		fprintf( stderr, "Failed to read the response of the bogus command 0xAA\n" );
		xCurrentStatus_ = ERR_MPSSE_SYNC_FAILED;
		return;
	}

	/* We repeat the synchronization again with another bogus command */
	aucUSBDataBuffer[0] = 0xAB;
	/* Send off the BAD command */
	xCurrentStatus_ = ftdi_write_data ( xHwContext_, aucUSBDataBuffer, 1 );
	if( xCurrentStatus_ <= 0 )
	{
		/* Fail to write a bogus command to the device */
		fprintf( stderr, "Failed to write a bogus command 0xAB\n" );
		xCurrentStatus_ = ERR_MPSSE_SYNC_FAILED;
		return;
	}

	/* Wait for response with a finite loop. */
	i = 0;
	do
	{
		xCurrentStatus_ = ftdi_read_data( xHwContext_, aucUSBDataBuffer, 2);
		i++;
	}while( ( xCurrentStatus_ == 0 ) && ( i < 32767 ) );

	/* xCurrentStatus_ also indicates the total byte received. So, we check
	it against the required value or the loop has reached its maximum iteration */
	if( ( xCurrentStatus_ != 2 ) || ( i == 32767 ) )
	{
		/* Fail to write a bogus command to the device */
		fprintf( stderr, "Failed to read the response of the bogus command 0xAA\n" );
		xCurrentStatus_ = ERR_MPSSE_SYNC_FAILED;
		return;
	}
	
	/* All basic init done */
	xCurrentStatus_ = 0;
}

/* Send data to communication channel */
inline uint32_t ftdi_mpsse_impl::Send( const std::vector<uint8_t>& )
{
	/* Plain MPSSE mode is not ready for communication */
	xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;
	return( 0 );
}

/* Receive data from communication channel */
inline uint32_t ftdi_mpsse_impl::Receive( std::vector<uint8_t>& )
{
	/* Plain MPSSE mode is not ready for communication */
	xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;
	return( 0 );
}

/* Set I/O direction and initial state of GPIO pins (if available). The parameter is the mask. 0 = Input, 1 = Output */
int ftdi_mpsse_impl::SetGPIODirection( uint16_t usData, uint16_t ucData )
{
	uint8_t aucUSBDataBuffer[3];
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( xCurrentStatus_ );
	}
	
	usIODirection_ = usData;

	aucUSBDataBuffer[0] = SET_BITS_LOW;
	aucUSBDataBuffer[1] = static_cast<uint8_t>( ucData & 0xFF ); /* Initial pin state (Output data) */
	aucUSBDataBuffer[2] = static_cast<uint8_t>( usData & 0xFF ); /* Get only low byte */
	
	/* Write out the command */
	xCurrentStatus_ = ftdi_write_data( xHwContext_, aucUSBDataBuffer, 3 );
	
	/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully written */
	if( xCurrentStatus_ != 3 )
	{
		/* Write the command failed */
		fprintf( stderr, "Setting direction of LSB failed with error code %d (%s)\n", xCurrentStatus_, ftdi_get_error_string( xHwContext_ ) );
		xCurrentStatus_ = ERR_GPIO_FAILED;
		return( xCurrentStatus_ );
	}
	
	/* Set direction of high significant byte except for FT4232H as it has only low byte */
	if( xHwContext_->type != TYPE_4232H )
	{
		aucUSBDataBuffer[0] = SET_BITS_HIGH;
		aucUSBDataBuffer[1] = static_cast<uint8_t>( ( ucData >> 8 ) & 0xFF );	/* initial pin state (Output data) */
		aucUSBDataBuffer[2] = static_cast<uint8_t>( ( usData >> 8 ) & 0xFF ); /* High byte direction */
			
		/* Write out the command */
		xCurrentStatus_ = ftdi_write_data( xHwContext_, aucUSBDataBuffer, 3 );
	
		/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully written */
		if( xCurrentStatus_ != 3 )
		{
			/* Write the command failed */
			fprintf( stderr, "Setting direction of MSB failed\n" );
			xCurrentStatus_ = ERR_GPIO_FAILED;
			return( xCurrentStatus_ );
		}
	}
	
	/* Clear the status to indicate success */
	xCurrentStatus_ = 0;
	return( xCurrentStatus_ );
}

/* Out data to high-byte GPIO pins */
int ftdi_mpsse_impl::SetHiGPIOData( uint8_t ucData )
{
	uint8_t aucUSBDataBuffer[3];
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( xCurrentStatus_ );
	}

	if( xHwContext_->type == TYPE_4232H )
	{
		fprintf( stderr, "FT4232 does not have high byte\n" );
		xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;		/* FT4232H only has low-byte GPIO */
	}
	else
	{
		aucUSBDataBuffer[0] = SET_BITS_HIGH;
		aucUSBDataBuffer[1] = ucData;	/* Output data */
		aucUSBDataBuffer[2] = static_cast<uint8_t>( ( usIODirection_ >> 8 ) & 0xFF ); /* High byte direction */

		/* Write out the command */
		xCurrentStatus_ = ftdi_write_data( xHwContext_, aucUSBDataBuffer, 3 );
	
		/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully written */
		if( xCurrentStatus_ != 3 )
		{
			/* Write the command failed */
			fprintf( stderr, "Setting value of high byte failed\n" );
			xCurrentStatus_ = ERR_GPIO_FAILED;
			return( xCurrentStatus_ );
		}
		else
		{
			/* Clear the status to indicate success */
			xCurrentStatus_ = 0;
		}	
	}
	
	return( xCurrentStatus_ );
}

/* Out data to low-byte GPIO pins */
int ftdi_mpsse_impl::SetLoGPIOData( uint8_t ucData )
{
	uint8_t aucUSBDataBuffer[3];

	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( xCurrentStatus_ );
	}

	aucUSBDataBuffer[0] = SET_BITS_LOW;
	aucUSBDataBuffer[1] = ucData;	/* Output data */
	aucUSBDataBuffer[2] = static_cast<uint8_t>( usIODirection_ & 0xFF ); /* Low byte direction */

	/* Write out the command */
	xCurrentStatus_ = ftdi_write_data( xHwContext_, aucUSBDataBuffer, 3 );
	
	/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully written */
	if( xCurrentStatus_ != 3 )
	{
		/* Write the command failed */
		fprintf( stderr, "Setting value of low byte failed\n" );
		xCurrentStatus_ = ERR_GPIO_FAILED;
		return( xCurrentStatus_ );
	}
	else
	{
		/* Clear the status to indicate success */
		xCurrentStatus_ = 0;
	}	
	
	return( xCurrentStatus_ );
}

/* Read data from high-byte GPIO pins */
uint8_t ftdi_mpsse_impl::ReadHiGPIOData()
{
	uint8_t aucUSBDataBuffer[3];
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( 0 );
	}

	if( xHwContext_->type == TYPE_4232H )
	{
		fprintf( stderr, "FT4232H does not have high byte\n" );
		xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;		/* FT4232H only has low-byte GPIO */
		return( 0 );
	}
	else
	{
		aucUSBDataBuffer[0] = GET_BITS_HIGH;	/* Command to read GPIO */

		/* Write out the command */
		xCurrentStatus_ = ftdi_write_data( xHwContext_, aucUSBDataBuffer, 1 );
	
		/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully written */
		if( xCurrentStatus_ != 1 )
		{
			/* Write the command failed */
			fprintf( stderr, "Request for high byte GPIO data failed\n" );
			xCurrentStatus_ = ERR_GPIO_FAILED;
			return( 0 );
		}
		else
		{
			/* Read back the port data */
			do
			{
				xCurrentStatus_ = ftdi_read_data( xHwContext_, aucUSBDataBuffer, 1 );
			}while( xCurrentStatus_ == 0 );
			
			/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully received */
			if( xCurrentStatus_ != 1 )
			{
				/* Write the command failed */
				fprintf( stderr, "Reading high byte GPIO data failed\n" );
				xCurrentStatus_ = ERR_GPIO_FAILED;
				return( 0 );
			}

			/* Clear the status to indicate success */
			xCurrentStatus_ = 0;
		}	
	}
	return( aucUSBDataBuffer[0] );	
}

/* Read data from low-byte GPIO pins */
uint8_t ftdi_mpsse_impl::ReadLoGPIOData()
{
	uint8_t aucUSBDataBuffer[2];
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( 0 );
	}

	aucUSBDataBuffer[0] = GET_BITS_LOW;	/* Command to read GPIO */

	/* Write out the command */
	xCurrentStatus_ = ftdi_write_data( xHwContext_, aucUSBDataBuffer, 1 );
	
	/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully written */
	if( xCurrentStatus_ != 1 )
	{
		/* Write the command failed */
		fprintf( stderr, "Request for low byte GPIO data failed\n" );
		xCurrentStatus_ = ERR_GPIO_FAILED;
		return( 0 );
	}
	else
	{
		/* Read back the port data */
		do
		{
			xCurrentStatus_ = ftdi_read_data( xHwContext_, aucUSBDataBuffer, 1 );
		}while( xCurrentStatus_ == 0 );

		
		/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully received */
		if( xCurrentStatus_ != 1 )
		{
			/* Write the command failed */
			fprintf( stderr, "Reading low byte GPIO data failed\n" );
			xCurrentStatus_ = ERR_GPIO_FAILED;
			return( 0 );
		}

		/* Clear the status to indicate success */
		xCurrentStatus_ = 0;
	}	
	return( aucUSBDataBuffer[0] );	
}
	
/*===================================================================*/

/*===================================================================*/
/* Implementation body of UART										 */
/*===================================================================*/

/* Initialize a device unit specified by xHandle to MPSSE mode (either I2C, SPI, or JTAG) */
ftdi_uart_impl::ftdi_uart_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex, uint32_t ulBaudRate )
	: ftdi_impl( xChipID, stDeviceDesc, iIndex )
{
	/* If there was an unrecoverable error, we cannot do anything */
	if( xCurrentStatus_ != 0 )
	{
		return;
	}

	/* Set Baud-Rate */
	xCurrentStatus_ = ftdi_set_baudrate ( xHwContext_, ulBaudRate );
		
	/* Set 8 data bits, 1 stop bit and no parity */
	xCurrentStatus_ |= ftdi_set_line_property( xHwContext_, BITS_8, STOP_BIT_1, NONE );

	if( xCurrentStatus_ != 0 )
	{
		fprintf( stderr, "Failed to initialize the specified FTDI module\n");
		xCurrentStatus_ = ERR_INIT_FAILED;
		return;
	}
		
	/* Wait 50ms for all operation to complete */
	(void)usleep( 50000 );
}

/* Change baud rate */
int ftdi_uart_impl::SetBaudRate( uint32_t ulBaudRate )
{
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( xCurrentStatus_ );
	}

	/* Set Baud-Rate */
	xCurrentStatus_ = ftdi_set_baudrate ( xHwContext_, ulBaudRate );
	
	if( xCurrentStatus_ != 0 )
	{
		fprintf( stderr, "Failed to initialize the specified FTDI module\n");
		xCurrentStatus_ = ERR_INIT_FAILED;
	}
	return( xCurrentStatus_ );
}

/* Send data to communication channel */
uint32_t ftdi_uart_impl::Send( const std::vector<uint8_t>& pucData )
{
	uint32_t ulAccWritten;
	uint32_t ulLen = pucData.size();
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( 0 );
	}

	/* Loop writing data when the amount of written data is still less than the required one 
	and ftdi_write_data still return value >= 0 */
	ulAccWritten = 0;
	do
	{
		xCurrentStatus_ = ftdi_write_data( xHwContext_, ( const_cast<uint8_t*>( pucData.data() ) + ulAccWritten ) , ulLen );
		/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully sent */
		if( xCurrentStatus_ < 0 )
		{
			break;
		}
		ulLen -= (uint32_t)xCurrentStatus_;
		ulAccWritten += (uint32_t)xCurrentStatus_;
	}while( ulLen > 0 );
	
	/* Clear all status to success even partially */
	xCurrentStatus_ = 0;
	
	return( ulAccWritten );
}

/* Receive data from communication channel */
uint32_t ftdi_uart_impl::Receive( std::vector<uint8_t>& pucData )
{
	uint8_t buffer[128];
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( 0 );
	}

	pucData.clear();
	
	/* Read the data */
        uint8_t round = 0;      /* limit the waiting time */
	do
	{
		xCurrentStatus_ = ftdi_read_data( xHwContext_, buffer, 128 );
		
		/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully received */
		if( xCurrentStatus_ > 0 )
		{
			/* Append the read-in data */
			pucData.insert( pucData.end(), buffer, ( buffer + xCurrentStatus_ ) );
		}

                round++;

	}while( ( ( xCurrentStatus_ == 0 ) || ( xCurrentStatus_ == 128 ) ) && ( round <= 10 ) );
	
	if( xCurrentStatus_ >= 0 )
	{
		/* If the reading has succeeded, clear all status to success even partially */
		xCurrentStatus_ = 0;
	}
	else
	{
		fprintf( stderr, "Reading UART failed\n" );
		xCurrentStatus_ = ERR_COMMUNICATION_FAILED;
	}

	return( pucData.size() );	/* Return the actual amount of received data */
}

/* Set I/O direction of GPIO pins (if available). The parameter is the mask. 0 = Input, 1 = Output */
inline int ftdi_uart_impl::SetGPIODirection( uint16_t, uint16_t )
{
	/* UART mode does not have any GPIO */
	xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;
	return( ERR_UNSUPPORTED_FUNCTION );
}

/* Out data to high-byte GPIO pins */
inline int ftdi_uart_impl::SetHiGPIOData( uint8_t )
{
	/* UART mode does not have any GPIO */
	xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;
	return( ERR_UNSUPPORTED_FUNCTION );
}

/* Out data to low-byte GPIO pins */
inline int ftdi_uart_impl::SetLoGPIOData( uint8_t )
{
	/* UART mode does not have any GPIO */
	xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;
	return( ERR_UNSUPPORTED_FUNCTION );
}

/* Read data from high-byte GPIO pins */
inline uint8_t ftdi_uart_impl::ReadHiGPIOData()
{
	/* UART mode does not have any GPIO */
	xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;
	return( 0 );
}

/* Read data from low-byte GPIO pins */
inline uint8_t ftdi_uart_impl::ReadLoGPIOData()
{
	/* UART mode does not have any GPIO */
	xCurrentStatus_ = ERR_UNSUPPORTED_FUNCTION;
	return( 0 );
}
	
/*===================================================================*/

/*=====================================================================================*
 * Implementation class for SPI mode. This class only manipulate low byte of the port. *
 *=====================================================================================*/

/* Constructor. Initialize SPI */
ftdi_spi_impl::ftdi_spi_impl(enum ChipType xChipID, std::string stDeviceDesc, int iIndex )
	: ftdi_mpsse_impl( xChipID, stDeviceDesc, iIndex )
{
}
		
/* Send data to communication channel */
uint32_t ftdi_spi_impl::Send( const std::vector<uint8_t>& pucData )
{
	std::vector<uint8_t> pucBuffer;
	uint8_t ucPortValue;
	uint32_t ulAccWritten, ulLen;
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( 0 );
	}

	/* Prepare the buffer */
	pucBuffer.reserve( pucData.size() + 9 );

	/* Prepare the commands */
	
	/* Step 0: Get old status of the LSB port */
	ucPortValue = ReadLoGPIOData();
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( 0 );
	}

	/* Step 1: Set CS to low (active) */
	pucBuffer.push_back( SET_DATA_LOW_BYTE );
	pucBuffer.push_back( ( ucPortValue & 0xF0 ) | ( GetSPIIdleState() & 0x07 ) );
        pucBuffer.push_back( usIODirection_ );

	/* Step 2: Get 16-bit data at rising edge clock without any data out. 
	The data size 0 means 1 byte and so on. */
	pucBuffer.push_back( GetSPIWriteBlockCmd());
        pucBuffer.push_back( ( pucData.size() - 1 ) & 0xFF );
        pucBuffer.push_back( ( ( pucData.size() - 1 ) >> 8 ) & 0xFF );
	pucBuffer.insert( pucBuffer.end(), pucData.begin(), pucData.end() );
	
	/* Step 3: Set CS back to high (idle) */
	pucBuffer.push_back( SET_DATA_LOW_BYTE );
	pucBuffer.push_back( ( ucPortValue & 0xF0 ) | ( GetSPIIdleState() & 0x0F ) );
        pucBuffer.push_back( usIODirection_ );

	/* Write the commands */

	/* Loop writing data when the amount of written data is still less than the required one 
	and ftdi_write_data still return value >= 0 */
	ulAccWritten = 0;
        ulLen = pucBuffer.size();
	do
	{
		xCurrentStatus_ = ftdi_write_data( xHwContext_, ( const_cast<uint8_t*>( pucBuffer.data() ) + ulAccWritten ) , ulLen );
		/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully sent */
		if( xCurrentStatus_ < 0 )
		{
			break;
		}
		ulLen -= (uint32_t)xCurrentStatus_;
		ulAccWritten += (uint32_t)xCurrentStatus_;
	}while( ( xCurrentStatus_ >= 0 ) && ( ulAccWritten < pucBuffer.size() ) );

	if( xCurrentStatus_ < 0 )
	{
		fprintf( stderr, "Writing to SPI failed\n" );
		xCurrentStatus_ = ERR_COMMUNICATION_FAILED;
	}
	else
	{
		/* Clear all status to success */
		xCurrentStatus_ = 0;
	}

	/* Return the total written byte */
	return( ulAccWritten ); 
}

/* Receive data from communication channel */
uint32_t ftdi_spi_impl::Receive( std::vector<uint8_t>& pucData )
{
	uint8_t buffer[128];
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return( 0 );
	}

	pucData.clear();
		
	/* Read the data */
	do
	{
		xCurrentStatus_ = ftdi_read_data( xHwContext_, buffer, 128 );
		
		/* if xCurrentStatus_ >= 0, it indicates the number of byte successfully received */
		if( xCurrentStatus_ > 0 )
		{
			/* Append the read-in data */
			pucData.insert( pucData.end(), buffer, ( buffer + xCurrentStatus_ ) );
		}
	}while( ( xCurrentStatus_ == 0 ) || ( xCurrentStatus_ == 128 ) );
	
	if( xCurrentStatus_ >= 0 )
	{
		/* If the reading has succeeded, clear all status to success even partially */
		xCurrentStatus_ = 0;
	}
	else
	{
		fprintf( stderr, "Reading SPI failed\n" );
		xCurrentStatus_ = ERR_COMMUNICATION_FAILED;
	}

	return( pucData.size() );	/* Return the actual amount of received data */
}

/* Override of set I/O direction of GPIO pins (if available). The parameter is the mask. 0 = Input, 1 = Output */
int ftdi_spi_impl::SetGPIODirection( uint16_t usData, uint16_t ucData )
{
	/* This method should mask out the direction setting to prevent interfering to SPI */
	usData &= SPIMaskOut_;
	usData |= GetSPIIOMask();

        /* mask out the initial state setting to prevent interfering to SPI */
        ucData &= SPIMaskOut_;
        ucData |= GetSPIIdleState();
	
	return( ftdi_mpsse_impl::SetGPIODirection( usData, ucData) );
}

/* Out data to low-byte GPIO pins */
int ftdi_spi_impl::SetLoGPIOData( uint8_t ucData )
{
	/* This method should mask the data from interfering with SPI data lines */
	ucData &= 0xF0;
	ucData |= GetSPIIdleState();
	
	return( ftdi_mpsse_impl::SetLoGPIOData( ucData ) );
}

/*===================================================================*/

ftdi_spi_cpol1_cha0_msb_impl::ftdi_spi_cpol1_cha0_msb_impl( enum ChipType xChipID, std::string stDeviceDesc, int iIndex )
		: ftdi_spi_impl( xChipID, stDeviceDesc, iIndex )
{
	uint8_t aucUSBDataBuffer[9];
	
	/* If there was an unrecoverable error, we cannot do anything */
	if( ( xCurrentStatus_ != 0 ) && ( xCurrentStatus_ != ERR_UNSUPPORTED_FUNCTION ) )
	{
		fprintf( stderr, "Cannot perform the request when an unrecoverable error occurred\n" );
		return;
	}

	/* Send consecutive commands as:
	 * 1. Disable 1/5 clock divisor. Therefore, the SPI clock is computed from 60MHz.
	 * 2. Disable adaptive clocking.
	 * 3. Disable three-phase clocking used only in i2c.
	 * 4. Set SPI clock following the formular 60MHz / ( ( 1 + ValueH:ValueL ) * 2 ). (0x0007 means 3.75MHz)
	 * 5. Set pin states and directions. The pins are: (MSB)GPIO3:0, CS, DI, DO, CLK(LSB)
	 */
	aucUSBDataBuffer[0] = DISABLE_CLK_DIV;
	aucUSBDataBuffer[1] = DISABLE_ADAPTIVE_CLK;
	aucUSBDataBuffer[2] = DISABLE_THREE_PHASE;
	aucUSBDataBuffer[3] = SET_CLK_RATE; /* Set to 3.75MHz */
	aucUSBDataBuffer[4] = 0x07;		/* Lo-Byte */
	aucUSBDataBuffer[5] = 0;		/* Hi-Byte */
	aucUSBDataBuffer[6] = SET_DATA_LOW_BYTE;
	aucUSBDataBuffer[7] = GetSPIIdleState(); /* Pin state (Output data). CLK, DI, and CS are normally high. */
	aucUSBDataBuffer[8] = GetSPIIdleState(); /* All pins are output except the DI pin (bit value 1 means the corresponding pin is output) */

        usIODirection_ = GetSPIIdleState();

	/* Write out the commands (2 commands at once) */
	xCurrentStatus_ = ftdi_write_data( xHwContext_, aucUSBDataBuffer, 9 );
		
	/* if xCurrentStatus_ also indicates the number of byte successfully sent and it should equal to 9 */
	if( xCurrentStatus_ != 9 )
	{
		xCurrentStatus_ = ERR_INIT_FAILED;
	}
	else
	{
		xCurrentStatus_ = 0;	/* Clear the status to ok */
	}
			
	/* Wait for 30ms for USB operation to complete */
	(void)usleep( 30000 );
}
