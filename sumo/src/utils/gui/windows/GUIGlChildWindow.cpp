#include "GUIGlChildWindow.h"


GUIGlChildWindow::GUIGlChildWindow( FXMDIClient* p,
                                   FXMDIMenu *mdimenu, const FXString& name,
                                   FXIcon* ic, FXPopup* pup,
                                   FXuint opts,FXint x,FXint y,FXint w,FXint h)
    : FXMDIChild( p, name, ic, mdimenu, opts, 10, 10, 300, 200 ),
    _view(0)
{
}


GUIGlChildWindow::~GUIGlChildWindow()
{
}


FXGLCanvas *
GUIGlChildWindow::getBuildGLCanvas() const
{
    return _view;
}


FXToolBar &
GUIGlChildWindow::getToolBar(GUISUMOAbstractView &v)
{
    return *myToolBar;
}


