#ifndef ODDistrictHandler_h
#define ODDistrictHandler_h
//---------------------------------------------------------------------------//
//                        ODDistrictHandler.h -
//  The XML-Handler for district loading
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
// Revision 1.3  2003/06/18 11:20:24  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/07 10:44:19  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utility>
#include <utils/sumoxml/SUMOSAXHandler.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class ODDistrict;
class ODDistrictCont;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ODDistrictHandler
 * A XML-handler to load districts from a sumo network file.
 */
class ODDistrictHandler : public SUMOSAXHandler {
public:
    /// Constructor
    ODDistrictHandler(ODDistrictCont &cont);

    /// Destructor
    ~ODDistrictHandler();

protected:
    /// Called when an opening-tag occures
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /// Called when a closing tag occures
    void myEndElement(int element, const std::string &name);

    /// Called on characters
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

private:
    /// Begins the parsing of a district
    void openDistrict(const Attributes &attrs);

    /// Adds a read source to the current district
    void addSource(const Attributes &attrs);

    /// Adds a read sink to the current district
    void addSink(const Attributes &attrs);

    /// Closes the processing of the current district
    void closeDistrict();

    /// Returns the values for a sink/source (name and weight)
    std::pair<std::string, double> getValues(const Attributes &attrs,
        const std::string &type);

private:
    /// The container to add read districts to
    ODDistrictCont &_cont;

    /// The currently parsed district
    ODDistrict *_current;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ODDistrictHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

