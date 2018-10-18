<h1>WELCOME TO CONTROL</h1>
<h2>01. Introduction</h2>
	<p>This control is use pid in term of discrete to control robot</p>

<h2>02. All File</h2>
	<ul>
		<li>main_control.cpp</li>
		<p>this file have main thread to run program</p>
		<li>head_control.cpp</li>
		<p>This file will collect other file to this file code</p>
		<li>listen_odometry.cpp</li>
		<p>This file is about callback in subscribe node to listen state of robot </p>
		<li>listen_twist.cpp</li>
		<p>This file is about callbacl in subscribe node to listen twist message</p>
	</ul>

<h2>03. Variable</h2>
	<p>This control will use many array to show step for calculate</p>
	<ul>
		<li>current_state</li>
			<p>For collecting current state of robot</p>
		<li>target_state</li>
			<p>For collecting target state of robot</p>
		<li>world_error</li>
			<p>For collecting error in term of world don't care state of robot</p>
		<li>robot_error</li>
			<p>For collecting error in term of robot term</p>
		<li>pid_force</li>
			<p>For collecting output from pid by robot_error<\p>
		<li>robot_force</li>
			<p>For collecting output when decision of pid_force to send to thruster</p>
	</ul>

<h2>04. Function</h2>
