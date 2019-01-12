/*
	File name			:	dvl_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 12
	Date last modified	:	2019 , ??? ??
	Purpose				:	This is file to use read connect ros and DVL

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Maintain

	Namespace			:	None
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/sensor/string_port.h>

int main( int argc , char ** argv ){

	ros::init( argc , argv , "port_dvl" );

	ros::NodeHandle nh(""); // Handle for manage about this file in ros system
	ros::NodeHandle ph("~"); // Handle for managae param from launch 

//////////////////////////////-- PARAMETER PART --///////////////////////////////////////////////

	std::string port_name =  "/dev/usb2serial/ftdi_FT2VR5PM_02";
	std::string topic_output;

//	ph.param< std::string >("name_port_dvl" , port_name , "/dev/usb2serial/ftdi_FT2VR5PM_02");
	ph.param< std::string >("topic_output_port_dvl" , topic_output , "/sensor/dvl/port" );

////////////////////////////////////////////////////////////////////////////////////////////////

	zeabus_library::sensor::StringPort serial_port( port_name ) ;

	bool result;
	std::string message;
	int count = 0;

	serial_port.open_port( result );
	if( !(result) ){
		printf("Failure to open port %s\n" , port_name.c_str() );
		return -1 ;
	}

	serial_port.set_option_port( boost::asio::serial_port_base::baud_rate( 115200 ) );

	serial_port.write_data("===");
	count = 0;
	do{
		serial_port.read_data( message );
		count++;
	}while( message != "Explorer DVL" && count < 50 );
	serial_port.read_data( message );
	serial_port.read_data( message );
	serial_port.read_data( message );

	serial_port.close_port( result );
	printf("End Action on DVL port file\n");
	return 0;

}
