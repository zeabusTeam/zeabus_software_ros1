// standard library 
#include	<iostream>
#include	<stdio.h> 
#include	<ros/ros.h>
#include	<cstdlib> // for malloc
#include	<iomanip> // for manage about std::cout about precision

// inclue library header and source from zeabus_extension
#include	<zeabus_extension/pid.h>
#include	"./../../zeabus_extension/src/discrete_pid.cpp"
#include	"./../../zeabus_extension/src/pid_control.cpp"
#include	<zeabus_extension/log_data.h>
#include	"./../../zeabus_extension/src/log_data.cpp"

// include part of control
#include	"head_variable.h"
