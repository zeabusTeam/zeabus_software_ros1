// please :set nu tabstop=4

// include 2 part file
#include "offset_controller_header.h"
#include "offset_controller_detail.h"

// initial code run 1 time
void init(){
	#ifdef test_02
		std::cout << "welocome to init file\n";
	#endif
	PID_position = ( find_velocity::second_case*)calloc 
						(6 , sizeof( find_velocity::second_case));
	PID_velocity = ( find_velocity::second_case*)calloc
						(6 , sizeof( find_velocity::second_case));
	for(int count = 0 ; count < 6 ; count++){
		PID_position[count].set_constant(0 ,0 ,0);
		PID_velocity[count].set_constant(0 ,0 ,0);
	}
}

int main(int argc , char **argv){
// declare basic ros subscribe . publish . service
	ros::init(argc , argv, "Controller");
	ros::NodeHandle nh;

// ---------------------------------- part of subscriber --------------------------------------
	ros::Subscriber sub_state = // listen now where I am
		nh.subscribe( "/auv/state" , 1000 , &listen_current_state);
	ros::Subscriber sub_target_velocity = // listen what target do you want
		nh.subscribe( "/zeabus/cmd_vel" , 1000 , &listen_target_velocity);
// ------------------------------------- end part ---------------------------------------------

// ---------------------------------- part of service -----------------------------------------
	ros::ServiceServer ser_cli_target_xy = // listen target of xy
		nh.advertiseService("/fix_abs_xy", service_target_xy);
	ros::ServiceServer ser_cli_target_distance = // listen target of xy
		nh.advertiseService("/fix_rel_xy", service_target_distance);
	ros::ServiceServer ser_cli_target_yaw = // listen target of yaw
		nh.advertiseService("/fix_abs_yaw", service_target_yaw);
	ros::ServiceServer ser_cli_rel_yaw = // listen target of yaw
		nh.advertiseService("/fix_rel_yaw", service_rel_yaw);
	ros::ServiceServer ser_cli_target_depth = // listen target of depth
		nh.advertiseService("/fix_abs_depth", service_target_depth);
	ros::ServiceServer ser_cli_rel_depth = // listen target of depth
		nh.advertiseService("/fix_rel_depth", service_rel_depth);
	ros::ServiceServer ser_cli_ok_position = // listen and answer now ok?
		nh.advertiseService("/ok_position", service_ok_position);
	ros::ServiceServer ser_cli_change_mode = // listen target mode
		nh.advertiseService("/change_mode", service_change_mode); 
	ros::ServiceServer ser_know_target = // listen target mode
		nh.advertiseService("/know_target", service_know_target); 
// ------------------------------------- end part ---------------------------------------------

// ------------------------------------ test state --------------------------------------------
	#ifdef test_01
		ros::Subscriber sub_test_state = // listen test state
			nh.subscribe( "/test/auv/state" , 1000 , &test_current_state);
		ros::Subscriber sub_test_orientation = // listen test orientation
			nh.subscribe( "/test/orientation" , 1000 , &test_current_orientation);
	#endif 
// ------------------------------------- end part ---------------------------------------------

// ----------------------------------- part of publisher --------------------------------------
	ros::Publisher tell_force = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
//	ros::Publisher tell_force = nh.advertise<geometry_msgs::Twist>("/feedback/cmd_vel", 1000);
//	ros::Publisher tell_force = nh.advertise<geometry_msgs::Twist>("/cmd_vel_blank", 1000);
// -------------------------------------- end part --------------------------------------------

// ------------------------------- part of dynamic reconfiure ---------------------------------
	dynamic_reconfigure::Server<zeabus_controller::OffSetConstantConfig> server;
	dynamic_reconfigure::Server<zeabus_controller::OffSetConstantConfig>::CallbackType tunning;
// -------------------------------------- end part --------------------------------------------

	init();

// ---------------------------------- again about dynamic -------------------------------------
	tunning = boost::bind(&config_constant_PID, _1 , _2);
	server.setCallback( tunning );
// -------------------------------------- end part --------------------------------------------

	ros::Rate rate(20);

    PID_velocity[2].set_limit_i(0.5);
	while(nh.ok()){
		if(first_time_tune){
			#ifdef test_02
				std::cout << "Before download\n";
			#endif
				PID_file.load_file("Controller");
			#ifdef test_02
				std::cout << "After download\n";
			#endif
			first_time_tune = false;
			rate.sleep();
			set_all_tunning();
			reset_all_I();
		}
		else if(change_tune){
			#ifdef test_02
				std::cout << "Before save\n";
			#endif
				PID_file.save_file("Controller");
			#ifdef test_02
				std::cout << "After save\n";
			#endif
			change_tune = false;
			set_all_tunning();
			reset_all_I();
		}
		else{}
		if( mode_control == 1 ){
			#ifdef print_data
				std::cout << "mode control is 1 : test offset z : " << offset_force[2] << "\n"; 
				std::cout << "value of depth is " << current_position[2] << "\n";
				std::cout << "target of depth is " << target_position[2] << "\n";
			#endif // this part will allow force of z in about offset only
			sum_force[0] = 0;
			sum_force[1] = 0;
			sum_force[2] = offset_force[2];
			sum_force[3] = 0;
			sum_force[4] = 0;
			sum_force[5] = 0;
			for( int count = 0 ; count < 6 ; count++){
				if( absolute( sum_force[count]) >= bound_force[count]){
					ROS_FATAL("Controller force over bound warning : %d" , count);
					if( sum_force[count] > 0) sum_force[count] = bound_force[count];
					else sum_force[count] = -1*bound_force[count];
				}
			}
			tell_force.publish( create_msg_force() );	
		}
		else if( mode_control == 2){
			world_error[2] = target_position[2] - current_position[2];
			robot_error[2] = world_error[2];
			sum_force[2] = PID_position[2].calculate_velocity( robot_error[2] ) + offset_force[2];
			#ifdef print_data
				std::cout << "mode control is 2 : open z : " << sum_force[2] << "\n";
				std::cout << "value of depth is " << std::setprecision(3) 
							<< current_position[2] << "\n";
				std::cout << "target of depth is " << target_position[2] << "\n";
			#endif
			sum_force[0] = 0;
			sum_force[1] = 0;
			sum_force[3] = 0;
			sum_force[4] = 0;
			sum_force[5] = 0;
			for( int count = 0 ; count < 6 ; count++){
				if( absolute( sum_force[count]) >= bound_force[count]){
					ROS_FATAL("Controller force over bound warning : %d" , count);
					if( sum_force[count] > 0) sum_force[count] = bound_force[count];
					else sum_force[count] = -1*bound_force[count];
				}
			}
			tell_force.publish( create_msg_force() );	
		}
		else if( mode_control == 3){
			#ifdef print_data
				std::cout << "mode control is 3 : test offset 3 : " << offset_force[3] << "\n"; 
				std::cout << "value of roll is " << std::setprecision(3) 
							<< current_position[3] << "\n";
			#endif // this part will allow force of roll in about offset only
			sum_force[0] = 0;
			sum_force[1] = 0;
			sum_force[2] = offset_force[2];
			sum_force[3] = offset_force[3];
			sum_force[4] = 4;
			sum_force[5] = 5;
			for( int count = 0 ; count < 6 ; count++){
				if( absolute( sum_force[count]) >= bound_force[count]){
					ROS_FATAL("Controller force over bound warning : %d" , count);
					if( sum_force[count] > 0) sum_force[count] = bound_force[count];
					else sum_force[count] = -1*bound_force[count];
				}
			}
			tell_force.publish( create_msg_force() );	
		}
		else if( mode_control == 4){
			#ifdef print_data
				std::cout << "mode control is 4 : test offset 4 : " << offset_force[4] << "\n"; 
				std::cout << "value of pitch is " << std::setprecision(3) 
							<< current_position[4] << "\n";
			#endif // this part will allow force of roll and pitch in about offset only
			sum_force[0] = 0;
			sum_force[1] = 0;
			sum_force[2] = offset_force[2];
			sum_force[3] = offset_force[3];
			sum_force[4] = offset_force[4];
			sum_force[5] = 5;
			for( int count = 0 ; count < 6 ; count++){
				if( absolute( sum_force[count]) >= bound_force[count]){
					ROS_FATAL("Controller force over bound warning : %d" , count);
					if( sum_force[count] > 0) sum_force[count] = bound_force[count];
					else sum_force[count] = -1*bound_force[count];
				}
			}
			tell_force.publish( create_msg_force() );	
		}
		else if( mode_control == 5){
			#ifdef test_02
				std::cout << "----------------- now you are mode 5 -------------------\n";
			#endif	
			for( int count = 0 ; count < 6 ; count++){
				world_error[count] = target_position[count] - current_position[count];
			}
			// calculate error of world
			world_distance = sqrt( pow(world_error[0] , 2) +
								   pow(world_error[1] , 2));
			world_yaw = convert_range_radian( atan2( world_error[1], world_error[0]));
//			diff_yaw = convert_min_radian ( current_position[5] - world_yaw );
			diff_yaw = find_min_angular( current_position[5] , world_yaw);
//			diff_yaw = find_min_angular(target_position[5] , world_yaw);
			#ifdef print_data
				std::cout << "World Distance : " << world_distance << "\n";
				std::cout << "World Yaw : " << world_yaw << "\n";
				std::cout << "Diff Yaw : " << diff_yaw << "\n";
			#endif
			// calculate error of robot
			robot_error[0] = world_distance * cos( diff_yaw );
			robot_error[1] = world_distance * sin( diff_yaw );
			robot_error[2] = target_position[2] - current_position[2];
			robot_error[3] = convert_min_radian( target_position[3] - current_position[3]);
			robot_error[4] = convert_min_radian( target_position[4] - current_position[4]);
			robot_error[5] = convert_min_radian( target_position[5] - current_position[5]);
			for( int count = 0 ; count < 6 ; count++){
				if( can_fix[ count ] && want_fix[ count ]){
					#ifdef test_02
						std::cout << "Count is " << count << " use pid\n";
					#endif
					if( absolute(robot_error[count]) < ok_error[count] && count != 2){
						sum_force[count] = offset_force[count];
					}
					else if( count == 2){
						pid_force[2] += PID_position[ count ].calculate_velocity( 
							robot_error[2] );
                        sum_force[2] = pid_force[2] + offset_force[2];
					}
					else{
						pid_force[count] = 
							PID_position[ count ].calculate_velocity( robot_error[ count]);
						sum_force[ count ] = pid_force[count] + offset_force[count];
                    }
                    if( count == 3 || count == 4) sum_force[count] *= -1;
				}
				else{
					#ifdef test_02
						std::cout << "Count is " << count << " use velocity\n";
					#endif
					if( count < 2 && use_K_velocity){
// -------------------------------------- force = k * velocity * velocity ------------------
//						sum_force[count] = K_velocity[count] * 
//												pow (target_velocity[count] , 2)
//                                                * find_direction( target_velocity[count]);
// ----------------------------------force = k * velocity ----------------------------------
//						sum_force[count] = ( K_velocity[count] * target_velocity[ count ] )
//												+ 0.08586;
						sum_force[count] = ( 3.4141 * target_velocity[ count ] )
										 + ( find_direction( target_velocity[ count ])*0.08586);
						std::cout << "In linear formular \n";
						std::cout << " count is " << count 
								  << " and value is " << sum_force[count] << "\n";
					}
//                    *
                        //sum_force[count] = target_velocity[ count ];
//					if( count == 2 && target_velocity[2] < 0) sum_force[2] = -4.5;
					else if( count == 2){
//                        sum_force[2] += PID_velocity[2].calculate_velocity(
//											target_velocity[2]) + offset_force[count];
                        pid_force[2] += PID_velocity[2].calculate_velocity(
											target_velocity[2]);
                        sum_force[2] = pid_force[2] + offset_force[2];
                    
                    }
					else {
                        sum_force[count] = PID_velocity[count].calculate_velocity(
											target_velocity[count] - current_velocity[count]);

                    }
                        //sum_force[count] = target_velocity[ count ];
				}
//                if( count == 5 ) sum_force[count] *= -1;
                if( count == 2 ){
//                    if( robot_error[count] < -0.1) sum_force[ count ] = -4.5;
                    if( ( target_position[2] < epsilon ) && (target_position[2] > -1*epsilon) ){
                        if( current_position[2] < -1 ) sum_force[2] = 1;
                        else sum_force[count] = 0;

                    }
                }
			}
			for( int count = 0 ; count < 6 ; count++){
				if( absolute( sum_force[count]) >= bound_force[count]){
					ROS_FATAL("Controller force over bound warning : %d" , count);
					if( sum_force[count] > 0) sum_force[count] = bound_force[count];
					else sum_force[count] = -1*bound_force[count];
                    if( count == 2 and sum_force[count] > 0) pid_force[2] = 1.5;
                    else if(count == 2) pid_force[2] = -1.5;
				}
			}
			tell_force.publish( create_msg_force() );
		}
		#ifdef print_all_value
			PID_file.clear();
			ROS_INFO("-------------------------- print data -----------------------------");
			ROS_INFO("target_position:\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.4lf" ,
						target_position[0] , target_position[1] , target_position[2],
						target_position[3] , target_position[4] , target_position[5]);
			ROS_INFO("current_position:\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.4lf" ,
						current_position[0] , current_position[1] , current_position[2],
						current_position[3] , current_position[4] , current_position[5]);
			ROS_INFO("world_error:\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.4lf" ,
						world_error[0] , world_error[1] , world_error[2],
						world_error[3] , world_error[4] , world_error[5]);
			ROS_INFO("robot_error:\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.4lf" ,
						robot_error[0] , robot_error[1] , robot_error[2],
						robot_error[3] , robot_error[4] , robot_error[5]);
			ROS_INFO("offset_force:\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf" ,
						offset_force[0] , offset_force[1] , offset_force[2],
						offset_force[3] , offset_force[4] , offset_force[5]);
			ROS_INFO("pid_force:\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.4lf" ,
						pid_force[0] , pid_force[1] , pid_force[2],
						pid_force[3] , pid_force[4] , pid_force[5]);
			ROS_INFO("sum_force:\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.4lf" ,
						sum_force[0] , sum_force[1] , sum_force[2],
						sum_force[3] , sum_force[4] , sum_force[5]);
			ROS_INFO("can_fix:\t\t%d\t%d\t%d\t%d\t%d\t%d" ,
						can_fix[0] , can_fix[1] , can_fix[2],
						can_fix[3] , can_fix[4] , can_fix[5]);
			ROS_INFO("want_fix:\t\t%d\t%d\t%d\t%d\t%d\t%d" ,
						want_fix[0] , want_fix[1] , want_fix[2],
						want_fix[3] , want_fix[4] , want_fix[5]);
			ROS_INFO("target_velocity:\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.4lf" ,
						target_velocity[0] , target_velocity[1] , target_velocity[2],
						target_velocity[3] , target_velocity[4] , target_velocity[5]);
			ROS_INFO("current_velocity:\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.4lf" ,
						current_velocity[0] , current_velocity[1] , current_velocity[2],
						current_velocity[3] , current_velocity[4] , current_velocity[5]);
		#endif
		current_time = ros::Time::now();
		if( (current_time - last_target_velocity).toSec() < diff_time){
			#ifdef test_02
				std::cout << "Min than " << diff_time << " second form last time to get "
							<< "target velocity " << "reset now \n"; 
			#endif
			reset_position = true;
		}
		else reset_want_fix();
		ros::spinOnce();
		rate.sleep();
	}
}
