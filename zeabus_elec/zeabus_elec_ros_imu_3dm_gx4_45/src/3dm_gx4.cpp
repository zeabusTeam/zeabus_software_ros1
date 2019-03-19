#include "../include/3dm_gx4.h"

#include <array>
#include <algorithm>
#include <exception>

using namespace Zeabus_Elec;

/*====================================================================
 * Implementation of RawSerial, the physical layer
 *====================================================================*/

/* Open a specified serial port device and set it to non blocking with
8n1 format and baudrate of 115200 */
void RawSerial::OpenPort( const std::string& strDevName )
{
	termios options;
	
	/* Open the port for raw operation */
	io_.fd = open( strDevName.c_str(), O_NOCTTY | O_RDWR | O_NONBLOCK );
	if( io_.fd < 0 )
	{
		throw IOError("Failed to open a serial port");
	}
	
	/* Retrieve the current port setting */
	if( tcgetattr( io_.fd, &options ) != 0 )
	{
		/* Fail to get the attributes, then we close the port */
		close( io_.fd );
		io_.fd = -1;
		throw IOError("Failed to get attributes from an opened serial port");
	}
	
	/* Set baudrate to 115200 bps */
	cfsetispeed( &options, B115200 );
	cfsetospeed( &options, B115200 );

	/* Set data format to 8n1 */
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	
	/* Disable both hardware and software flow-control */
	options.c_cflag &= ~CRTSCTS;
	options.c_iflag &= ~( IXON | IXOFF | IXANY );
	
	/* Set port mode to raw mode */
	options.c_cflag &= ~OPOST;
	options.c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG );

	/* Set the new port setting */
	if( tcsetattr( io_.fd, TCSANOW, &options ) != 0 )
	{
		/* Fail to set the attributes, then we close the port */
		close( io_.fd );
		io_.fd = -1;
		throw IOError("Failed to set attributes of an opened serial port");
	}
}
/*====================================================================*/

/* Read from the opened port. In case of no available data, then wait for iTimeOut before retarding */
uint32_t RawSerial::AsyncRead( DataStream_t& data, int iTimeOut_ms )
{
	uint8_t buffer[400];
	int pollRet;
	size_t totalRead;
	
	/* Preparing data */
	totalRead = 0;
	
	if( io_.fd > 0 )
	{
		/* The device has been prepared */	
		io_.events = POLLIN;	/* Poll for input */
	
		/* Polling for data */
		pollRet = poll( &io_, 1, iTimeOut_ms );
	
		if( pollRet < 0 )
		{
			/* Error!! occurred */
			throw IOError("Serial port generates I/O error while polling");
		}
		
		if( pollRet > 0 )
		{
			/* We detect the event */
			if( ( io_.revents & POLLIN ) > 0 ) /* Checking whether the event is our waiting one */
			{
				totalRead = read( io_.fd, buffer, 400 );
				if( totalRead < 0 )
				{
					/* Fail to read the port */
					throw IOError("Failed to read from serial port");
				}
				
				if( totalRead > 0 )
				{
					/* Just append the new data to the buffer in case of non-empty buffer */
					data.insert( data.end(), buffer, ( buffer + totalRead ) );
				}
			}
			io_.revents = 0;	/* Clear the event */
		}
	}

	/* Return the total read data */
	return( totalRead ) ;	
}
/*====================================================================*/

/* Write the opened port. In case of port busy, then wait for iTimeOut before retarding */
uint32_t RawSerial::AsyncWrite( const DataStream_t& data, int iTimeOut_ms )
{
	int pollRet;
	size_t totalWrite;
	
	totalWrite = 0;
	
	if( io_.fd > 0 )
	{
		/* The device has been prepared */	
		io_.events = POLLOUT;	/* Poll for input */
	
		/* Polling for data */
		pollRet = poll( &io_, 1, iTimeOut_ms );
	
		if( pollRet < 0 )
		{
			/* Error!! occurred */
			throw IOError("Serial port generates I/O error while polling");
		}
		
		if( pollRet > 0 )
		{
			/* We detect the event */
			if( ( io_.revents & POLLOUT ) > 0 ) /* Checking whether the event is our waiting one */
			{
				totalWrite = write( io_.fd, data.data(), data.size() );
				if( totalWrite < 0 )
				{
					/* Fail to read the port */
					throw IOError("Failed to write from serial port");
				}
			}
			io_.revents = 0;	/* Clear the event */
		}
	}

	/* Return the total written data */
	return( totalWrite ) ;	
}
/*====================================================================*/

/*====================================================================
 * Link_3dm_gx4 is the link layer used to assembly the packet from a stream and 
 * any packet level functions such as verifying check-sum and discarding invalid
 * packet.
 *====================================================================*/
	
/* Constructor */
Link_3dm_gx4::Link_3dm_gx4( const std::string& port )
{
	channel_.OpenPort( port );
}
/*====================================================================*/
	
/* Read from the opened port. In case of no available data, then wait for iTimeOut before retarding 
		   The method return "true" if a complete packet has been retrieved. */
bool Link_3dm_gx4::AsyncRead( MIPPacket& packet, int iTimeOut_ms )
{
	RawSerial::DataStream_t::iterator packetBegin;
	
	channel_.AsyncRead( incomingPacketFragment_, iTimeOut_ms );
	
	if( incomingPacketFragment_.size() <= 0 )
	{
		/* We get nothing from reading */
		return( false );
	}
	
	/* parsing for a valid packet */

	/* Looking for sync. string */
	for( packetBegin = incomingPacketFragment_.begin();
		packetBegin < ( incomingPacketFragment_.end() - 1 );
		packetBegin++ )
	{
		if( ( ( *packetBegin ) == MIPPacket::SYNC1 ) && ( ( *( packetBegin + 1 ) ) == MIPPacket::SYNC2 ) )
			break;
	}
	
	if( packetBegin == ( incomingPacketFragment_.end() - 1 ) )
	{
		/* No SYNC found. The last element may be the first SYNC byte or none at all.
		Therefore, we trim all garbage and return "false" to indicate that nothing useful was found*/
		incomingPacketFragment_.erase( incomingPacketFragment_.begin(), ( incomingPacketFragment_.end() - 1 ) );
	}
	else
	{
		/* We have found the sync pattern. So we try to extract the packet */
		
		/* 1. Trim the garbage at the front of the package */
		if( packetBegin != incomingPacketFragment_.begin() )
		{
			incomingPacketFragment_.erase( incomingPacketFragment_.begin(), packetBegin );
			packetBegin = incomingPacketFragment_.begin();
		}
		
		/* 2. If the size of available stream is less than MIPPacket::PACKET_MIN_LENGTH
		       it means that the strem store only a fragment of packet. Therefore, 
		       we just do nothing and wait for more data coming */
		if( incomingPacketFragment_.size() < MIPPacket::PACKET_MIN_LENGTH )
		{
			return( false );
		}
		
		/* 3. Parse the packet length and see whether the whole packet is ready (all payloads were retrieved). 
		       Otherwise, we just do nothing and wait again. */
		if( incomingPacketFragment_[3] > ( incomingPacketFragment_.size() - MIPPacket::PACKET_MIN_LENGTH ) )
		{
			return( false );
		}
		
		/* 4. Now at least a full packet is ready. Then, we extract it */
		auto packetEnd = incomingPacketFragment_.begin() + ( MIPPacket::PACKET_MIN_LENGTH + incomingPacketFragment_[3] );

		packet.GenPacket( packetBegin, packetEnd );
		incomingPacketFragment_.erase( packetBegin, packetEnd ); /* Erase extracted packet */
		
		/* 5. Verify whether the packet has correct checksum. If not then we discard it */
		if( packet.VerifyChecksum() )
		{
			return( true );
		}
		else
		{
			packet.ResetPacket();
		}
			
		return( false );
	}
}
/*====================================================================*/

/* Write the opened port. In case of port busy, then wait for iTimeOut before retarding.
   If the data cannot be sent at all, the method returns "false". */
bool Link_3dm_gx4::AsyncWrite( const MIPPacket& packet, int iTimeOut_ms )
{
	/* Revoke "const" qualifier from "packet" */
	MIPPacket::Packet_t p = const_cast<MIPPacket&>(packet).GetPacket();
	
	/* Append the packet into outgoing queue */
	outgoingPacketFragment_.insert( outgoingPacketFragment_.end(), p.begin(), p.end() );
	
	/* Try to send out the buffer with time-out */
	auto totalWrite = channel_.AsyncWrite( outgoingPacketFragment_, iTimeOut_ms );
	
	if( totalWrite == 0 )
	{
		return( false );	/* We cannot write any data */
	}
	
	/* Stripe the written data */
	if( totalWrite == outgoingPacketFragment_.size() )
	{
		/* The whole awaiting data was written */
		outgoingPacketFragment_.clear();
	}
	else
	{
		outgoingPacketFragment_.erase( outgoingPacketFragment_.begin(), ( outgoingPacketFragment_.begin() + totalWrite ) );
	}
	
	return( true );  /* Some or all data were able to send out */
}
/*====================================================================*/

/*====================================================================
 * imu_3dm_gx4 is the protocol layer used to manage packet based communication.
 * It consists of base methods for sending and receiving packets as well as
 * helper methods for some functions such as SetToIdle and Ping 
 *====================================================================*/

/* Constructor */	
imu_3dm_gx4::imu_3dm_gx4( const std::string& port )
{
	linklayer_ = std::make_shared<Link_3dm_gx4>( port );
}

/*=============== Fundamental functions ================*/

/* Read from the opened port. In case of no available data, then wait for iTimeOut before retarding 
   The method return "true" if a complete packet has been retrieved. */
inline bool imu_3dm_gx4::ReadPacket( MIPPacket& packet, int iTimeOut_ms )
{
	return( linklayer_->AsyncRead( packet, iTimeOut_ms ) );
}
/*====================================================================*/

/* Write the opened port. In case of port busy, then wait for iTimeOut before retarding.
   If data cannot be sent out at all, the method returns "false". */
inline bool imu_3dm_gx4::WritePacket(const MIPPacket& packet, int iTimeOut_ms )
{
	return( linklayer_->AsyncWrite( packet, iTimeOut_ms ) );
}
/*====================================================================*/

/* Write a packet and Read a reply packet from the opened port. 
   In case of no available data, then wait for iTimeOut before retry for iTries times, then retard.
   The method return "true" if a complete packet has been retrieved. */
bool imu_3dm_gx4::WriteAndReadPacket( MIPPacket& packet, int iTimeOut_ms, int iTries )
{
	bool res;
	
	/* Send out the packet. */
	res = WritePacket( packet, iTimeOut_ms );
	
	if( ! res )
	{
		throw IOError("Failed to write a packet in ReadAndWrite");
		return( false );
	}
	
	/* Tries loop for receiving data */
	while( iTries >  0 )
	{
		res = ReadPacket( packet, iTimeOut_ms );
		if( res == true )
		{
			/* Got a packet then exit the loop */
			break;
		}
	}
	
	/* At this point, "res" already contain the result from the last packet reading */
	return( res );
}
/*====================================================================*/

/*=============== Helper functions only general used ================*/
bool imu_3dm_gx4::Ping()
{
	MIPPacket p( CLASS_CMD_BASE );
	std::vector<MIPPacket::Payload_t> payload;
	
	p.StartNewPayloadField( CMD_PING );
	if( ! (WriteAndReadPacket( p ) ) )
	{
		/* We received nothing back from ping */
		return( false );
	}
	
	p.DistributePayload( payload );
	if( ( payload[0][1] != FIELD_ACK_NACK ) && 
		( payload[0][2] != CMD_PING ) && 
		( payload[0][1] != 0 ) )
	{
		/* we have got error response */
		return( false );
	}
	
	return( true );
}
/*====================================================================*/

bool imu_3dm_gx4::SetToIdle()
{
	MIPPacket p( CLASS_CMD_BASE );
	std::vector<MIPPacket::Payload_t> payload;
	
	p.StartNewPayloadField( CMD_IDLE );
	if( ! (WriteAndReadPacket( p ) ) )
	{
		/* We received nothing back from ping */
		return( false );
	}
	
	p.DistributePayload( payload );
	if( ( payload[0][1] != FIELD_ACK_NACK ) && 
		( payload[0][2] != CMD_IDLE ) && 
		( payload[0][1] != 0 ) )
	{
		/* we have got error response */
		return( false );
	}
	
	return( true );
}
/*====================================================================*/

bool imu_3dm_gx4::ResetDevice()
{
	MIPPacket p( CLASS_CMD_BASE );
	std::vector<MIPPacket::Payload_t> payload;
	
	p.StartNewPayloadField( CMD_RESET );
	if( ! (WriteAndReadPacket( p ) ) )
	{
		/* We received nothing back from ping */
		return( false );
	}
	
	p.DistributePayload( payload );
	if( ( payload[0][1] != FIELD_ACK_NACK ) && 
		( payload[0][2] != CMD_RESET ) && 
		( payload[0][1] != 0 ) )
	{
		/* we have got error response */
		return( false );
	}
	
	return( true );
}
/*====================================================================*/

bool imu_3dm_gx4::ResumeDevice()
{
	MIPPacket p( CLASS_CMD_BASE );
	std::vector<MIPPacket::Payload_t> payload;
	
	p.StartNewPayloadField( CMD_RESUME );
	if( ! (WriteAndReadPacket( p ) ) )
	{
		/* We received nothing back from ping */
		return( false );
	}
	
	p.DistributePayload( payload );
	if( ( payload[0][1] != FIELD_ACK_NACK ) && 
		( payload[0][2] != CMD_RESUME ) && 
		( payload[0][1] != 0 ) )
	{
		/* we have got error response */
		return( false );
	}
	
	return( true );
}
/*====================================================================*/

bool imu_3dm_gx4::InitializeFilterwithMagneto()
{
	MIPPacket p( CLASS_CMD_EF );
	std::vector<MIPPacket::Payload_t> payload;
	
	p.StartNewPayloadField( CMD_INITIAL_ATTITUDE_WITH_MAGNETOMETER );
	p.AppendPayload( float(0) );
	
	if( ! (WriteAndReadPacket( p ) ) )
	{
		/* We received nothing back from ping */
		return( false );
	}
	
	p.DistributePayload( payload );
	if( ( payload[0][1] != FIELD_ACK_NACK ) && 
		( payload[0][2] != CMD_INITIAL_ATTITUDE_WITH_MAGNETOMETER ) && 
		( payload[0][1] != 0 ) )
	{
		/* we have got error response */
		return( false );
	}
	
	return( true );
}
/*====================================================================*/

/*=============== Helper functions only for Zeabus ================*/
bool imu_3dm_gx4::SetIMUDataFormat( uint16_t rateDecimation ) /* Actual rate = 100Hz / rateDecimation */
{
	std::vector<MIPPacket::Payload_t> payload;
	MIPPacket p( CLASS_CMD_3DM );
  
	p.StartNewPayloadField( CMD_IMU_MESSAGE_FORMAT );
	p.AppendPayload( FUNCTION_APPLY );
	p.AppendPayload( (uint8_t)3 );
	p.AppendPayload( FIELD_IMU_SCALED_ACCELEROMETER );
    p.AppendPayload( rateDecimation );
	p.AppendPayload( FIELD_IMU_SCALED_GYRO );
    p.AppendPayload( rateDecimation );
	p.AppendPayload( FIELD_IMU_CF_QUATERNION );
    p.AppendPayload( rateDecimation );

	if( ! (WriteAndReadPacket( p ) ) )
	{
		/* We received nothing back from ping */
		return( false );
	}
	
	p.DistributePayload( payload );
	if( ( payload[0][1] != FIELD_ACK_NACK ) && 
		( payload[0][2] != CMD_INITIAL_ATTITUDE_WITH_MAGNETOMETER ) && 
		( payload[0][1] != 0 ) )
	{
		/* we have got error response */
		return( false );
	}
	
	return( true );
}
/*====================================================================*/

bool imu_3dm_gx4::EnableDataStream()
{
	std::vector<MIPPacket::Payload_t> payload;
	MIPPacket p(CLASS_CMD_3DM);

    p.StartNewPayloadField( CMD_ENABLE_DATA_STREAM );
    p.AppendPayload( FUNCTION_APPLY );
    p.AppendPayload( (uint8_t)1 );	/* Enable only IMU data (1 = IMU, 2 = GPS, 3 = EF)*/
    p.AppendPayload( (uint8_t)1 );

	if( ! (WriteAndReadPacket( p ) ) )
	{
		/* We received nothing back from ping */
		return( false );
	}
	
	p.DistributePayload( payload );
	if( ( payload[0][1] != FIELD_ACK_NACK ) && 
		( payload[0][2] != CMD_INITIAL_ATTITUDE_WITH_MAGNETOMETER ) && 
		( payload[0][1] != 0 ) )
	{
		/* we have got error response */
		return( false );
	}
	
	return( true );
}
/*====================================================================*/
