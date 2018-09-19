How to run control in machine (4 step) in Machine
---minimal ( alias )
---sensor ( alias )
---thruster ( alias )
---linear ( alias )

This controll use x y z roll pitch yaw to calculate value and use matrix transform to convert to pwn for moter
and use PID to calcuater about that
										Supasan Komonlit, 2018

Now we will description about calculate force for target velocity
--- In the past we use formular is force = K * velocity * velocity
--- now we use formulat is force = K * velocity
--- that mean if We try to calculate about min force and max fore in Y and X axies 
--- we will find in X axies : MIN 0.12 MAX is 3.5 
--- so in Y axies have use thruster same X aies that mean : MIN 0.12 MAX is 3.5
		form equation one variable

			0.12 = K * 0.01 + C  ----1
			3.5  = K * 1 + C     ----2
	
			3.38 = 0.99K
			K = 3.41414141
			C = 0.08586

                                        Supasan Komonlit, 2018 -- 07_28_2518

Now we will description about calculate fore for control position depth
    In past we try to use only pid and fusion of pid and offset to control that
    Now we have a problem so we try to use pid of delta but it have problem of frequency
    So I will try to use linear equation of offset to control depth together with pid
    We will use equation when target depth < 0
        from equation two variable
            force = K * depth + C

            -2    = K * -0.075 + C   ----1
            -2.2  = K * -0.1  + C   ----2 or
                  =   * -0.12 + C

when battery is full
            so set 01 is
                -2    = K * -0.10 + C   -----1 
                -2.1  = K * -0.13 + C   -----2
and         so set 02 is
                -2.1  = K * -0.5  + C   -----3
                -2.3  = K* -1     + C   -----4
                -2.4  = K + -1.5  + C   -----5
            form set 01 can get
                -0.1  = K * -0.03 
                K = 3.3333
                C = -1.6667
            form set 02 can get
                -0.1  = K * -0.5 
                K = 0.2
                C = -2.1
                                        Supasan Komonlit, 2018 -- 07_30_2518
