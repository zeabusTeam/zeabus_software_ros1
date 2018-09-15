/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: imu_communication.cpp
//
//	Edit	: Aug 15 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Nov 21 , 2014
//			: mip.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// for information about imu communication command please read
// 3DM-gx4-45 Data Communication Protocal

namespace zeabus_sensor{

	namespace 3dm_gx4_45{

//Command and data class
		static const uint8_t CMD_CLASS_BASE = 0x01;
		static const uint8_t CMD_CLASS_3DM = 0x0C;
		static const uint8_t CMD_CLASS_EF = 0x0D;
		static const uint8_t CMD_CLASS_SYSTEM = 0x7F;

		static const uint8_t DATA_CLASS_IMU = 0x80;
		static const uint8_t DATA_CLASS_GPS = 0x81;
		static const uint8_t DATA_CLASS_EF = 0x82;
		
		static const uint8_t SELECT_IMU = 0x01;
		static const uint8_t SELECT_GPS = 0x02;
		static const uint8_t SELECT_EF = 0x03;
		
		static const uint8_t FUNCTION_APPLY = 0x01;
		static const uint8_t FIELD_ACK_NACK = 0xF1;
		
		static const uint8_t DATA_STREAM_ENABLE = 0x01;
		static const uint8_t DATA_STREAM_DISABLE = 0x00;


//Base commands
		static const uint8_t CMD_PING = 0x01;
		static const uint8_t CMD_IDLE = 0x02;
		static const uint8_t CMD_RESUME = 0x06;
		static const uint8_t CMD_GPS_TIME_UPDATE = 0x72;
		static const uint8_t CMD_GET_DEVICE_INFORMATION = 0x03;
		static const uint8_t CMD_GET_DEVICE_DESCRIPTION = 0x04;
		static const uint8_t CMD_BUILD_IN_TEST = 0x05;
		static const uint8_t CMD_RESET = 0x7E;

//3DM commands
		static const uint8_t CMD_IMU_MESSAGE_FORMAT = 0x08;
		static const uint8_t CMD_GPS_MESSAGE_FORMAT = 0x09;
		static const uint8_t CMD_EF_MESSAGE_FORMAT = 0x0A;
		static const uint8_t CMD_UART_BAUD_RATE = 0x40;
		static const uint8_t CMD_ENABLE_DATA_STREAM = 0x11;
		static const uint8_t CMD_CF_SETTINGS = 0x51;
		
//EF commands
		
		
		static const uint8_t CMD_SET_INITIAL_ATTITUDE = 0x02;
		
/*
 * This command is not available in 45 data sheet.
 * But it is mentioned in 45 example and available in
 * 25 data sheet.
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
		
		
		struct IMU_DATA{
			enum{
				SCALED_ACCELEROMETER = (1 << 0),
				SCALED_GYRO = (1 << 1),
				SCALED_MAGNETO = (1 << 2),
				SCALED_PRESSURE = (1 << 3),
				DELTA_THETA = (1 << 4),
				DELTA_VELOCITY = (1 << 5),
		
				CF_ORIENTATION_MATRIX = (1 << 6),
				CF_QUATERNION = (1 << 7),
				CF_EULAR_ANGLES = (1 << 8),
				CF_STABILIZED_MAG_VECTOR = (1 << 9),
				CF_STABILIZED_ACCEL_VECTOR = (1 << 10),
		
				GPS_CORRELATION_TIMESTAMP = (1 << 11),
		
				NUM_IMU_DATA = 12
			};
		
			uint32_t fields;
			float scaled_accelerometer[3];
			float scaled_gyro[3];
			float scaled_magneto[3];
			float orientation_quaternion[4];
		
			imu_data() : fields(0){}
		};

		struct DATA_STREAM{
			enum{
				IMU_DATA = (1 << 0),
				GPS_DATA = (1 << 1),
				 EF_DATA = (1 << 2)
			};
		};

		class MIP : public zeabus_serial::packet{
			public:
				static const uint8_t HEADER_LENGTH = 4;
				static const uint8_t SYNC1 = 0x75;
				static const uint8_t SUNC2 = 0x65;
				static const uint8_t MAX_PAYLOAD = 255;

				union{
					struct{
						uint8_t sync1;
						uint8_t sync2;
					};
					uint16_t sync;
				};

				uint8_t descriptor;
				uint8_t length;
				std::vector<uint8_t> pay_load;

				union{
					struct{
						uint8_t crc16_msb;
						uint8_t crc16_lsb;
					};
					uint16_t crc16;
				}

				MIP( uint8_t desc = 0 ): sync1(SYNC1) , sync2(SYNC2) , descriptor( desc )
									,	crc16(0) , payload( MAX_PAYLOAD , 0 )
									,	field_length_position(0) , message_position(2)
									,	is_encoding( false ){}

				void reset();

				void begin_field( uint8_t description);

				template<typename T>void append( const T& t);

				void end_field();

				int get_filed_descriptor();
		}
	}
}
