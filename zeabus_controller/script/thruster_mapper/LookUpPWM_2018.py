#!/usr/bin/env python
import rospy
import bisect
import rospkg
from read_pwm_force import read_file

print("finish import")

#set variable to find locate of package
rospack = rospkg.RosPack()

name_package = 'zeabus_controller'
print("finish init node")
#location_file_01 = '/script/teledyne_2018_bound_min.txt'
location_file_01 = '/script/thruster_mapper/linear_teledyne_2018.txt'
#location_file_01 = '/script/temporary.txt'
pwm_force_data_01 = read_file( rospack.get_path(name_package) + location_file_01 )

#2location_file_02 = '/script/blue_robotics_2018_convert_update.txt'
location_file_02 = '/script/thruster_mapper/linear_bluerobotics_2018.txt'
pwm_force_data_02 = read_file( rospack.get_path(name_package) + location_file_02 )

PWM_TABLE_01 = pwm_force_data_01.get_column(0, ',')
FORCE_LOOKUP_01 = pwm_force_data_01.get_column(1, ',')

PWM_TABLE_02 = pwm_force_data_02.get_column(0, ',')
FORCE_LOOKUP_02 = pwm_force_data_02.get_column(1, ',')

print("------------- set 01 ---------------")
print(PWM_TABLE_01)
print(FORCE_LOOKUP_01)
print("------------- set 02 ---------------")
print(PWM_TABLE_02)
print(FORCE_LOOKUP_02)
def _float_equal(in1, in2, epsilon=0.000001):
#    "checking float equality"
    return True if abs(in1-in2) < epsilon else False

def lookup_pwm_01(force):
#    "look for pwm from force"
    # FORCE_LOOKUP = [1, 2, 3, 4, 5]
    # pwm_table = [2, 3, 8, 10, 11]
    idx = bisect.bisect_left(FORCE_LOOKUP_01, force)
    if idx == 0:
        out_pwm = PWM_TABLE_01[0]
    elif idx == len(FORCE_LOOKUP_01):
        out_pwm = PWM_TABLE_01[len(PWM_TABLE_01)-1]
    elif _float_equal(force, FORCE_LOOKUP_01[idx]):
        out_pwm = PWM_TABLE_01[idx]
    else:
        out_pwm = PWM_TABLE_01[idx - 1] + ((PWM_TABLE_01[idx] - PWM_TABLE_01[idx - 1]) * ((force - FORCE_LOOKUP_01[idx - 1]) / (FORCE_LOOKUP_01[idx] - FORCE_LOOKUP_01[idx - 1])))
    return out_pwm

def lookup_pwm_array_01(force):
    out = []
    for force in forces:
        out.append(lookup_pwm_01(force))
    return out

def lookup_pwm_02(force):
#    "look for pwm from force"
    # FORCE_LOOKUP = [1, 2, 3, 4, 5]
    # pwm_table = [2, 3, 8, 10, 11]
    idx = bisect.bisect_left(FORCE_LOOKUP_02, force)
    if idx == 0:
        out_pwm = PWM_TABLE_02[0]
    elif idx == len(FORCE_LOOKUP_02):
        out_pwm = PWM_TABLE_02[len(PWM_TABLE_02)-1]
    elif _float_equal(force, FORCE_LOOKUP_02[idx]):
        out_pwm = PWM_TABLE_02[idx]
    else:
        out_pwm = PWM_TABLE_02[idx - 1] + ((PWM_TABLE_02[idx] - PWM_TABLE_02[idx - 1]) * ((force - FORCE_LOOKUP_02[idx - 1]) / (FORCE_LOOKUP_02[idx] - FORCE_LOOKUP_02[idx - 1])))
    return out_pwm

def lookup_pwm_array_02(force):
    out = []
    for force in forces:
       out.append(lookup_pwm_02(force))
    return out

def main():
	print("testing purpose")
	print("please press ctrl+d for close program")
	print("data for 8 thruster and use \" \" for split each data")
	while(True):
		data = raw_input().split()
		answer = []
		for i in range(len(data)):
			data[i] = float(data[i])
		print("data is ")
		print(data)
		for i in range(0,4):
			answer.append(lookup_pwm_02(data[i]))
		for i in range(4,8):
			answer.append(lookup_pwm_01(data[i]))
		print("for table_02")
		print(answer[:4])
#		print(lookup_pwm_array_02(answer[:4]))
		print("for table_01")
		print(answer[4:])
#		print(lookup_pwm_array_01(answer[4:]))

#    print(lookup_pwm_array_01(aaaa))
#    print(len(PWM_TABLE_01),len(FORCE_LOOKUP_01));

if __name__ == "__main__":
	rospy.init_node("test_find_force")
	main()
