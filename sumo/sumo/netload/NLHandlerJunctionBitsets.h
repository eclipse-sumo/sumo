#ifndef NLHandlerJunctionBitsets_h
#define NLHandlerJunctionBitsets_h
/***************************************************************************
                          NLHandlerJunctionBitsets.h
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
// Revision 2.2  2002/03/15 09:34:12  traffic
// Loading of logics debugged (trafo-matrix is now loaded correctly)
//
// Revision 2.1  2002/02/21 19:49:09  croessel
// MSVC++ Linking-problems solved, hopefully.
//
// Revision 2.0  2002/02/14 14:43:23  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.2  2002/02/13 16:32:17  croessel
// Changed MSBitSet to predefined type MSBitset.
//
// Revision 1.1  2002/02/13 15:40:43  croessel
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
#include <bitset>
#include <sax/HandlerBase.hpp>
#include "NLHandlerJunctionLogics.h"
#include "../microsim/MSBitSetLogic.h"

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
 * NLHandlerJunctionBitsets
 */
class NLHandlerJunctionBitsets : public NLHandlerJunctionLogics {
private:
    /// the right-of-way-logic of the currently chosen bitset-logic
    MSBitsetLogic::Logic   *m_pActiveLogic;
    /// the transformation matrix of the bitset-logic currently chosen
    MSBitsetLogic::Link2LaneTrafo   *m_pActiveTrafo;
    /// the size of the request
    int     _requestSize;
    /// the size of the response
    int     _responseSize;
    /// the number of lanes
    int     _laneNo;
    /// inserted items
    size_t _trafoItems, _requestItems;
public:
    /// standard constructor
    NLHandlerJunctionBitsets(NLContainer *container);
    /// standard destructor
    ~NLHandlerJunctionBitsets();

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
    void addLogicItem(int request, std::string response);
    void addTrafoItem(std::string links, int lane);

private:
    /** invalid copy constructor */
    NLHandlerJunctionBitsets(const NLHandlerJunctionBitsets &s);
    /** invalid assignment operator */
    NLHandlerJunctionBitsets &operator=(const NLHandlerJunctionBitsets &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerJunctionBitsets.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
