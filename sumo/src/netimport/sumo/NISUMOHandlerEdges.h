/****************************************************************************/
/// @file    NISUMOHandlerEdges.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A handler for SUMO edges
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <netbuild/NLLoadFilter.h>

// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NISUMOHandlerEdges : public SUMOSAXHandler
{
private:
    LoadFilter _loading;
public:
    NISUMOHandlerEdges(NBEdgeCont &ec, NBNodeCont &nc, LoadFilter what);
    ~NISUMOHandlerEdges();
protected:
    void myStartElement(int element, const std::string &name,
                        const Attributes &attrs);
    void myCharacters(int element, const std::string &name,
                      const std::string &chars);
    void myEndElement(int element, const std::string &name);
private:
    void addEdge(const Attributes &attrs);
    NBNode *getNode(const Attributes &attrs, unsigned int id,
                    const std::string &dir, const std::string &name);
private:
    NBEdgeCont &myEdgeCont;
    NBNodeCont &myNodeCont;

};


#endif

/****************************************************************************/

