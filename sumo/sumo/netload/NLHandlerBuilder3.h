#ifndef NLHandlerBuilder3_h
#define NLHandlerBuilder3_h
/***************************************************************************
                          NLHandlerBuilder3.h
			  The fifth-step - handler building remaining 
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
// Revision 1.1  2001/12/06 13:36:07  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
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
 * NLHandlerBuilder3
 * This class performs the fifth and last step of the XML-parsing
 * Lanes are connected with their succeeding edges vai jucntions
 */
class NLHandlerBuilder3 : public NLSAXHandler {
private:
    /** the information whether only dynamic parts 
        (not the network shall be parsed) */
    bool    m_bDynamicOnly; 
    /** storage for currently build lane */
    std::string m_LaneId;
public:
    /// standard constructor
    NLHandlerBuilder3(NLContainer *container, bool dynamicOnly);
    /// standard destructor
    ~NLHandlerBuilder3();
    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag; 
        this method 
        a) opens a junction for the addition of elements describing it 
        b) adds right-of-way-logic items to it or 
        c) allocates new vehicles in dependence of the occured tag */
    void startElement(const XMLCh* const name, AttributeList& attributes);
    /** called on the end of an element; 
        this method closes the processing of a previously chosen junction */
    void endElement(const XMLCh* const name);
private:
    /** invalid copy constructor */
    NLHandlerBuilder3(const NLHandlerBuilder3 &s);
    /** invalid assignment operator */
    NLHandlerBuilder3 &operator=(const NLHandlerBuilder3 &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerBuilder3.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
