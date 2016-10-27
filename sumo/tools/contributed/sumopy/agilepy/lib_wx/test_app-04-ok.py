import os, sys
 
import wx
from wx.lib.wordwrap import wordwrap

from mainframe import *

# import corepackages
from test_glcanvas import *

##
##import wx
##

##try:
##    dirName = os.path.dirname(os.path.abspath(__file__))
##except:
##    dirName = os.path.dirname(os.path.abspath(sys.argv[0]))
##
##sys.path.append(os.path.split(dirName)[0])

try:
    APPDIR = os.path.dirname(os.path.abspath(__file__))
except:
    APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    
IMAGEDIR = os.path.join(os.path.dirname(__file__),"images")
ICONPATH = os.path.join(IMAGEDIR,'icon_color_small.png')# None

class MyApp(wx.App):
   def __init__(self, redirect=False, filename=None):
       wx.App.__init__(self, redirect, filename)
       #self.frame = wx.Frame(None, wx.ID_ANY, title='My Title')
       self.mainframe = AgileMainframe(title='MyApp')
       if ICONPATH != None:
            icon = wx.Icon(ICONPATH,wx.BITMAP_TYPE_PNG, 16,16)
            self.mainframe.SetIcon(icon) 
        
       #splitter = MainSplitter(self.frame)
       #gleditor = splitter.add_view("GLeditor", GlEditor)
       
       gleditor = self.mainframe.add_view("GLeditor", GlEditor)
   
       # change the button's parent to refer to my panel
       #b = wx.Button(self.panel, -1, "Show a wx.AboutBox", (50,50))
       #self.Bind(wx.EVT_BUTTON, self.OnButton, b)

       self.mainframe.Show()
       
       canvas = gleditor.get_canvas()
       canvas.add_element(lines)
       canvas.add_element(triangles)
       canvas.add_element(rectangles)

   def OnButton(self, evt):
       # First we create and fill the info object
       info = wx.AboutDialogInfo()
       info.Name = "Hello World"
       info.Version = "1.2.3"
       info.Copyright = "(C) 2006 Programmers and Coders Everywhere"
       info.Description = wordwrap(
           "A \"hello world\" program is a software program that prints out "
           "\"Hello world!\" on a display device. It is used in many introductory "
           "tutorials for teaching a programming language."
                     "\n\nSuch a program is typically one of the simplest programs possible "
           "in a computer language. A \"hello world\" program can be a useful "
           "sanity test to make sure that a language's compiler, development "
           "environment, and run-time environment are correctly installed.",
           # change the wx.ClientDC to use self.panel instead of self
           350, wx.ClientDC(self.panel))
       info.WebSite = ("http://en.wikipedia.org/wiki/Hello_world", "Hello World home page")
       info.Developers = [ "Joe Programmer",
                           "Jane Coder",
                           "Vippy the Mascot" ]

       # change the wx.ClientDC to use self.panel instead of self
       info.License = wordwrap(licenseText, 500, wx.ClientDC(self.panel))

       # Then we call wx.AboutBox giving it that info object
       wx.AboutBox(info)





licenseText = "blah " * 250 + "\n\n" +"yadda " * 100

if __name__ == '__main__':
    #if len(sys.argv)==3:
    #    ident = sys.argv[1]
    #    dirpath = sys.argv[2]
    #else:
    #    ident =  None
    #    dirpath = None
    myapp = MyApp(0)
    

    myapp.MainLoop()
    