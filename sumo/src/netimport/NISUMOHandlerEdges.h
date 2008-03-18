/****************************************************************************/
/// @file    NISUMOHandlerEdges.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id:NISUMOHandlerEdges.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for SUMO edges
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
#ifndef NISUMOHandlerEdges_h
#define NISUMOHandlerEdges_h


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
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NISUMOHandlerEdges
 * @brief Importer for SUMO edges
 *
 * @todo rework!
 */
class NISUMOHandlerEdges : public SUMOSAXHandler
{
public:
    NISUMOHandlerEdges(NBEdgeCont &ec, NBNodeCont &nc);
    ~NISUMOHandlerEdges() throw();
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
                        const Attributes &attrs) throw(ProcessError);
    //@}
private:
    void addEdge(const Attributes &attrs);
    NBNode *getNode(const Attributes &attrs, SumoXMLAttr id,
                    const std::string &dir, const std::string &name);
private:
    NBEdgeCont &myEdgeCont;
    NBNodeCont &myNodeCont;

};


#endif

/****************************************************************************/

