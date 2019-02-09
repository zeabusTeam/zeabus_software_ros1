/*
	File name			:	second_control.cpp
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 26
	Date last modified	:	2018 , ??? ??
	Purpose				:	helpint function of second_control
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	-
*/

#include	<dynamic_reconfigure/server.h>
#include	<zeabus_control/pid_Config.h>

#include	<zeabus_library/control/pid.h>

double constant_velocity[3][6] = {	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }};
double offset_force[6] = { 0 , 0 , 0 , 0 , 0 , 0 };

bool already_loading_constant = false;
bool want_save_constant = false;

void dynamic_reconfigure_callback( zeabus_control::pid_Config &config , uint32_t level){
	printf("Tunning constant value\n");

	constant_velocity[0][0] = config.p_x_velocity;
	constant_velocity[1][0] = config.i_x_velocity;
	constant_velocity[2][0] = config.d_x_velocity;

	constant_velocity[0][1] = config.p_y_velocity;
	constant_velocity[1][1] = config.i_y_velocity;
	constant_velocity[2][1] = config.d_y_velocity;

	constant_velocity[0][2] = config.p_z_velocity;
	constant_velocity[1][2] = config.i_z_velocity;
	constant_velocity[2][2] = config.d_z_velocity;

	constant_velocity[0][3] = config.p_roll_velocity;
	constant_velocity[1][3] = config.i_roll_velocity;
	constant_velocity[2][3] = config.d_roll_velocity;

	constant_velocity[0][4] = config.p_pitch_velocity;
	constant_velocity[1][4] = config.i_pitch_velocity;
	constant_velocity[2][4] = config.d_pitch_velocity;

	constant_velocity[0][5] = config.p_yaw_velocity;
	constant_velocity[1][5] = config.i_yaw_velocity;
	constant_velocity[2][5] = config.d_yaw_velocity;

	offset_force[0] = config.x_offset;
	offset_force[1] = config.y_offset;
	offset_force[2] = config.z_offset;
	offset_force[3] = config.yaw_offset;
	offset_force[4] = config.pitch_offset;
	offset_force[5] = config.yaw_offset;
	
	if( already_loading_constant ) want_save_constant = true;
}

void reset_constant( zeabus_library::control::PID* pid ){
	for( int run = 0 ; run < 6 ; run++ ){
		pid[ run ].offset_i( offset_force[ run ] );
		pid[ run ].set_constant( constant_velocity[0][ run ] 
				, constant_velocity[1][ run ]
				, constant_velocity[2][ run ] );
	}
}
