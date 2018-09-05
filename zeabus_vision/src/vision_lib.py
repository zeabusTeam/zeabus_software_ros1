import color_text as ctxt

def print_result(msg, color=ctxt.DEFAULT):
    """
        print ('<----------') + str(msg) + ('---------->')
        #len of <---msg---> = 50
    """
    print '<{:-^80}>'.format(' ' + color + str(msg) + ctxt.DEFAULT + ' ')

def img_is_none():
    print(ctxt.RED + 'img is none.\nPlease check topic name or check camera is running' + ctxt.DEFAULT)
