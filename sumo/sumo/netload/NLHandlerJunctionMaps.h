#ifndef NLHandlerJunctionMaps_h
#define NLHandlerJunctionMaps_h
/***************************************************************************
                          NLHandlerJunctionMaps.h
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:24  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:40:44  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:09  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <sax/HandlerBase.hpp>
#include "NLHandlerJunctionLogics.h"
#include "../microsim/MSMapLogic.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class AttributeList;
class NLContainer;
class MSJunctionLogic;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLHandlerJunctionMaps
 */
class NLHandlerJunctionMaps : public NLHandlerJunctionLogics {
private:
    /// the current file
    std::string    m_File;
    /// the current key
    std::string    m_Key;
    /// the right-of-way-logic of the currently chosen junction
    MSMapLogic::Logic   *m_pActiveLogic;
    /// the number of links and inlanes
    unsigned int _links, _inlanes;
public:
    /// standard constructor
    NLHandlerJunctionMaps(NLContainer *container);
    /// standard destructor
    ~NLHandlerJunctionMaps();

    /// prepares for the usage on the next file
    void init(std::string key, std::string file);
    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag; */
    void startElement(const XMLCh* const name, AttributeList& attributes);
    /** called when simple characters occure; */
    void characters(const XMLCh* const chars, const unsigned int length);
    /** returns the build logic */
    MSJunctionLogic *build();
private:
    /** converts and adds a logic item to the current logic */
    void addMapLogic(const char *request, const char *respond);
private:
    /** invalid copy constructor */
    NLHandlerJunctionMaps(const NLHandlerJunctionMaps &s);
    /** invalid assignment operator */
    NLHandlerJunctionMaps &operator=(const NLHandlerJunctionMaps &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerJunctionMaps.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
