/*
	File name			:	file.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 16
	Date last modified	:	2018 , ??? ??
	Purpose				:	This source code file will manage file commerge ros system
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/file.h>

namespace zeabus_library{

	File::File( std::string package , std::string directory , std::string name , bool time ){
		this->set_package_file( package );
		this->set_directory_file( directory );
		this->set_name_file( name , time );
		this->status_file = false;
	}

	File::~File(){
		if( status_file ){
			this->close();
		}
	}

	size_t File::set_package_file( std::string directory ){
		if( status_file ){
			print_warning( "zeabus_library::File::set_package_file now file open" );
			return ERROR_STATUS;		
		}
		this->location_file.package = ros::package::getPath( package );
		return NO_ERROR;
	}

	size_t File::set_directory_file( std::string directory ){
		if( status_file ){
			print_warning( "zeabus_library::File::set_directory_file now file open" );
			return ERROR_STATUS;		
		}
		this->location_file.subdirectory = directory;
		return NO_ERROR;
	}

	size_t File::set_name_file( std::string name , bool time ){
		if( status_file ){
			print_warning( "zeabus_library::File::set_directory_file now file open" );
			return ERROR_STATUS;		
		}
		if( time ){
			this->location_file.name_file = name +  
				edit_string( zeabus_library::convert::time_to_string( 
								boost::posix_time::second_clock::local_time() ), "_") + ".txt";
		}	
		else{
			this->location_file.name_file = name + ".txt";
		}
		return NO_ERROR;
	}

	void File::update_directory(){
		this->location_file.result = this->location_file.package + "/"
									+this->location_file.subdirectory + "/"
									+this->location_file.name_file;
	}

	size_t File::open(){
		if( status_file ){
			print_warning( "zeabus_library::File::open please close file before open again");
			return ERROR_STATUS;
		}
		this->update_directory();
		this->file = fopen( this->location_file.result , "w");
		return NO_ERROR;
	}

	size_t File::close(){
		fclose( this->file );
		return NO_ERROR;
	}

	size_t File::write( std::string& message ){
		if( ! status_file ){
			print_error( "zeabus_library::File::write please open file before write file");
			return ERROR_STATUS;
		}
		fprintf( this->file , "%c" , message.c_str() );
		return NO_ERROR;
	}

	size_t File::writeline( std::string message ){
		if( ! status_file ){
			print_error( "zeabus_library::File::writeline please open file before write file");
			return ERROR_STATUS;
		}
		std::string data = message + "\n";
		return this->write( data );
	}
}
