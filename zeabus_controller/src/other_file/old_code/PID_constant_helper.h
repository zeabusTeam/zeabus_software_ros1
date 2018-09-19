#include<fstream>
#include<iostream>
#include<cstdlib>
#include<ros/package.h>
#define CONTROLLER_SAVED_FILE_PATH "~/controller_dump.yaml"

class PID_constant_helper{
    private:
        static std::string get_file_path(){
            std::string path = ros::package::getPath("zeabus_controller");
            std::fstream target_file;
            path += "/const_dump.yaml";
            return path;
        }
    public:
        static void dump_file(double cmd_vel_k[6][3],double fix_point_k[6][3]){
            static std::string axis[] = {"x","y","z","roll","pitch","yaw"};
            static std::string type[] = {"P","V"};
            static std::string param[] = {"P","I","D"};
            std::fstream target_file;
            target_file.open (get_file_path().c_str(),std::ios::out);
            if( target_file.is_open()){
               for(int i=0;i<3;i++){
                    for(int j=0;j<6;j++){
                        target_file << "K" << param[i] << "P" << axis[j] << ": " << fix_point_k[j][i] << std::endl;
                    }
                }
                for(int i=0;i<3;i++){
                    for(int j=0;j<6;j++){
                        target_file << "K" << param[i] << "V" << axis[j] << ": " << cmd_vel_k[j][i] << std::endl;
                    }
                }
                target_file.close();
            }
            else{
                std::cout << "Controller : Error opening files to dump" << std::endl;
            }
        }
        static void load_file(std::string node_name){
            std::ifstream target_file(get_file_path().c_str());
	    std::cout << "Why I'm here PID_constant_help.h " << std::endl;
            if(!target_file.is_open()){
                std::cout << "Controller : Error opening file to load"<<std::endl;
                return;
            }else{
                target_file.close();
            }
            std::string cmd_string(std::string("rosrun dynamic_reconfigure dynparam load /") + node_name + " " + get_file_path());
            std::cout << cmd_string << std::endl;
            //cmd_string.append(node_name);
            ////cmd_string.append(path);
            //system(cmd_string.c_str());
        }
};
