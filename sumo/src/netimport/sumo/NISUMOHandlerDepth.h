/****************************************************************************/
/// @file    NISUMOHandlerDepth.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A handler for deeper SUMO-import (connections and logics)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NISUMOHandlerDepth_h
#define NISUMOHandlerDepth_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <netbuild/NLLoadFilter.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NISUMOHandlerDepth : public SUMOSAXHandler
{
private:
    LoadFilter _loading;
public:
    NISUMOHandlerDepth(LoadFilter what);
    ~NISUMOHandlerDepth() throw();
protected:
    /// @name inherited from GenericSAXHandler
    //@{ 
    void myStartElement(SumoXMLTag element, const std::string &name,
                        const Attributes &attrs) throw();
    void myCharacters(SumoXMLTag element, const std::string &name,
                      const std::string &chars) throw();
    void myEndElement(SumoXMLTag element, const std::string &name) throw();
    //@}
};


#endif

/****************************************************************************/

