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


def Aconvert(inp, full):
    inp = float(inp)
    full = float(full)
    res = (inp - (full / 2.0)) / (full / 2.0)
    return res