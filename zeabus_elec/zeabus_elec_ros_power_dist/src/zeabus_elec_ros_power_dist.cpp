#include <ros/ros.h>
#include <std_msgs/String.h>
#include <zeabus_elec_ros_power_dist/power_dist.h>
#include <ftdi_impl.h>

#include <memory>
#include <sstream>

/* ===================================================
 * File-scope global variables
 * ===================================================
 */
static const char* cstPowerDistSerial = "PowerDist";

static std::shared_ptr<Zeabus_Elec::ftdi_mpsse_impl> pxMssp;
static ros::Publisher errMsgPublisher;	/* Publisher for error message */
static ros::ServiceServer switchServiceServer; /* Service server for switch-controlling */

/* ===================================================
 * ROS service subroutines
 * ===================================================
 */
bool ZeabusElec_SetSwitch( zeabus_elec_ros_power_dist::power_dist::Request &req,
                            zeabus_elec_ros_power_dist::power_dist::Response &res )
{
	int ftStat;
        uint8_t switchState, currentSwitchState, switchMask;

        res.result = true;

        switchMask = 0x01 << ( req.switchIndex );

        currentSwitchState = pxMssp->ReadHiGPIOData();

        if( req.isSwitchHigh )
        {
            switchState = ( currentSwitchState | switchMask );
        }
        else
        {
            switchState = ( currentSwitchState & ~( switchMask ) );
        }
	
	ftStat = pxMssp->SetHiGPIOData( switchState );
	if( ftStat != 0 )
	{
		/* Some Error occurred. So, we publish the error message */
		std_msgs::String msg;
		std::stringstream ss;
		ss << "Error in power-distributor controller with the code " << ftStat;
		msg.data = ss.str();
		
		errMsgPublisher.publish( msg );

                res.result = false;
	}

        return res.result;
}

/* ===================================================
 * main
 * ===================================================
 */
int main( int argc, char** argv )
{
        int paramInitIODirection, paramInitIOPinState;
        uint16_t initIODirection, initIOPinState;

	/* Initialize ROS functionalities */	
	ros::init(argc, argv, "Zeabus_Elec_Power_dist");
 	ros::NodeHandle nh("/zeabus/elec");

        /* Retrieve parameter from launch file */
        nh.param < int > ("/Zeabus_Elec_Power_dist/IODirection", paramInitIODirection, 0xFFFF);
        nh.param < int > ("/Zeabus_Elec_Power_dist/IOPinState", paramInitIOPinState, 0x0000);

        /* cast int to uint16_t because NodeHandle::param doesn't support uint16_t */
        initIODirection = static_cast<uint16_t>(paramInitIODirection);
        initIOPinState = static_cast<uint16_t>(paramInitIOPinState);

	/*=================================================================================
	  Discover the Power Distributor and also open handles for it.
	  =================================================================================*/

	/* Create the device manager class to implement chip functions */
	pxMssp = std::make_shared<Zeabus_Elec::ftdi_mpsse_impl>( Zeabus_Elec::FT232H, cstPowerDistSerial );
	
	if( pxMssp->GetCurrentStatus() != 0 )
	{
		/* Fail - unable to initialize Power Distribution module */
		ROS_FATAL( "Unable to initialize Power Distribution module" );
		return( -1 );
		
	}
	
	/* Set GPIO direction and pin intial state to all output, bit=1 means output, 0 means input */
	pxMssp->SetGPIODirection( initIODirection , initIOPinState );	/* All bits are output, initial pin state is low */
	if( pxMssp->GetCurrentStatus() != 0 )
	{
		/* Fail - unable to initialize Power Distribution module */
		ROS_FATAL( "Unable to initialize set-up GPIO direction" );
		return( -1 );
		
	}
	
	/*=================================================================================
	  Now the FTDI chip is opened and hooked. We can continue ROS registration process 
	  =================================================================================*/
	
	/* Register ROS publisher to Elec/Hw_error topic */
	errMsgPublisher = nh.advertise<std_msgs::String>( "hw_error", 1000 );
	/* Register ROS service server to Elec/Power_switch topic */
	switchServiceServer = nh.advertiseService( "power_switch", ZeabusElec_SetSwitch );

	/* Main-loop. Just a spin-lock */
	ros::spin();
	
	/*=================================================================================
	  At this point of code. ROS has some fatal errors or just normal shutdown. Also us. 
	  =================================================================================*/
	return( 0 );
}
