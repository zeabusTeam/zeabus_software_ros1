/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: dvl_port.cpp
//
//	Edit	: Aug 25 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Dec 22 , 2014
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//#include	<zeabus_sensor/dvl_port.h>
#include	"dvl_port.h"

int main( int argc , char **argv){

	ros::init( argc , argv , "port_dvl"); // init node name

	ros::NodeHandle nh("~");

	zeabus_serial::serial serial; // setup variable for connect device

// set up variable for receivce param data
	std::string device;
	int baudrate;
	std::string frame_id;
	int maximum_bottom_search_depth;
	double heading_alignment;
	int salinity_water;
	std::string time_between_ensemble;
	std::string time_between_pings;
	int data_stream_select;

// get param data
	nh.param< std::string >("device" , device , "/dev/usb2serial/ftdi_FT03OMNT_03");
	nh.param< int >("baudrate" , baudrate , 115200 );
	nh.param< std::string >("frame_id" , frame_id , "dvl_link"); // what
	nh.param< float >("surface_depth" , surface_depth , 4.7);
	nh.param< int >("maximum_bottom_search_depth" , maximum_bottom_search_depth , 100);
	nh.param< double >("heading_alignment" ,  heading_alignment , 0.0);
	nh.param< int >("salinity_water" , salinity_water , 0);
	nh.param< std::string >("time_between_ensemble" , time_between_ensemble , "00:00:00.00");
	nh.param< std::string >("time_between_pings" , time_between_pings , "00:00.00");
	nh.param< int >("data_stream_select" , data_stream_select , 6);

// try to open port is can't open will close program
	if( !serial.open_port( device , baudrate )){
		ROS_FATAL(" Cannot open port %s" , device.c_str());
		return -1;
	}
	
	ROS_INFO("Open DVL on : %s" , device.c_str());
	ROS_INFO("\tbaudrate : %d" , baudrate);
	ROS_INFO("\tframe_id : %s" , frame_id.c_str());

// stop device for write device
	serial.write_string("===");
	serial.read_line();
	serial.read_line();
	serial.read_line();
	serial.read_line();

// load default
	serial.write_string("CR1\n"); // 1 is load default of factory
	serial.read_line();
	serial.read_line();

// enable = 1 , 0 = uenable single-ping bottom tracking
	serial.write_string("BP001\n");

//set maximum bottom search depth (dm)
	std::ostringstream temporary;
	temporary << "BX" << maximum_bottom_search_depth << "\n";
	serial.write_string( temporary.str() );

//set Heading alignment to 0 degrees , page 133
	std::string heading_alignment_angle = 
				zeabus_extension::convert::to_string( heading_alignment , true , false);
	heading_alignment_angle = "EA" + heading_alignment_angle + "\n";
	serial.write_string( heading_alignment_angle );	

//set salinity ( salt is component ) value of water
	std::string salinity_water_string = 
				zeabus_extension::convert::to_string( salinity_water , true , 5 , false);
	salinity_water_string = "ES" + salinity_water_string + "\n";
	serial.write_string( salinity_water_string );

// set time between ensemble
	serial.write_string("TE" + time_between_ensemble + "\n");

// set time between ping
	serial.write_string("TP" + time_between_pings + "\n");

// select type of data stream
	serial.write_string("PD" + 
				zeabus_extension::convert::to_string( data_stream_select ) + "\n");
// see detail of PD6 in page 212

// keep paramter and start to ping
	serial.write_string("CK\n");
	serial.write_string("CS\n");

//------------------------------ FINISH SETTING DVL -------------------------------------------//

//----------------------------------------SET UP ROS-------------------------------------------//

	ros::Publisher publisher_dvl_port = ros::NodeHandle().advertise< std_msgs::String >(
											"dvl/port" , 10 );
	std_msgs::String data_dvl_port;
// -------------------------------- SET UP DIAGNOSTIC UPDATER --------------------------------//

/*	diagnostic_msgs::Updater updater;
	updater.setHardwareIDF("Teledyne DVL @ %s" , device.c_str());
	updater.add("DVL status updater" , check_dvl_status);
	updater.force_update() */

	while( ros::ok() ){
		try{

//			updater.update();

			data_dvl_port.data = serial.read_line();
			publisher_dvl_port.publish( data_dvl_port );
		}
		catch ( std::exception& error){
			std::cout << error.what() << "\n";
		}
		ros::spinOnce();
	}

	serial.write_string("===");
	serial.read_line();
	serial.read_line();
	serial.read_line();
	serial.read_line();
	std::cout << "end file of DVL port\n";
	
}
