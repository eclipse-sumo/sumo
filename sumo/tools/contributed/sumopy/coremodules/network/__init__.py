__version__ = "0.0"

print 'init', __name__


def get_wxgui():
    # try:
    from wxgui import WxGui
    return WxGui(__name__)
    # except:
    #  return None
