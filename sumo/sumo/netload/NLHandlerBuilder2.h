#ifndef NLHandlerBuilder2_h
#define NLHandlerBuilder2_h
/***************************************************************************
                          NLHandlerBuilder2.h
			  The fourth-step - handler building remaining 
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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:22  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:42  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:06  traffic
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
 * NLHandlerBuilder2
 * This class performs the fourth of the XML-parsing
 * Here the following parsing and allocating is done:
 * - parsing and allocating of MSJunction-instances
 * - parsing and allocating of MSVehicle-instances
 */
class NLHandlerBuilder2 : public NLSAXHandler {
private:
    bool    m_bDynamicOnly; 
public:
    /// standard constructor
    NLHandlerBuilder2(NLContainer *container, bool dynamicOnly);
    /// standard destructor
    ~NLHandlerBuilder2();
    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag; 
        a) this method opens a junction for the addition of elements 
           describing it 
        b) adds right-of-way-logic items to it or 
        c) allocates new vehicles in dependence of the occured tag */
    void startElement(const XMLCh* const name, AttributeList& attributes);
    /** called on the end of an element; 
        this method closes the processing of a previously chosen junction */
    void endElement(const XMLCh* const name);
    /** called when simple characters occure; 
        this method adds incoming lanes to the previously selected junction */
    void characters(const XMLCh* const chars, const unsigned int length);
private:
    /** invalid copy constructor */
    NLHandlerBuilder2(const NLHandlerBuilder2 &s);
    /** invalid assignment operator */
    NLHandlerBuilder2 &operator=(const NLHandlerBuilder2 &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerBuilder2.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

