#ifndef NLHandlerEdgeAllocator_h
#define NLHandlerEdgeAllocator_h
/***************************************************************************
                          NLHandlerEdgeAllocator.h
			  The second-step - handler that is responsible for 
			  the allocation of edges
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
// Revision 2.0  2002/02/14 14:43:23  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:43  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:08  traffic
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

/* =========================================================================
 * class declarations
 * ======================================================================= */
class AttributeList;
class NLContainer;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLHandlerEdgeAllocater
 * This is the handler for the second step of the parsing 
 * Only allocation of MSEdge-instances is done.
 */
class NLHandlerEdgeAllocator : public NLSAXHandler {
public:
    /// standard constructor
    NLHandlerEdgeAllocator(NLContainer *container);
    /// standard destructor
    ~NLHandlerEdgeAllocator();
    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag; 
        this method allocates undescribed MSEdges which were counted by 
        "NLHandlerCounter" previously */
    void startElement(const XMLCh* const name, AttributeList& attributes);
private:
    /** invalid copy constructor */
    NLHandlerEdgeAllocator(const NLHandlerEdgeAllocator &s);
    /** invalid assignment operator */
    NLHandlerEdgeAllocator &operator=(const NLHandlerEdgeAllocator &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerEdgeAllocator.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

