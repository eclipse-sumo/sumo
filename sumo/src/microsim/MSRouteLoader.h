#ifndef MSRouteLoader_h
#define MSRouteLoader_h
/***************************************************************************
                          MSRouteLoader.h
			  A class that performs the loading of routes
                             -------------------
    project              : SUMO
    begin                : Wed, 6 Nov 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.2  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.1  2003/02/07 10:41:51  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <string>
#include <utils/common/FileErrorReporter.h>
#include <microsim/MSNet.h>
#include "MSVehicleContainer.h"
#include "MSRouteHandler.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSRouteLoader
 */
class MSRouteLoader : public MSRouteHandler
{
public:
    /// constructor
    MSRouteLoader(bool verbose, bool warn, const std::string &file,
        MSNet &net);

    /// destructor
    ~MSRouteLoader();

    /** loads vehicles until a vehicle is read that starts after
        the specified time */
    void loadUntil(MSNet::Time time, MSVehicleContainer &into);

    /// resets the reader
    void init();

    /// returns the information whether new data is available
    bool moreAvailable() const;
private:
    /// the used SAX2XMLReader
    SAX2XMLReader* myParser;

    /// the token for saving the current position
    XMLPScanToken  myToken;

    /// information whether more vehicles should be available
    bool _moreAvailable;
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSRouteLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

