//---------------------------------------------------------------------------//
//                        GUISUMOViewParent.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.4  2003/07/18 12:29:28  dkrajzew
// removed some warnings
//
// Revision 1.3  2003/07/16 15:18:23  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.2  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.1  2003/05/20 09:25:14  dkrajzew
// new view hierarchy; some debugging done
//
// Revision 1.6  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.5  2003/04/14 08:24:56  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
// Revision 1.4  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/12 16:55:18  dkrajzew
// centering of objects debugged
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

#include <utils/geom/Position2D.h>
#include <utils/geom/Boundery.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSJunction.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>

#include <string>
#include <vector>
#include <qwidget.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qworkspace.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>
#include <qstring.h>
#include <qapplication.h>
#include <qspinbox.h>
#include "icons/locate_junction.xpm"
#include "icons/locate_edge.xpm"
#include "icons/locate_vehicle.xpm"
#include "icons/recenter_view.xpm"
#include "icons/show_legend.xpm"
#include "icons/allow_rotation.xpm"
#include "icons/view1.xpm"
#include "icons/view2.xpm"
#include "icons/view3.xpm"
#include "GUIChooser.h"
#include "GUIViewTraffic.h"
#include "QGUIToggleButton.h"
#include "GUIApplicationWindow.h"
#include "GUISUMOViewParent.h"
#include "GUIGlObjectTypes.h"

#ifndef WIN32
#include "GUISUMOViewParent.moc"
#endif



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUISUMOViewParent::GUISUMOViewParent( QWidget *parent, const char* name,
                                     int wflags,
                                     GUINet &net,
                                     GUIApplicationWindow &parentWindow )
    : QMainWindow( parent, name, wflags ), _zoomingFactor(100),
    _view(0), _viewTools(0), _trackingTools(0),
    _showLegendToggle(0), _allowRotationToggle(0),
    _behaviourToggle1(0), _behaviourToggle2(0), _behaviourToggle3(0),
    _showLegend(true), _allowRotation(false), _chooser(0),
    myParent(parentWindow)
{
    // build the tool bar
    buildViewTools();
    buildTrackingTools();
    // set the size
    setMinimumSize(100, 30);
    setBaseSize(300, 300);
    // build the display widget
    _view = new GUIViewTraffic(myParent, *this, net);
    setCentralWidget(_view);
    _view->buildViewToolBars(*this);
}


GUISUMOViewParent::~GUISUMOViewParent()
{
}


void
GUISUMOViewParent::buildViewTools()
{
    // build the tooolbar
    _viewTools = new QToolBar( this, "view settings" );
    addToolBar( _viewTools, tr( "View Settings" ), Top, FALSE );
    // add the recenter - button
    QPixmap icon = QPixmap( recenter_view_xpm );
    new QToolButton( icon, "Recenter View",
        QString::null, this, SLOT(recenterView()), _viewTools,
        "recenter view" );
    // add a separator
    _viewTools->addSeparator();
    // add the legend-toggle button
    icon = QPixmap( show_legend_xpm );
    _showLegendToggle = new QGUIToggleButton( icon, "Show Legend",
        QString::null, this, SLOT(toggleShowLegend()), _viewTools,
        "show legend", true );
    icon = QPixmap( allow_rotation_xpm );
    _allowRotationToggle = new QGUIToggleButton( icon, "Allow Rotation",
        QString::null, this, SLOT(toggleAllowRotation()), _viewTools,
        "allow rotation", true );
    // !!! add "what's this"
    // add a separator
    _viewTools->addSeparator();
    // add the behaviour toggling buttons
    icon = QPixmap( view1_xpm );
    _behaviourToggle1 = new QGUIToggleButton( icon, "Toggle View Behaviour 1",
        QString::null, this, SLOT(toggleBehaviour1()), _viewTools,
        "toggle view behaviour 1", true );
    icon = QPixmap( view2_xpm );
    _behaviourToggle2 = new QGUIToggleButton( icon, "Toggle View Behaviour 2",
        QString::null, this, SLOT(toggleBehaviour2()), _viewTools,
        "toggle view behaviour 2", false );
    icon = QPixmap( view3_xpm );
    _behaviourToggle3 = new QGUIToggleButton( icon, "Toggle View Behaviour 3",
        QString::null, this, SLOT(toggleBehaviour3()), _viewTools,
        "toggle view behaviour 3", false );
}


void
GUISUMOViewParent::buildTrackingTools()
{
    // build the tooolbar
    _trackingTools = new QToolBar( this, "tracking settings" );
    addToolBar( _trackingTools, tr( "Tracking Settings" ), Top, FALSE );
    // add the junction locator start
    QPixmap icon = QPixmap( locate_junction_xpm );
    QToolButton *button = new QToolButton( icon, "Locate Junction",
        QString::null, this, SLOT(chooseJunction()), _trackingTools,
        "locate junction" );
    // add the edge locator start
    icon = QPixmap( locate_edge_xpm );
    button = new QToolButton( icon, "Locate Edge",
        QString::null, this, SLOT(chooseEdge()), _trackingTools,
        "locate edge" );
    // add the vehicle locator start
    icon = QPixmap( locate_vehicle_xpm );
    button = new QToolButton( icon, "Locate Vehicle",
        QString::null, this, SLOT(chooseVehicle()), _trackingTools,
        "locate vehicle" );
    // !!! add "what's this"
}


double
GUISUMOViewParent::getZoomingFactor() const
{
    return _zoomingFactor;
}


void
GUISUMOViewParent::setZoomingFactor(double val)
{
    _zoomingFactor = val;
}


void
GUISUMOViewParent::chooseJunction()
{
    vector<string> names = MSJunction::getNames();
    showValues(GLO_JUNCTION, names);
}


void
GUISUMOViewParent::chooseEdge()
{
    vector<string> names = GUIEdge::getNames();
    showValues(GLO_EDGE, names);
}


void
GUISUMOViewParent::chooseVehicle()
{
    vector<string> names = GUIVehicle::getNames();
    showValues(GLO_VEHICLE, names);
}


void
GUISUMOViewParent::showValues(GUIGlObjectType type,
                              std::vector<std::string> &names)
{
    GUIChooser *chooser = new GUIChooser(this, type, names);
    chooser->show();
}


void
GUISUMOViewParent::setView(GUIGlObjectType type,
                           const std::string &name)
{
    _view->centerTo(type, name);
}


void
GUISUMOViewParent::recenterView()
{
    _zoomingFactor = 100;
    _view->recenterView();
    _view->update();
}


void
GUISUMOViewParent::load( const QString& fn )
{
    filename  = fn;
    QFile f( filename );
    if ( !f.open( IO_ReadOnly ) )
	return;

    if(fn.contains(".gif")) {
	QWidget * tmp=new QWidget(this);
	setFocusProxy(tmp);
	setCentralWidget(tmp);
#ifdef _WS_QWS_ // temporary speed-test hack
	qm->setDisplayWidget(tmp);
#endif
	tmp->setBackgroundMode(QWidget::NoBackground);
	tmp->show();
    } else {
    }
    setCaption( filename );
    emit message( QString("Loaded document %1").arg(filename), 2000 );
}


void
GUISUMOViewParent::save()
{
    if ( filename.isEmpty() ) {
        saveAs();
        return;
    }

    QFile f( filename );
    if ( !f.open( IO_WriteOnly ) ) {
        emit message( QString("Could not write to %1").arg(filename),
		      2000 );
        return;
    }
    setCaption( filename );
    emit message( QString( "File %1 saved" ).arg( filename ), 2000 );
}


void
GUISUMOViewParent::saveAs()
{
    QString fn = QFileDialog::getSaveFileName( filename, QString::null, this );
    if ( !fn.isEmpty() ) {
        filename = fn;
        save();
    } else {
        emit message( "Saving aborted", 2000 );
    }
}


void
GUISUMOViewParent::print( QPrinter* )
{
}


void
GUISUMOViewParent::toggleShowLegend()
{
    _showLegend = _showLegendToggle->isOn();
    _view->update();
}


bool
GUISUMOViewParent::showLegend() const
{
    return _showLegend;
}


void
GUISUMOViewParent::toggleAllowRotation()
{
    _allowRotation = !_allowRotationToggle->isOn();
    _view->update();
}


bool
GUISUMOViewParent::allowRotation() const
{
    return _allowRotation;
}


bool
GUISUMOViewParent::event ( QEvent *e )
{
    if(e->type()==QEvent::User) {
        _view->update();
        return TRUE;
    }
    return QMainWindow::event(e);
}


void
GUISUMOViewParent::toggleBehaviour1()
{
    _behaviourToggle2->publicSetOn(false);
    _behaviourToggle3->publicSetOn(false);
}


void
GUISUMOViewParent::toggleBehaviour2()
{
    _behaviourToggle1->publicSetOn(false);
    _behaviourToggle3->publicSetOn(false);
}


void
GUISUMOViewParent::toggleBehaviour3()
{
    _behaviourToggle1->publicSetOn(false);
    _behaviourToggle2->publicSetOn(false);
}


int
GUISUMOViewParent::getMaxGLWidth() const
{
    return myParent.getMaxGLWidth();
}

int
GUISUMOViewParent::getMaxGLHeight() const
{
    return myParent.getMaxGLHeight();
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUISUMOViewParent.icc"
//#endif

// Local Variables:
// mode:C++
// End:


