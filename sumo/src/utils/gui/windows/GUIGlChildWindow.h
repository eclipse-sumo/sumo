#ifndef GUIGlChildWindow_h
#define GUIGlChildWindow_h

#include <fx.h>
#include <fx3d.h>

#include "GUISUMOAbstractView.h"

class GUIGlChildWindow : public FXMDIChild {
public:
    GUIGlChildWindow( FXMDIClient* p, FXMDIMenu *mdimenu, const FXString& name,
        FXIcon* ic=NULL, FXPopup* pup=NULL,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0
        );

    virtual ~GUIGlChildWindow();

    virtual FXGLCanvas *getBuildGLCanvas() const;

    virtual bool showLegend() const = 0;
    virtual bool allowRotation() const = 0;

    FXToolBar &getToolBar(GUISUMOAbstractView &v);

protected:
    /// the view
    GUISUMOAbstractView *_view;

    /// The tool bar
    FXToolBar *myToolBar;

protected:
    GUIGlChildWindow() { }

};




#endif
