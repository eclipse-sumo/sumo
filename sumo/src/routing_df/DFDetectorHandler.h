/****************************************************************************/
/// @file    DFDetectorHandler.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// missing_desc
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
#ifndef DFDetectorHandler_h
#define DFDetectorHandler_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/options/OptionsCont.h>
#include "DFDetector.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DFDetectorHandler
 * @brief SAX2-Handler for loading DFROUTER-detector definitions
 */
class DFDetectorHandler : public SUMOSAXHandler
{
public:
    /// Constructor
    DFDetectorHandler(OptionsCont &oc, DFDetectorCon &con);

    /// Destrcutor
    virtual ~DFDetectorHandler();

protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
                        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
                      const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);

protected:
    /// the options (program settings)
    OptionsCont &myOptions;

    DFDetectorCon &myContainer;

private:
    /// invalidated copy constructor
    DFDetectorHandler(const DFDetectorHandler &src);

    /// invalidated assignment operator
    DFDetectorHandler &operator=(const DFDetectorHandler &src);

};


#endif

/****************************************************************************/

