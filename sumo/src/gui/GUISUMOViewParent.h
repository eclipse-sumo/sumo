#ifndef GUISUMOViewParent_h
#define GUISUMOViewParent_h
//---------------------------------------------------------------------------//
//                        GUISUMOViewParent.h -
//  A window that controls the display(s) of the simulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.3  2003/09/05 14:45:44  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.2  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.1  2003/05/20 09:25:14  dkrajzew
// new view hierarchy; some debugging done
//
// Revision 1.4  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.3  2003/04/14 08:24:57  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <qmainwindow.h>
#include <qworkspace.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundery.h>
#include "GUIChooser.h"
#include "GUIGlObjectTypes.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;
class QToolBar;
class GUISUMOAbstractView;
class QGUIToggleButton;
class GUIApplicationWindow;


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * This class represents a single view on the application. It is made of a
 * tool-bar containing a field to change the type of display, buttons that
 * allow to choose an artifact and some other view controlling options.
 * The rest of the window is a canvas that contains the display itself
 */
class GUISUMOViewParent : public QMainWindow
{
    // is a q-object
    Q_OBJECT
public:
    enum ViewType {
        MICROSCOPIC_VIEW,
        LANE_AGGREGATED_VIEW
    };

    /// constructor
    GUISUMOViewParent( QWidget *parent, const char* name, int wflags,
        GUINet &net, GUIApplicationWindow &parentWindow, ViewType view);

    /// destructor
    ~GUISUMOViewParent();

    /// not implemented: shall load a view definition
    void load( const QString& fn );

    /// not implemented: shall save a view definition
    void save();

    /// not implemented: "save as" a view definition
    void saveAs();

    /// not implemented: print a view
    void print( QPrinter* );


    /// centers the view onto the given artifact
    void setView(GUIGlObjectType type, const std::string &name);

    /// returns the zooming factor
    double getZoomingFactor() const;

    /// sets the zooming factor
    void setZoomingFactor(double val);

    /// returns the information whether the legend shall be shown
    bool showLegend() const;

    /// information whether the canvas may be rotated
    bool allowRotation() const;

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

public slots:
    /// called when the user presses the "choose junction"-button
    void chooseJunction();

    /// called when the user presses the "choose edge"-button
    void chooseEdge();

    /// called when the user presses the "choose vehicle"-button
    void chooseVehicle();

    /** called when the user wants to recenter the view
        (presses the "recenter view"-button) */
    void recenterView();

    /// called when the user toggles the "show legend"-button
    void toggleShowLegend();

    /// called when the user toggles the "allow rotation"-button
    void toggleAllowRotation();

    /// switches behaviour 1 on
    void toggleBehaviour1();

    /// switches behaviour 2 on
    void toggleBehaviour2();

    /// switches behaviour 3 on
    void toggleBehaviour3();

protected:
    /// event handler (especially for own events)
    bool event(QEvent *e);

signals:
    /// displays the given message into the status bar
    void message(const QString&, int );

private:
    /** builds the toolbar containing the window settings
	(display type - combo box, zomming factor) */
    void buildSettingTools();

    /** builds the toolbar containing the view settings
	("recenter view"-button, "show legend"-button, "allow rotation"-button,
	behaviour toggler) */
    void buildViewTools();

    /** build the artifact choosing toolbar */
    void buildTrackingTools();

    /** build the GUIChooser which contains the given values */
    void showValues(GUIGlObjectType type,
        std::vector<std::string> &names);

private:
    /// the zooming factor
    double _zoomingFactor;

    /// ??
    QString filename;

    /// the view used
    GUISUMOAbstractView *_view;

    /// toolbars (view, tracking)
    QToolBar *_viewTools, *_trackingTools;

    /// view-toolbar toggle buttons
    QGUIToggleButton *_showLegendToggle, *_allowRotationToggle;

    /// behaviour-toggling buttons
    QGUIToggleButton *_behaviourToggle1, *_behaviourToggle2,
        *_behaviourToggle3;

    /// information whether the legend shall be shown
    bool _showLegend;

    /// information whether the canvas may be rotated
    bool _allowRotation;

    /// the artifact chooser
    GUIChooser *_chooser;

    /// The parent window
    GUIApplicationWindow &myParent;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUISUMOViewParent.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

