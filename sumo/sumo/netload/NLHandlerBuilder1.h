#ifndef NLHandlerBuilder1_h
#define NLHandlerBuilder1_h
/***************************************************************************
                          NLHandlerBuilder1.h
			  The third-step - handler building structures
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 2.0  2002/02/14 14:43:21  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:41  croessel
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
 * NLHandlerBuilder1
 * This is the handler performing the third step of the parsing of the 
 * XML-file used.
 * It is used for:
 * - parsing the data of previously allocated instances of the MSEdge-class
 * - parsing and allocating MSVehicleType-instances
 * - parsing and allocating MSNet::Route-instances
 */
class NLHandlerBuilder1 : public NLSAXHandler {
private:
    /** the information whether only dynamic parts 
        (not the network shall be parsed) */
    bool    m_bDynamicOnly;
public:
    /// standard constructor
    NLHandlerBuilder1(NLContainer *container, bool dynamicOnly);
    /// standard destructor
    ~NLHandlerBuilder1();
    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag; 
        this method 
        a) opens edges for the addition of their elements 
        b) adds lanes to the edges 
        c) opens descriptions of connections between edges 
        d) adds vehicle-types or 
        e) adds routes in dependence of the occured tag */
    void startElement(const XMLCh* const name, AttributeList& attributes);
    /** called on the end of an element; 
        this method 
        a) builds single edges by closing their description 
        b) closes the addition of lanes to an edge 
        c) closes the addition of connections to a previously chosen following
           edge or 
        d) closes the generation of a route in dependence of the occured tag*/
    void endElement(const XMLCh* const name);
    /** called when simple characters occure; this method 
        a) adds lanes connecting the previously chosen current edge with 
           the previously chosen following edge 
        b) adds edges to a route in dependence of the occured tag */
    void characters(const XMLCh* const chars, const unsigned int length);
private:
    /** invalid copy constructor */
    NLHandlerBuilder1(const NLHandlerBuilder1 &s);
    /** invalid assignment operator */
    NLHandlerBuilder1 &operator=(const NLHandlerBuilder1 &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerBuilder1.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
