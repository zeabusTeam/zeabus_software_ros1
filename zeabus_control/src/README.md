<h1>WELCOME TO CONTROL</h1>
<h2>01. Introduction</h2>
	<p>This control is use pid in time-error to control robot</p>

<h2>02. All File</h2>
	<p>some file on list have move to zeabus_library package for make shared library</p>
	<ul>
		<li>main_control.cpp</li>
		<p>this file have main thread to run program</p>
		<li>head_control.cpp</li>
		<p>This file will collect other file to this file code</p>
		<li>other_function.cpp</li>
		<p>This file collect other function to use only this part</p>
		<li>listen_odometry.cpp</li>
		<p>This file is about callback in subscribe node to listen state of robot </p>
		<li>listen_twist.cpp</li>
		<p>This file is about callbacl in subscribe node to listen twist message</p>
		<li>find_error_state.cpp</li>
		<p>This file error state of robot</p>
		<li>find_robot_error.cpp</li>
		<p>This file is use for covert world_error into robot frame</p>
		<li>find_robot_force.cpp</li>
		<p>This file is use for manage parity of axis or axes of robot</p>
		<li>normal_pid.cpp</li>
		<p>This file collect pid file in standard pid</p>
		<li>normal_pid_bound_i.cpp</li>
		<p>This file is child of normal_pid but adding condition to bound term force</p>
		<li>discrete_pid.cpp</li>
		<p>This file is child of normal_pid but change alogorithym to calculate force</p>
		<li>zeabus_extension/log_data.h</li>
		<p>This file import from other package for use about load save dynamic reconfigure</p>
		<li>service_control.cpp</li>
		<p>This file is main class of other service by control</p>
		<li>service_get_target.cpp</li>
		<p>This file is subservice of control by using get_target.srv</p>
		<li>service_one_point.cpp</li>
		<p>This file is subservice of control by using one_point.srv</p>
		<li>service_two_point.cpp</li>
		<p>This file is subservice of control by using two_point.srv</p>
		<li>service_check_state.cpp</li>
		<p>This file is subservice of control by using check_positiob.srv</p>
		
	</ul>

<h2>03. Print_Variable</h2>
	<p>This control will use many array to show step for calculate and will display on</p>
	<ul>
		<li>current_state</li>
			<p>For collecting current state of robot</p>
		<li>target_state</li>
			<p>For collecting target state of robot</p>
		<li>world_error</li>
			<p>For collecting error in term of world don't care state of robot</p>
		<li>robot_error</li>
			<p>For collecting error in term of robot term</p>
		<li>ok_error</li>
			<p>set about value of error when happand form error of sensor</p>
		<li>bound_error</li>
			<p>For callecting error from robot error after checking dicision about ok_error</p>
		<li>pid_force</li>
			<p>For collecting output from pid by bound_error<\p>
		<li>robot_force</li>
			<p>For collecting output when decision of pid_force to send to thruster</p>
		<li>current_velocity</li>
			<p>For collecting current velocity</p>
		<li>target_velocity</li>
			<p>For collecting target velocity</p>
		<li>error_velocity</li>
			<p>For collecting error velocity when use target velocity</p>
		<li>use_target_velocity</li>
			<p>For count limit of use target velocity</p>
	</ul>

<h2>04. Function</h2>
