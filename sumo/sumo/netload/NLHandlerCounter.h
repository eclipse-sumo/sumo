#ifndef NLHandlerCounter_h
#define NLHandlerCounter_h
/***************************************************************************
                          NLHandlerCounter.h
			  The first-step - handler which counts the given
			  structures
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
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:22  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:43  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:07  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax/HandlerBase.hpp>
#include "NLSAXHandler.h"
#include "NLLoadFilter.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class AttributeList;
class NLContainer;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLHandlerCounter
 * This handler performs the first step of the parsing.
 * It counts all occurences of edges, lanes, junctions, vehicles, vehicle 
 * types and routes
 */
class NLHandlerCounter : public NLSAXHandler {
public:
    /// standard constructor
    NLHandlerCounter(NLContainer &container, LoadFilter filter);
    /// standard destructor
    ~NLHandlerCounter();
    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag; 
        this method counts edges, junctions, lanes, vehicle types, 
        vehicles and routes */
    void myStartElement(int element, const std::string &name, const Attributes &attrs);
    /// returns a message about the processing
    std::string getMessage() const;
    /// changes the type of data to count
    void changeLoadFilter(LoadFilter filter);
private:
    /** invalid copy constructor */
    NLHandlerCounter(const NLHandlerCounter &s);
    /** invalid assignment operator */
    NLHandlerCounter &operator=(const NLHandlerCounter &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerCounter.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
