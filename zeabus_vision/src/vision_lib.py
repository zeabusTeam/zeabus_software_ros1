import color_text as ctxt

def print_result(msg, color=ctxt.DEFAULT):
    """
        print ('<----------') + str(msg) + ('---------->')
        #len of <---msg---> = 50
    """
    print '<{:-^80}>'.format(' ' + color + str(msg) + ctxt.DEFAULT + ' ')
