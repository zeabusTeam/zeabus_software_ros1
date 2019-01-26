/* Hardware driver for 3DM-GX4-45 IMU unit
 * The device communicates with the host computer through a virtual serial port.
 * The device is configured to "/dev/ttyACM0" by default. The configurations of 
 * the port are:
 *  - Neither hardware nor software flow-control is used.
 *  - Baud rate is 115200 by default
 *  - Data format is 8-bit with 1 stop-bit but without parity bit
 *  - All read and write operations are in asynchronous mode with time-out of 2 seconds
 *
 * Version 1.0.0. Author by Akrapong Patchararungruang on May 14, 2017
 *
 */
#ifndef __ZEABUS_ELEC_IMU_3DM_GX4_H
#define __ZEABUS_ELEC_IMU_3DM_GX4_H

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#include <string>
#include <vector>
#include <iterator>
#include <memory>

#include "mip.h"

namespace Zeabus_Elec
{

	class IOError : public std::runtime_error
	{
	public:
		IOError(const std::string& description)
			: std::runtime_error(description)
		{ }
	};

	class Quaternion_t
	{
	public:
		double x, y, z, w;
		
		Quaternion_t() : x(0.0), y(0.0), z(0.0), w(0.0) {};
	};

	/*====================================================================
	 * RawSerial is the physical layer used to manage and control the 
	 * serial port directly. It knows nothing about the packet.
	 *====================================================================*/
	class RawSerial
	{
	protected:
		struct pollfd io_;
	
	public:
		typedef std::vector<uint8_t> DataStream_t;
	
		RawSerial() {};
		virtual ~RawSerial()
		{
			ClosePort();
		};
	
		/* Open the serial port device and configures it for asynchronous access */
		void OpenPort( const std::string& strDevName );
	
		/* Read from the opened port. In case of no available data, then wait for iTimeOut before retarding */
		uint32_t AsyncRead( DataStream_t& data, int iTimeOut_ms = 2000 );

		/* Write the opened port. In case of port busy, then wait for iTimeOut before retarding */
		uint32_t AsyncWrite( const DataStream_t& data, int iTimeOut_ms = 2000 );

		/* Close the opened port and set the handle to an invalid value	*/
		void ClosePort()
		{
			if( io_.fd != -1 )
			{
				close( io_.fd );
			}
		};
	};
	/*====================================================================*/
	
	/*====================================================================
	 * Link_3dm_gx4 is the link layer used to assembly the packet from a stream and 
	 * any packet level functions such as verifying check-sum and discarding invalid
	 * packet.
	 *====================================================================*/
	class Link_3dm_gx4
	{
	protected:
		RawSerial channel_;
		RawSerial::DataStream_t incomingPacketFragment_;
		RawSerial::DataStream_t outgoingPacketFragment_;
		
	public:
		Link_3dm_gx4( const std::string& port );
		
		/* Read from the opened port. In case of no available data, then wait for iTimeOut before retarding 
		   The method return "true" if a complete packet has been retrieved. */
		bool AsyncRead( MIPPacket& packet, int iTimeOut_ms = 2000 );

		/* Write the opened port. In case of port busy, then wait for iTimeOut before retarding.
		   If data cannot be sent out at all, the method returns "false". */
		bool AsyncWrite( const MIPPacket& packet, int iTimeOut_ms = 2000 );
	};
	/*====================================================================*/

	/*====================================================================
	 * imu_3dm_gx4 is the protocol layer used to manage packet based communication.
	 * It consists of base methods for sending and receiving packets as well as
	 * helper methods for some functions such as SetToIdle and Ping 
	 *====================================================================*/
	class imu_3dm_gx4
	{
	protected:
		std::shared_ptr<Link_3dm_gx4> linklayer_;
		
	public:
		imu_3dm_gx4( const std::string& port );
		
		/*=============== Fundamental functions ================*/
		
		/* Read from the opened port. In case of no available data, then wait for iTimeOut before retarding 
		   The method return "true" if a complete packet has been retrieved. */
		bool ReadPacket( MIPPacket& packet, int iTimeOut_ms = 2000 );

		/* Write the opened port. In case of port busy, then wait for iTimeOut before retarding.
		   If data cannot be sent out at all, the method returns "false". */
		bool WritePacket(const MIPPacket& packet, int iTimeOut_ms = 2000 );
		
		/* Write a packet and Read a reply packet from the opened port. 
		   In case of no available data, then wait for iTimeOut before retry for iTries times, then retard.
		   The method return "true" if a complete packet has been retrieved. */
		bool WriteAndReadPacket( MIPPacket& packet, int iTimeOut_ms = 2000, int iTries = 3 );

		/*=============== Helper functions only general used ================*/
		bool Ping();
		bool SetToIdle();
		bool ResetDevice();
		bool ResumeDevice();
		bool InitializeFilterwithMagneto();
		
		/*=============== Helper functions only for Zeabus ================*/
		bool SetIMUDataFormat( uint16_t rateDecimation ); /* Actual rate = 100Hz / rateDecimation */
		bool EnableDataStream();
	};
} /* Name space */

#endif /* __ZEABUS_ELEC_IMU_3DM_GX4_H */