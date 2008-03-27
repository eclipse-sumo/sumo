/****************************************************************************/
/// @file    NIOSMNodesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id:NIOSMNodesHandler.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Used to load the XML-description of the nodes given in a
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
#ifndef NIOSMNodesHandler_h
#define NIOSMNodesHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/geom/Position2D.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class NBNode;
class NBNodeCont;
class NBTrafficLightLogicCont;


// ===========================================================================
// class definitions
// ===========================================================================
struct NIOSMNode {
    int id;
    SUMOReal lon;
    SUMOReal lat;
};
/**
 * @class NIOSMNodesHandler
 * A class that parses nodes from XML-files
 */
class NIOSMNodesHandler
            : public SUMOSAXHandler
{

public:
    /// standard constructor
    NIOSMNodesHandler(std::map<int, NIOSMNode*> &toFill,
                      OptionsCont &options);

    /// Destructor
    ~NIOSMNodesHandler() throw();

protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}

private:
    /// A reference to the program's options
    OptionsCont &myOptions;

    std::map<int, NIOSMNode*> &myToFill;

private:
    /** invalid copy constructor */
    NIOSMNodesHandler(const NIOSMNodesHandler &s);

    /** invalid assignment operator */
    NIOSMNodesHandler &operator=(const NIOSMNodesHandler &s);

};


#endif

/****************************************************************************/

