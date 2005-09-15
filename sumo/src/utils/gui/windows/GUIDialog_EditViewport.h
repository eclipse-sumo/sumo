#ifndef GUIDialog_EditViewport_H
#define GUIDialog_EditViewport_H

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fx.h>

#include <utils/foxtools/FXRealSpinDial.h>

class GUISUMOAbstractView;

class GUIDialog_EditViewport : public FXDialogBox
{
    // FOX-declarations
    FXDECLARE(GUIDialog_EditViewport)
public:
    GUIDialog_EditViewport(GUISUMOAbstractView* parent,  const char* name,
        double zoom, double xoff, double yoff, int x, int y);
    ~GUIDialog_EditViewport();
    void create();
    long onCmdOk(FXObject*,FXSelector,void*);
    long onCmdCancel(FXObject*,FXSelector,void*);

protected:
    GUISUMOAbstractView *myParent;
    double myOldZoom;
    double myOldXOff, myOldYOff;
    FXRealSpinDial *myZoom, *myXOff, *myYOff;

protected:
    GUIDialog_EditViewport() { }

};

#endif
