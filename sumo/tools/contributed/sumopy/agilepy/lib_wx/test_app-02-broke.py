 
import wx
from wx.lib.wordwrap import wordwrap

from mainframe import *
from test_glcanvas import *

##
##import wx
##
##import os
##import sys
##
##try:
##    dirName = os.path.dirname(os.path.abspath(__file__))
##except:
##    dirName = os.path.dirname(os.path.abspath(sys.argv[0]))
##
##sys.path.append(os.path.split(dirName)[0])

IMAGEDIR = os.path.join(os.path.dirname(__file__),"images")

class MyApp(wx.App):
    def __init__(self, title, filename = None, redirect=False, iconfilepath = None):
        wx.App.__init__(self, redirect, filename)
        self.mainframe = AgileMainframe(title=title)
        #self._icon = icon
        
        #
        if iconfilepath != None:
            icon = wx.Icon(iconfilepath,wx.BITMAP_TYPE_PNG, 16,16)
            self.mainframe.SetIcon(icon)
            
        #def OnInit(self):   
        #splitter = MainSplitter(self.mainframe)
        #gleditor = splitter.add_view("GLeditor", GlEditor)
        self.mainframe.Show()
        #canvas = gleditor.get_canvas()
        #canvas.add_element(lines)
        #canvas.add_element(triangles)
        #canvas.add_element(rectangles)
        
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
    iconfilepath = os.path.join(IMAGEDIR,'icon_color_small.png')
    app = MyApp('TestApp', iconfilepath)
    app.MainLoop()
    