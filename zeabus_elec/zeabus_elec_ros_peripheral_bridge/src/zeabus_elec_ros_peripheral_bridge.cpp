#include <ros/ros.h>
#include <std_msgs/String.h>
#include <zeabus_elec_ros_peripheral_bridge/barometer.h>
#include <zeabus_elec_ros_peripheral_bridge/solenoid_sw.h>
#include <zeabus_elec_ros_peripheral_bridge/ios_state.h>
#include <zeabus_elec_ros_peripheral_bridge/comm_data.h>
#include <ftdi_impl.h>

#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <memory>

/* ===================================================
 * File-scope global variables
 * ===================================================
 */
static const char* stPeripheralSerial = "PeripheralBridge";

static std::shared_ptr<Zeabus_Elec::ftdi_impl> pxMsspA, pxMsspB, pxUartA, pxUartB;
static ros::Publisher errMsgPublisher;      /* Publisher for error message */
static ros::Publisher barometerPublisher;	/* Publisher for barometer message */
static ros::Publisher iosStatePublisher;     /* Publisher for IOs state message*/
static ros::Publisher comm1RecvPublisher;	/* Publisher for comm1 received-data message */
static ros::Publisher comm2RecvPublisher;	/* Publisher for comm2 received-data message */
static ros::Subscriber comm1SendSubscriber; /* Subscriber for comm1 sent-data message */
static ros::Subscriber comm2SendSubscriber; /* Subscriber for comm2 sent-data message */
static ros::ServiceServer setSolenoidServiceServer; /* ServiceServer for solenoid-controlling */

/* ===================================================
 * ROS service subroutines
 * ===================================================
 */

/* Set the solenoid switches. We have maximum 8 switches. The low-nibble 4 switches 
is controlled by MSSP_A and the other 4 switches is controlled by MSSP_B */
bool ZeabusElec_SetSolenoid( zeabus_elec_ros_peripheral_bridge::solenoid_sw::Request &req,
                            zeabus_elec_ros_peripheral_bridge::solenoid_sw::Response &res )
{
	uint8_t swNibble, switchState, currentSwitchState, switchMask;
	int ftStat;

        res.result = true;

        switchMask = 0x01 << ( req.switchIndex );

        currentSwitchState = pxMsspA->ReadLoGPIOData() >> 4;
        currentSwitchState |= pxMsspB->ReadLoGPIOData() & 0xF0;

        if( req.isSwitchHigh )
        {
            switchState = ( currentSwitchState | switchMask );
        }
        else
        {
            switchState = ( currentSwitchState & ~( switchMask ) );
        }

	/* Low nibble */
        swNibble = ( switchState << 4 );
	ftStat = pxMsspA->SetLoGPIOData( swNibble );

	if( ftStat != 0 )
	{
		/* Some Error occurred. So, we publish the error message */
		std_msgs::String msg;
		std::stringstream ss;
		ss << "Error in solenoid controller with the code " << ftStat;
		msg.data = ss.str();
		
		errMsgPublisher.publish( msg );

                res.result = false;
	}

	/* High nibble */
	swNibble = ( switchState & 0xF0 );
	ftStat = pxMsspB->SetLoGPIOData( swNibble );
	if( ftStat != 0 )
	{
		/* Some Error occurred. So, we publish the error message */
		std_msgs::String msg;
		std::stringstream ss;
		ss << "Error in solenoid controller with the code " << ftStat;
		msg.data = ss.str();
		
		errMsgPublisher.publish( msg );

                res.result = false;
	}
        return res.result;
}

/* Send a stream of uint8 to RS232 port 1 */
void ZeabusElec_SendComm1( const zeabus_elec_ros_peripheral_bridge::comm_data::ConstPtr& msg )
{
	std::vector<uint8_t> pucBuffer;
	uint32_t ulDataDone;
	
	pucBuffer.reserve( msg->len );
	for(uint32_t i = 0; i < msg->len; i++ )
	{
		pucBuffer.push_back( ( msg->data )[ i ] );
	}
	ulDataDone = pxUartA->Send( pucBuffer );

	if( ( pxUartA->GetCurrentStatus() != 0 ) || ( ulDataDone != msg->len ) )
	{
		/* Some Error occurred. So, we publish the error message */
		std_msgs::String eMsg;
		std::stringstream ss;
		ss << "Fail to send " << ( msg->len ) << " bytes to COMM1. Only " << ulDataDone << " are sent and error code is " << pxUartA->GetCurrentStatus();
		eMsg.data = ss.str();
		
		errMsgPublisher.publish( eMsg );
	}
}

/* Send a stream of uint8 to RS232 port 1 */
void ZeabusElec_SendComm2( const zeabus_elec_ros_peripheral_bridge::comm_data::ConstPtr& msg )
{
	std::vector<uint8_t> pucBuffer;
	uint32_t ulDataDone;
	
	pucBuffer.reserve( msg->len );
	for(uint32_t i = 0; i < msg->len; i++ )
	{
		pucBuffer.push_back( ( msg->data )[ i ] );
	}
	ulDataDone = pxUartB->Send( pucBuffer );
	
	if( ( pxUartB->GetCurrentStatus() != 0 ) || ( ulDataDone != msg->len ) )
	{
		/* Some Error occurred. So, we publish the error message */
		std_msgs::String eMsg;
		std::stringstream ss;
		ss << "Unable to send " << ( msg->len ) << " bytes to COMM2. Only " << ulDataDone << " are sent and error code is " << pxUartA->GetCurrentStatus();
		eMsg.data = ss.str();
		
		errMsgPublisher.publish( eMsg );
	}
}

/* Read barometer value. The barometer is connected by SPI protocol through MSSP_A */
bool ZeabusElec_GetPressure( uint16_t& barometerVal)
{
	std::vector<uint8_t> aucVal(2, 0); /* Data buffer. The raw data are in 2 bytes as a stream */
	int xFTStatus;
	
	/* To read barometer value, we need to send a 16-bit dummy data first */
	xFTStatus = pxMsspA->Send( aucVal );
	if( xFTStatus < 0 )
	{
		barometerVal = 0;
		return( false );
	}
	
	/* Read the actual 2-byte raw data. The actual data is 10 bits plus some unused bit as:
		   0 0 0 D D D D D  D D D D D x x x
		where the first byte is the MSB. */
	xFTStatus = pxMsspA->Receive( aucVal );
	if( xFTStatus < 0 )
	{
		barometerVal = 0;
		return( false );
	}
	
	/* Extract the data from 2-byte message */
	barometerVal = (uint16_t)( aucVal[ 0 ] ); /* High byte */
	barometerVal <<= 8;
	barometerVal |= (uint16_t)( aucVal[ 1 ] ); /* Low byte */
	barometerVal >>= 3;	/* Suppress 3 tailing zero bits */
        barometerVal >>= 1;     /* Shift right by 1 clock according to invalid first clock from command 0x31 */

        zeabus_elec_ros_peripheral_bridge::barometer barometerMsg;
        barometerMsg.pressureValue = barometerVal;

        barometerPublisher.publish( barometerMsg );

	return( true );	/* All success */	
}

/* Read IOs state. */
bool ZeabusElec_GetIOsState()
{
        uint8_t currentSwitchState;

        currentSwitchState = pxMsspA->ReadLoGPIOData() >> 4;
        currentSwitchState |= pxMsspB->ReadLoGPIOData() & 0xF0;

        zeabus_elec_ros_peripheral_bridge::ios_state iosStateMsg;
        iosStateMsg.iosState = currentSwitchState;

        iosStatePublisher.publish( iosStateMsg );

	return( true );	/* All success */	
}

/* Receive available data from either COMM1 or COMM2 */
void ZeabusElec_ReceiveComm( std::shared_ptr<Zeabus_Elec::ftdi_impl> commPort, uint8_t commID, ros::Publisher& commPublisher )
{
	std::vector<uint8_t> pucBuffer;
	uint32_t ulDataDone;

	/* Reading from the COMM port */
	ulDataDone = commPort->Receive( pucBuffer );
	if( commPort->GetCurrentStatus() <= 0 )
	{
		/* Reading finished with error */
		std_msgs::String msg;
		std::stringstream ss;
		ss << "Fail to receive data from COMM" << commID << " with error code " << commPort->GetCurrentStatus();
		msg.data = ss.str();
	
		errMsgPublisher.publish( msg );
	}
	else
	{
		/* Send the data to the topic channel */
		zeabus_elec_ros_peripheral_bridge::comm_data commMsg;

		for( uint32_t i = 0; i < pucBuffer.size(); i++ )
		{
			commMsg.data.push_back(pucBuffer[i]);
		}
		commMsg.len = ulDataDone;
		commPublisher.publish( commMsg );
	}
}

/* ===================================================
 * main
 * ===================================================
 */
int main( int argc, char** argv )
{
	uint16_t usBarometerVal, initIODirectionA, initIOStateA, initIODirectionB, initIOStateB;
	int comm1BaudRate, comm2BaudRate, paramInitIODirectionA, paramInitIOStateA, paramInitIODirectionB, paramInitIOStateB;

	/* Initialize ROS functionalities */	
	ros::init(argc, argv, "Zeabus_Elec_Peripheral_bridge");
 	ros::NodeHandle nh("/zeabus/elec");
 	
 	/* Retrieve parameter from launch file */
	nh.param < int > ("/Zeabus_Elec_Peripheral_bridge/comm1baudrate", comm1BaudRate, 115200);
	nh.param < int > ("/Zeabus_Elec_Peripheral_bridge/comm2baudrate", comm2BaudRate, 115200);
        nh.param < int > ("/Zeabus_Elec_Peripheral_bridge/IODirectionA", paramInitIODirectionA, 0xFFFF);
        nh.param < int > ("/Zeabus_Elec_Peripheral_bridge/IOStateA", paramInitIOStateA, 0x0000);
        nh.param < int > ("/Zeabus_Elec_Peripheral_bridge/IODirectionB", paramInitIODirectionB, 0xFFFF);
        nh.param < int > ("/Zeabus_Elec_Peripheral_bridge/IOStateB", paramInitIOStateB, 0x0000);

        /* cast int to uint16_t because NodeHandle::param doesn't support uint16_t */
        initIODirectionA = static_cast<uint16_t>(paramInitIODirectionA);
        initIOStateA = static_cast<uint16_t>(paramInitIOStateA);
        initIODirectionB = static_cast<uint16_t>(paramInitIODirectionB);
        initIOStateB = static_cast<uint16_t>(paramInitIOStateB);
	
	/*=================================================================================
	  Discover the Power Distributor and also open handles for it.
	  =================================================================================*/

	/* Attempt to open the Peripheral-Bridge modules described by their serial number */
	/* The module utilizes FT4232H consisting of 4 module instances inside */

	/* Create the device manager classes to implement chip functions */
	pxMsspA = std::make_shared<Zeabus_Elec::ftdi_spi_cpol1_cha0_msb_impl>( Zeabus_Elec::FT4232H, stPeripheralSerial, 1 );
	if( pxMsspA->GetCurrentStatus() != 0 )
	{
		/* Fail - unable to initialize pressure sensor and LOW solenoid switch interface*/
		ROS_FATAL( "Unable to initialize Pressure sensor and LOW Solenoid switch interface" );
		return( -5 );
		
	}
	pxMsspA->SetGPIODirection( initIODirectionA, initIOStateA );	/* All bits are output, initial pin state is low*/

	pxMsspB = std::make_shared<Zeabus_Elec::ftdi_spi_cpol1_cha0_msb_impl>( Zeabus_Elec::FT4232H, stPeripheralSerial, 2 );
	if( pxMsspB->GetCurrentStatus() != 0 )
	{
		/* Fail - unable to initialize HI solenoid switch interface*/
		ROS_FATAL( "Unable to initialize HI Solenoid switch interface" );
		return( -6 );
		
	}
	pxMsspB->SetGPIODirection( initIODirectionB, initIOStateB );	/* All bits are output, initial pin state is low*/
	
	//pxUartA = std::make_shared<Zeabus_Elec::ftdi_uart_impl>( Zeabus_Elec::FT4232H, stPeripheralSerial, 3, (uint32_t)comm1BaudRate );
	//if( pxUartA->GetCurrentStatus() != 0 )
	//{
	//	/* Fail - unable to initialize COM1 interface*/
	//	ROS_FATAL( "Unable to initialize COM1 interface" );
	//	return( -7 );
	//	
	//}

	//pxUartB = std::make_shared<Zeabus_Elec::ftdi_uart_impl>( Zeabus_Elec::FT4232H, stPeripheralSerial, 4, (uint32_t)comm2BaudRate );
	//if( pxUartB->GetCurrentStatus() != 0 )
	//{
	//	/* Fail - unable to initialize COM2 interface*/
	//	ROS_FATAL( "Unable to initialize COM2 interface" );
	//	return( -8 );
	//	
	//}

	/*=================================================================================
	  Now the FTDI chip is opened and hooked. We can continue ROS registration process 
	  =================================================================================*/
	
	/* Register ROS publishers for hardware error message, baromenter message and RS232 received-data message */
	errMsgPublisher = nh.advertise<std_msgs::String>( "hw_error", 1000 );	/* Publisher for error message */
	barometerPublisher = nh.advertise<zeabus_elec_ros_peripheral_bridge::barometer>( "barometer", 100 );	/* Publisher for barometer message */
        iosStatePublisher = nh.advertise<zeabus_elec_ros_peripheral_bridge::ios_state>( "ios_state", 100 );	/* Publisher for IOs state message */
	//comm1RecvPublisher = nh.advertise<zeabus_elec_ros_peripheral_bridge::comm_data>( "comm1/recv", 100 );	/* Publisher for comm1 received-data message */
	//comm2RecvPublisher = nh.advertise<zeabus_elec_ros_peripheral_bridge::comm_data>( "comm2/recv", 100 );	/* Publisher for comm2 received-data message */

	/* Register ROS subscribers for RS232 send-data message */
	//comm1SendSubscriber = nh.subscribe( "comm1/send", 100, ZeabusElec_SendComm1 ); /* Subscriber for comm1 sent-data message */
	//comm2SendSubscriber = nh.subscribe( "comm2/send", 100, ZeabusElec_SendComm2 ); /* Subscriber for comm2 sent-data message */

        /* Register ROS service server for solenoid switch controller */
	setSolenoidServiceServer = nh.advertiseService( "solenoid_sw", ZeabusElec_SetSolenoid ); /* Service server for solenoid-controlling */

	/* Main-loop. Just a spin-lock and wakeup at every 10ms (100Hz) */
	ros::Rate rate(100);

	while( ros::ok() )
	{
		/* Wait for the next cycle */
		rate.sleep();
		ros::spinOnce();
		
		/* Read barometer */
		if( !( ZeabusElec_GetPressure( usBarometerVal ) ) )
		{
			/* Fail to read from barometer */
			std_msgs::String msg;
			std::stringstream ss;
			ss << "Fail to read from barometer with error code " << pxMsspA->GetCurrentStatus();
			msg.data = ss.str();
		
			errMsgPublisher.publish( msg );
		}

                /* read IOs state */
                if( !(ZeabusElec_GetIOsState() ) )
                {
                        /* Fail to read IOs state */
			std_msgs::String msg;
			std::stringstream ss;
			ss << "Fail to get IOs state with error code " << pxMsspA->GetCurrentStatus();
			msg.data = ss.str();
		
			errMsgPublisher.publish( msg );
                }

		///* Read from COMM1 */
		//ZeabusElec_ReceiveComm( pxUartA, 1, comm1RecvPublisher );
		//	
		///* Read from COMM2 */
		//ZeabusElec_ReceiveComm( pxUartB, 2, comm2RecvPublisher );
	}
	
	/*=================================================================================
	  At this point of code. ROS has some fatal errors or just normal shutdown. Also us. 
	  =================================================================================*/
	return( 0 );
}
