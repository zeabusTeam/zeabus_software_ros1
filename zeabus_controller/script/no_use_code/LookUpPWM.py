"PWM lookup for thrust mapper"
import bisect
import rospkg
from read_pwm_force import read_file

#set variable to find locate of package
rospack = rospkg.RosPack()

name_package = 'zeabus_controller'
location_file_01 = '/script/teledyne_2018.txt'

pwm_force_data_01 = read_file( rospack.get_path(name_package) + location_file_01 )

location_file_02 = '/script/blue_robotics_2018.txt'

PWM_TABLE_01 = pwm_force_data_01.get_column(0, ',')
FORCE_LOOKUP_01 = pwm_force_data_01.get_column(1, ',')

PWM_TABLE_02 = pwm_force_data_02.get_column(0, ',')
FORCE_LOOKUP_02 = pwm_force_data_02.get_column(1, ',')

print("------------- set 01 ---------------")
print(PWM_TABLE_01)
print(FORCE_LOOKUP_01)
print("------------- set 01 ---------------")
print(PWM_TABLE_02)
print(FORCE_LOOKUP_02)
def _float_equal(in1, in2, epsilon=0.000001):
    "checking float equality"
    return True if abs(in1-in2) < epsilon else False

def lookup_pwm_01(force):
    "look for pwm from force"
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
        out_pwm = PWM_TABLE_01[idx - 1] + ((PWM_TABLE_01[idx] - PWM_TABLE_01[idx - 1]) *
                                        ((force - FORCE_LOOKUP_01[idx - 1])/
                                         (FORCE_LOOKUP_01[idx] - FORCE_LOOKUP_01[idx - 1])))
    return out_pwm

def lookup_pwm_array_01(forces):
    out = []
    for force in forces:
        out.append(lookup_pwm_01(force))
    return out

def lookup_pwm_02(force):
    "look for pwm from force"
    # FORCE_LOOKUP = [1, 2, 3, 4, 5]
    # pwm_table = [2, 3, 8, 10, 11]
    idx = bisect.bisect_left(FORCE_LOOKUP_01, force)
    if idx == 0:
        out_pwm = PWM_TABLE_02[0]
    elif idx == len(FORCE_LOOKUP_02):
        out_pwm = PWM_TABLE_02[len(PWM_TABLE_02)-1]
    elif _float_equal(force, FORCE_LOOKUP_02[idx]):
        out_pwm = PWM_TABLE_02[idx]
    else:
        out_pwm = PWM_TABLE_02[idx - 1] + ((PWM_TABLE_02[idx] - PWM_TABLE_02[idx - 1]) *
                                        ((force - FORCE_LOOKUP_02[idx - 1])/
                                         (FORCE_LOOKUP_02[idx] - FORCE_LOOKUP_02[idx - 1])))
    return out_pwm

def lookup_pwm_array_02(forces):
    out = []
    for force in forces:
        out.append(lookup_pwm_02(force))
    return out

def main():
    "testing purpose"
    aaaa = [0, 1, 2.5, 3.5, 4.2, 4.7, 10]
    print(lookup_pwm_array_01(aaaa))
    print(len(PWM_TABLE_01),len(FORCE_LOOKUP_01));

if __name__ == "__main__":
    main()
