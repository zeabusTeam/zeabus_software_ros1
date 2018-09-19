#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <cstdlib>
#include <ros/package.h>
#define name_file "control_tune"

class manage_PID_file{
	private:	
		std::string path;
		std::fstream target_file;
		std::ifstream open_file;
	public:
		void load_file(std::string node_name);
		void save_file(std::string node_name);
		std::string sub_directory;
		std::string locate_file;
		manage_PID_file(std::string file_name);
		void clear();
};

	manage_PID_file::manage_PID_file(std::string file_name){
		this->sub_directory = "constant";
		this->path = ros::package::getPath("zeabus_controller");
		this->locate_file = path + "/" + sub_directory + "/" + file_name;
		std::cout <<  "Intial file : " << this->locate_file << std::endl;
	}
	
	void manage_PID_file::save_file(std::string node_name){
		std::string cmd_string = "rosrun dynamic_reconfigure dynparam dump /" + node_name +" "+ this->locate_file;
		std::system( cmd_string.c_str());
		std::cout << "Save file : " << this->locate_file << std::endl;
	}

	void manage_PID_file::load_file(std::string node_name){
		std::string cmd_string = "rosrun dynamic_reconfigure dynparam load /" + node_name +" "+ this->locate_file + " &";
		std::system( cmd_string.c_str() );
		std::cout <<  "Load file : " << this->locate_file << std::endl;
	}

	void manage_PID_file::clear(){
		std::system( "clear" );
	}

class manage_log{
	private:
		std::time_t time_now;
		
	public:
		manage_log(std::string name);
		std::string sub_directory;
		std::string locate_file;
		std::string cmd_string;
		void write_log(std::string message);
};

	manage_log::manage_log(std::string name){
		time_now = std::time(NULL);
		sub_directory = ros::package::getPath("zeabus_controller") + "/src/log";
		locate_file = name;
		cmd_string = "echo \"----------------- start log control -----------------\" > " 
					 + sub_directory + "/" + locate_file + ".txt";
		std::system( cmd_string.c_str() );
	}

	void manage_log::write_log( std::string message){
		time_now = std::time(NULL);
		cmd_string = "echo \"------------------ at " 
					 + (std::string) std::asctime(std::localtime(&time_now)) 
					 + " >> " + sub_directory + "/" + locate_file + ".txt";
		std::system( cmd_string.c_str() );
		cmd_string = "echo \"" + message + "\"" 
					 + " >> " + sub_directory + "/" + locate_file + ".txt";
		std::system( cmd_string.c_str() );
	}
