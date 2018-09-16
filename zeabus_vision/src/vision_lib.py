import color_text as ct


def print_result(msg, color=ct.DEFAULT):
    """
        print ('<----------') + str(msg) + ('---------->')
        #len of <---msg---> = 50
    """
    print '<{:-^80}>'.format(' ' + color + str(msg) + ct.DEFAULT + ' ')


def img_is_none():
    print(ct.RED + 'img is none.\nPlease check topic name or check camera is running' + ct.DEFAULT)


def get_topic(camera):
    if camera == 'front':
        return '/vision/front/image_raw/compressed'
    elif camera == 'bottom':
        return '/vision/bottom/image_raw/compressed'

def get_color_range(color, camera_position, number, mission):
    lower = None
    upper = None
    color_list = CONST.COLOR_LIST
    print('color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/lower_' + color)
    if color in color_list:
        lower = rospy.get_param(
            'color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/lower_' + color, '0,0,0')
        upper = rospy.get_param(
            'color_range_' + str(camera_position) + '_' + str(number) + '_' + str(mission) + '/color_' + camera_position + '/upper_' + color, '179,255,255')
        lower = range_str2array(lower)
        upper = range_str2array(upper)
        print "FOUND"
    print(lower, upper)
    return lower, upper

def Aconvert(inp, full):
    inp = float(inp)
    full = float(full)
    res = (inp - (full / 2.0)) / (full / 2.0)
    return res