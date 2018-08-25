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

#include	<zeabus_sensor/dvl_port.cpp>

int main( int argc , char **argv){

	ros::init( argc , argv , "port_dvl"); // init node name

	ros::NodeHandle nh("~");

	zeabus_serial::serial serial; // setup variable for connect device

// set up variable for receivce param data
	std::string device;
	int baudrate;
	std::string frame_id;
	int maximum_bottom_search_depth;
	int heading_alignment;

// get param data
	nh.param< std::string >("device" , device , "/dev/usb2serial/ftdi_FT03OMNT_03");
	nh.param< int >("baudrate" , baudrate , 115200 );
	nh.param< std::string >("frame_id" , frame_id , "dvl_link"); // what
	nh.param< float >("surface_depth" , surface_depth , 4.7);
	nh.param< int >("maximum_bottom_search_depth" , maximum_bottom_search_depth , 100);
	nh.param< int >("heading_alignment" ,  heading_alignment , 0);

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
	serial.write_string("BP1\n");

//set maximum bottom search depth (dm)
	stringstream temporary;
	temporary << "BX" << maximum_bottom_search_depth << "\n";
	search.write_string( temporary.str() );

//set Heading alignment to 0 degrees , page 133
	
}
