zeabus_extension::zeabus_control::discrete_pid* pid_position;
zeabus_extension::zeabus_control::discrete_pid* pid_velocity;

double constant_position[][3] =	{	{ 0 , 0 , 0 },
									{ 0 , 0 , 0 },
									{ 0 , 0 , 0 },
									{ 0 , 0 , 0 },
									{ 0 , 0 , 0 },
									{ 0 , 0 , 0 }
							};

double constant_velocity[][3] =	{	{ 0 , 0 , 0 },
									{ 0 , 0 , 0 },
									{ 0	, 0 , 0 },
									{ 0 , 0 , 0 },
									{ 0 , 0 , 0 },
									{ 0 , 0 , 0 }
							};

double current_state[6]		= { 0 , 0 , 0 , 0 , 0 , 0 };
double target_state[6]		= { 0 , 0 , 0 , 0 , 0 , 0 };
double different_state[6]	= { 0 , 0 , 0 , 0 , 0 , 0 };
double robot_error[6]		= { 0 , 0 , 0 , 0 , 0 , 0 };
double bound_error[6]		= { 0.01 , 0.01 , 0.03 , 0.1 , 0.1 , 0.03 };
double pid_force[6]			= { 0 , 0 , 0 , 0 , 0 , 0 };
double current_velocity[6]	= { 0 , 0 , 0 , 0 , 0 , 0 };
double target_velocity[6]	= { 0 , 0 , 0 , 0 , 0 , 0 };

int use_velocity[6] = { 0 , 0 , 0 , 0 , 0 , 0 };

zeabus_extension::zeabus_ros::dynamic_reconfigure* dynamic_reconfigure;
std::string package_dynamic_reconfigure = "zeabus_control";
std::string path_file_dynamic_reconfigure = "constant";
std::string name_file_dynamic_reconfigure = "2019_SAUVE";
std::string node_name_dynamic_reconfigure = "pid_control";
