//---------------------------------------------------------------------------//
//                        GUILoadThread.cpp -
//  Class describing the thread that performs the loading of a simulation
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
// Revision 1.7  2003/06/06 10:33:47  dkrajzew
// changes due to moving the popup-menus into a subfolder
//
// Revision 1.6  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.5  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.4  2003/03/12 16:55:18  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <qthread.h>
#include <iostream>

#include <guisim/GUINet.h>
#include <guinetload/GUINetBuilder.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <sumo_only/SUMOFrame.h>
#include <helpers/SingletonDictionary.h>
#include "QSimulationLoadedEvent.h"
#include "GUIApplicationWindow.h"
#include "GUILoadThread.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUILoadThread::GUILoadThread(GUIApplicationWindow *mw)
    : _parent(mw)
{
}


GUILoadThread::~GUILoadThread()
{
}


void
GUILoadThread::init(const string &file)
{
    _file = file;
}


void GUILoadThread::run()
{
    OptionsCont *oc = SUMOFrame::getOptions();
    oc->set("c", _file);
    GUINet *net = 0;
    std::ostream *craw = 0;
    int simStartTime = 0;
    int simEndTime = 0;
    try {
        SErrorHandler::clearErrorInformation();
        OptionsIO::loadConfiguration(oc);
        GUINetBuilder builder(*oc);
        net = builder.buildGUINet();
        if(net!=0) {
            SUMOFrame::postbuild(*net);
            simStartTime = oc->getInt("b");
            simEndTime = oc->getInt("e");
            craw = SUMOFrame::buildRawOutputStream(oc);
        }
    } catch (UtilException &e) {
        delete net;
        delete craw;
        net = 0;
        craw = 0;
        SingletonDictionary< std::string, MSLaneState* >::remove();
    } catch (XMLBuildingException &e) {
        delete net;
        delete craw;
        net = 0;
        craw = 0;
        SingletonDictionary< std::string, MSLaneState* >::remove();
    }
    delete oc;
    QThread::postEvent( _parent,
        new QSimulationLoadedEvent(net, craw, simStartTime, simEndTime,
        string(_file)) );
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUILoadThread.icc"
//#endif

// Local Variables:
// mode:C++
// End:


