/*
 * mip.h
 *
 *  Created on: Nov 18, 2014
 *      Author: mahisorn
 *	Upgraded on: May 14, 2017
 * 		Author: Akrapong Patchararungruang
 */

#ifndef IMU_3DM_GX4_45_MIP_H
#define IMU_3DM_GX4_45_MIP_H

#include <termios.h>
#include <assert.h>

#include <vector>
#include <array>
#include <iterator>
#include <algorithm>
#include <sstream>

namespace Zeabus_Elec
{

	/* List of IMU class codes and command codes */

	/* Command and data class codes */
	static const uint8_t CLASS_CMD_BASE = 0x01;
	static const uint8_t CLASS_CMD_3DM = 0x0C;
	static const uint8_t CLASS_CMD_EF = 0x0D;
	static const uint8_t CLASS_CMD_SYSTEM = 0x7F;

	static const uint8_t CLASS_DATA_IMU = 0x80;
	static const uint8_t CLASS_DATA_GPS = 0x81;
	static const uint8_t CLASS_DATA_EF = 0x82;
	
	/* Generic codes (belong to any class */
	static const uint8_t FUNCTION_APPLY = 0x01;
	static const uint8_t FIELD_ACK_NACK = 0xF1;
	
	/* Base commands */
	static const uint8_t CMD_PING = 0x01;
	static const uint8_t CMD_IDLE = 0x02;
	static const uint8_t CMD_GET_DEVICE_INFORMATION = 0x03;
	static const uint8_t CMD_GET_DEVICE_DESCRIPTION = 0x04;
	static const uint8_t CMD_BUILD_IN_TEST = 0x05;
	static const uint8_t CMD_RESUME = 0x06;
	static const uint8_t CMD_GPS_TIME_UPDATE = 0x72;
	static const uint8_t CMD_RESET = 0x7E;

	//3DM commands
	static const uint8_t CMD_POLL_IMU_DATA = 0x01;
	static const uint8_t CMD_POLL_GPS_DATA = 0x02;
	static const uint8_t CMD_POLL_EF_DATA = 0x03;
	static const uint8_t CMD_IMU_MESSAGE_FORMAT = 0x08;
	static const uint8_t CMD_GPS_MESSAGE_FORMAT = 0x09;
	static const uint8_t CMD_EF_MESSAGE_FORMAT = 0x0A;
	static const uint8_t CMD_UART_BAUD_RATE = 0x40;
	static const uint8_t CMD_ENABLE_DATA_STREAM = 0x11;
	static const uint8_t CMD_CF_SETTINGS = 0x51;

	//EF commands
	static const uint8_t CMD_RESET_EF = 0x01;
	static const uint8_t CMD_SET_INITIAL_ATTITUDE = 0x02;
	static const uint8_t CMD_SET_INITIAL_HEADING = 0x03;

	/*
	 * This command is not available in 45 data sheet.
	 * But it is mentioned in 45 example and available in
	 * 25 data sheet. (A useful initialization command)
	 */
	static const uint8_t CMD_INITIAL_ATTITUDE_WITH_MAGNETOMETER = 0x04;


	//IMU Data field
	static const uint8_t FIELD_IMU_SCALED_ACCELEROMETER = 0x04;
	static const uint8_t FIELD_IMU_SCALED_GYRO = 0x05;
	static const uint8_t FIELD_IMU_SCALED_MAGNETO = 0x06;
	static const uint8_t FIELD_IMU_SCALED_PRESSURE = 0x17;
	static const uint8_t FIELD_IMU_DELTA_THETA = 0x07;
	static const uint8_t FIELD_IMU_DELTA_VELOCITY = 0x08;

	static const uint8_t FIELD_IMU_CF_ORIENTATION_MATRIX = 0x09;
	static const uint8_t FIELD_IMU_CF_QUATERNION = 0x0A;
	static const uint8_t FIELD_IMU_CF_EULAR_ANGLES = 0x0C;
	static const uint8_t FIELD_IMU_CF_STABILIZED_MAG_VECTOR = 0x10;
	static const uint8_t FIELD_IMU_CF_STABILIZED_ACCEL_VECTOR = 0x11;

	static const uint8_t FIELD_IMU_GPS_CORRELATION_TIMESTAMP = 0x12;

	//GPS Data field
	static const uint8_t FIELD_GPS_LLH_POSITION = 0x03;
	static const uint8_t FIELD_GPS_ECEF_POSITION = 0x04;
	static const uint8_t FIELD_GPS_NED_VELOCITY = 0x05;
	static const uint8_t FIELD_GPS_ECEF_VELOCITY = 0x06;
	static const uint8_t FIELD_GPS_DOP_DATA = 0x07;

	static const uint8_t FIELD_GPS_UTC_TIME = 0x08;
	static const uint8_t FIELD_GPS_TIME = 0x09;
	static const uint8_t FIELD_GPS_CLOCK_INFORMATION = 0x0A;
	static const uint8_t FIELD_GPS_FIX_INFORMATION = 0x0B;
	static const uint8_t FIELD_GPS_SPACE_VEHICLE_INFORMATION = 0x0C;

	static const uint8_t FIELD_GPS_HARDWARE_STATUS = 0x0D;
	static const uint8_t FIELD_DGPS_INFORMATION = 0x0E;
	static const uint8_t FIELD_DGPS_CHANNEL_STATUS = 0x0F;

	//EF Data field
	static const uint8_t FIELD_EF_FILTER_STATUS = 0x10;
	static const uint8_t FIELD_EF_GPS_TIMESTAMP = 0x11;
	static const uint8_t FIELD_EF_LLH_POSITION = 0x01;
	static const uint8_t FIELD_EF_NED_VELOCITY = 0x02;
	static const uint8_t FIELD_EF_ORIENTATION_QUATERNION = 0x03;

	static const uint8_t FIELD_EF_ORIENTATION_MATRIX = 0x04;
	static const uint8_t FIELD_EF_ORIENTATION_EULER = 0x05;
	static const uint8_t FIELD_EF_GYRO_BIAS = 0x06;
	static const uint8_t FIELD_EF_ACCEL_BIAS = 0x07;
	static const uint8_t FIELD_EF_LLH_POSITION_UNCERTAINTY = 0x08;

	static const uint8_t FIELD_EF_NED_VELOCITY_UNCERTAINTY = 0x09;
	static const uint8_t FIELD_EF_ALTITUDE_UNCERTAINTY = 0x0A;
	static const uint8_t FIELD_EF_GYRO_BIAS_UNCERTAINTY = 0x0B;
	static const uint8_t FIELD_EF_ACCEL_BIAS_UNCERTAINTY = 0x0C;
	static const uint8_t FIELD_EF_LINEAER_ACCELERATION = 0x0D;

	static const uint8_t FIELD_EF_COMPENSATED_ACCELERATION = 0x1C;
	static const uint8_t FIELD_EF_COMPENSATED_ANGULAR_RATE = 0x0E;
	static const uint8_t FIELD_EF_WGS84_LOCAL_GRAVITY_MAGNITUDE = 0x0F;
	static const uint8_t FIELD_EF_ALTITUDE_UNCERTAINTY_QUATERNION_ELEMENT = 0x12;
	static const uint8_t FIELD_EF_GRAVITY_VECTOR = 0x13;

	static const uint8_t FIELD_EF_HEADING_UPDATE_SOURCE_STATE = 0x14;
	static const uint8_t FIELD_EF_MAGNETIC_MODEL_SOLUTION = 0x15;
	static const uint8_t FIELD_EF_GYRO_SCALE_FACTOR = 0x16;
	static const uint8_t FIELD_EF_ACCEL_SCALE_FACTOR = 0x17;
	static const uint8_t FIELD_EF_GYRO_SCALE_FACTOR_UNCERTAINTY = 0x18;

	static const uint8_t FIELD_EF_ACCEL_SCALE_FACTOR_UNCERTAINTY = 0x19;
	static const uint8_t FIELD_EF_STANDARD_ATMOSPHERE_MODEL = 0x20;
	static const uint8_t FIELD_EF_PRESSURE_ALTITUDE = 0x21;
	static const uint8_t FIELD_EF_GPS_ANTENNA_OFFSET_CORRECTION = 0x30;
	static const uint8_t FIELD_EF_GPS_ANTENNA_OFFSET_CORRECTION_UNCERTAINTY = 0x31;

	const uint8_t SYNC_STR[] = { 0x75, 0x65 };

	struct IMUData
	{
		float scaled_accelerometer[3];
		float scaled_gyro[3];
		float orientation_quaternion[4];
	};

	/*=============== Class for managing the IMU packet_ ==============*/
	class MIPPacket
	{
	public:
		typedef std::vector<uint8_t> Packet_t;
		typedef std::vector<uint8_t> Payload_t;
	
	protected:
		 Packet_t packet_;
		 int nextPayloadHead_, nextPayloadPosition_;
	
	public:
		static const auto HEADER_LENGTH = 4;
		static const auto CHECKSUM_LENGTH = 2;
		static const auto PACKET_MIN_LENGTH = HEADER_LENGTH + CHECKSUM_LENGTH;
		static const auto MAX_PAYLOAD = 255;
		static const auto MAX_PACKET_SIZE = PACKET_MIN_LENGTH + MAX_PAYLOAD;
		static const uint8_t SYNC1 = 0x75;
		static const uint8_t SYNC2 = 0x65;

		Payload_t payload_;

		/*========================================================================*/
		/* Main methods for packet_ manipulation.                                   */
		/*========================================================================*/
		MIPPacket( uint8_t ucDescriptor = 0 )
			: packet_(PACKET_MIN_LENGTH, 0)
		{
			ResetPacket( ucDescriptor );
		};
	
		MIPPacket( const MIPPacket& sPacket )
			: packet_( sPacket.packet_ ), payload_( sPacket.payload_ ),
			  nextPayloadHead_( sPacket.nextPayloadHead_ ), 
			  nextPayloadPosition_( sPacket.nextPayloadPosition_ )
		{
		};
	
		MIPPacket( uint8_t sStream, uint32_t sSize )
			: packet_( sStream, sSize )
		{
			ExtractPayload();
		};

		/*========================================================================*/
		/* Main methods for packet_ manipulation.                                   */
		/*========================================================================*/

		/* Reset all content of the packet_ and  initialize with SYNC data + descriptor value */
		void ResetPacket( uint8_t ucDescriptor = 0, uint16_t size = PACKET_MIN_LENGTH )
		{
			packet_.resize( PACKET_MIN_LENGTH );
			packet_[0] = SYNC1;
			packet_[1] = SYNC2;
			packet_[2] = ucDescriptor;
			packet_[3] = 0;	/* Initialize the length with 0 */
			ResetPayload( );
		};
	
		/* Reset the payload_ content */
		void ResetPayload()
		{
			/* Clear all payload data */
			payload_.clear();
			nextPayloadPosition_ = 0;
			nextPayloadHead_ = 0;
		};
		
		uint8_t GetPacketDescriptor()
		{
			return packet_[2];
		}
		
		uint8_t GetPacketLen()
		{
			return packet_[3];
		}

		/* Generate packet_ from data stream */
		void GenPacket( Packet_t::iterator begin, Packet_t::iterator end )
		{
			packet_.clear();
			packet_.reserve( end - begin );
			packet_.insert( packet_.end(), begin, end );
			
			ExtractPayload();
		}
	
		/* Calculate Fletcher checksum of the packet_ */
		std::array<uint8_t, 2> CalculateChecksum ()
		{
			std::array<uint8_t, 2>checksum = {0, 0};
		
			/* Use for_each and lambda to calculate checksum */	
			std::for_each( packet_.begin() , packet_.end() - 2 , [ &checksum ]( uint8_t x ) {
				checksum[0] += x;
				checksum[1] += checksum[0];
			} );
		
			return( checksum );
		};
	
		/* Verify the packet_ against the checksum stored inside itself */
		bool VerifyChecksum()
		{
			Packet_t::reverse_iterator rit = packet_.rbegin();
			std::array<uint8_t, 2>checksum = CalculateChecksum();
		
			if( ( *rit == checksum[ 1 ] ) && ( *( rit + 1 ) == checksum[ 0 ] ) )	
			{
				return( true );
			}
			else
			{
				return( false );
			}	
		};
	
		/* Embed the check sum of the packet_ into the packet_ itself */
		void EmbedChecksum()
		{
			std::array<uint8_t, 2>checksum;
			Packet_t::reverse_iterator rit = packet_.rbegin();
		
			/* Calculate the checksum */
			checksum = CalculateChecksum();
			
			/* Embed the checksum into the packet_ through a reverse iterator (iterate in reverse order) */
			*rit = checksum[1];
			rit--;
			*rit = checksum[0];
		};
	
		/* Embed the payload_ into the packet_ */
		void EmbedPayload()
		{
			/* Resize the packet_ to include the payload_ size */
			packet_.resize( payload_.size() + PACKET_MIN_LENGTH );
		
			/* Append the payload_ */
			packet_.insert( packet_.begin() + 4, payload_.begin(), payload_.end() );
		};
	
		/* Extract the payload_ from the packet_ */
		void ExtractPayload()
		{
			auto payloadSize = packet_[3];
			
			assert( payloadSize == ( packet_.size() - PACKET_MIN_LENGTH ) );
			/* Resize the payload_ to the size of payload_ field inside the packet_ */
			payload_.resize( payloadSize );
		
			/* Extract the payload_ */
			payload_.insert( payload_.begin(), packet_.begin() + 4, packet_.end() - 2 );
			
			/* Set next payload position */
			nextPayloadHead_ = payload_.size();
			nextPayloadPosition_ = nextPayloadHead_;
		};
	
		/* A packet_ may contain more than 1 payload_ field. This method extract each 
		   payload_ field into a vector of fields */
		void DistributePayload( std::vector<Payload_t>& x_payloads)
		{
			auto ucAvailableLen = packet_[3];
			Payload_t::iterator payloadIterator = payload_.begin();
			auto i = 0, fieldLen = 0;
		
			while( ucAvailableLen > 0 )
			{
				/* Copy each payload_ field */
				fieldLen = *payloadIterator;
				x_payloads[i].clear();
				x_payloads[i].reserve( fieldLen );
				x_payloads[i].insert( x_payloads[i].end(), payloadIterator, payloadIterator + fieldLen );
			
				/* Modify the indexes for the next item */
				payloadIterator += fieldLen;
				ucAvailableLen -= fieldLen;
			}
		};
	
		/* Start new payload field */
		void StartNewPayloadField( uint8_t description )
  		{
  			/* Reset payload head pointed to "length" to the end of the payload to create a new one */
  			nextPayloadHead_ = nextPayloadPosition_;
			
			AppendPayload( ( uint8_t )description );
		};

		/* Append a datum of a primitive type to the payload_ */
	template<typename T>
			void AppendPayload(const T& t)
		{
		
			uint8_t* p = (uint8_t*)(&t);
			
			if( nextPayloadHead_ == nextPayloadPosition_ )
			{
				/* The next payload is still empty. Therefore, we initialize it with len field */
				payload_[ nextPayloadHead_ ] = 1;
				nextPayloadPosition_++;
			}
			
	#ifdef HOST_LITTLE_ENDIAN
			p += (sizeof(t) - 1);
	#endif
			for (int i = 0; i < sizeof(t); i++)
			{
	#ifdef HOST_LITTLE_ENDIAN
				payload_[ nextPayloadPosition_ ] = *( p - i );
	#else
				payload_[ nextPayloadPosition_ ] = *( p + i );
	#endif
				nextPayloadPosition_++;
				( payload_[ nextPayloadHead_ ] )++;
			}
		};

		/* extract a datum of a primitive type from the payload_ */
		template<typename T>
			T extract(uint32_t ulPosition = 0)
		{
    		T result;
    		uint8_t* p = reinterpret_cast<uint8_t*>( &result );

	#ifdef HOST_LITTLE_ENDIAN
			p += ( sizeof( T ) - 1 );
	#endif
			for (int i = 0; i < sizeof( T ); i++)
			{
	#ifdef HOST_LITTLE_ENDIAN
				*( p - i ) = payload_[ ulPosition + i ];
	#else
				*( p + i ) = payload_[ ulPosition + i ];
	#endif
			}
	    };

		/*========================================================================*/
		/* Auxiliary methods for packet_ manipulation.                                   */
		/*========================================================================*/
	
		/* Translate packet_ data into a debug string */
		std::string ToString() const
		{
			std::stringstream ss;
		
			ss << std::hex;
			ss << "\n======= Packet =======\n";
			ss << "Sync MSB: " << static_cast<int>( packet_[0] ) << "\n";
			ss << "Sync LSB: " << static_cast<int>( packet_[1] ) << "\n";
			ss << "Descriptor: " << static_cast<int>( packet_[2] ) << "\n";
			ss << "Length: " << static_cast<int>( packet_[3] ) << "\n";
			ss << "Payload: ";
			for( auto& s : payload_ )
			{
				ss << static_cast<int>( s ) << " ";
			}
			ss << "\nCRC MSB: " << static_cast<int>( packet_[ packet_.size() - 2 ] ) << "\n";
			ss << "CRC LSB: " << static_cast<int>( packet_[ packet_.size() - 1 ] ) << "\n";
			ss << "======================\n";
		
			return( ss.str() );
		};

		/* Return full packet_ with payload_ and check-sum */
		Packet_t& GetPacket()
		{
			EmbedPayload();
			EmbedChecksum();
		
			return( packet_ );
		};		
	}; /* class MIPPacket */

} /* Name space */

#endif /* IMU_3DM_GX4_45_MIP_H */
