#ifndef NLHandlerJunctionLogics_h
#define NLHandlerJunctionLogics_h
/***************************************************************************
                          NLHandlerJunctionLogics.h
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
// Revision 2.1  2002/03/15 09:36:38  traffic
// New declaration of data members now allows usage by derived classes; handling of map logics removed; unnecessary data members removed
//
// Revision 2.0  2002/02/14 14:43:23  croessel
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
#include "NLSAXHandler.h"

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
 * NLHandlerJunctionLogics
 */
class NLHandlerJunctionLogics : public NLSAXHandler {
protected:
    /// the current file
    std::string    m_File;
    /// the current key
    std::string    m_Key;
    
public:
    /// standard constructor
    NLHandlerJunctionLogics(NLContainer *container);
    /// standard destructor
    virtual ~NLHandlerJunctionLogics();

    /// prepares for the usage on the next file
    virtual void init(std::string key, std::string file);
    /** returns the build logic */
    virtual MSJunctionLogic *build() = 0;
protected:
  std::vector<bool> *convertBoolVector(const char *str);
private:
    /** invalid copy constructor */
    NLHandlerJunctionLogics(const NLHandlerJunctionLogics &s);
    /** invalid assignment operator */
    NLHandlerJunctionLogics &operator=(const NLHandlerJunctionLogics &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLHandlerJunctionLogics.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
