#ifndef AttributesReadingGenericSAX2Handler_h
#define AttributesReadingGenericSAX2Handler_h
//---------------------------------------------------------------------------//
//                        AttributesReadingGenericSAX2Handler.h -
//  A combination between a GenericSAXHandler and an AttributesHandler
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2003/02/07 10:53:52  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "GenericSAX2Handler.h"
#include "AttributesHandler.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class AttributesReadingGenericSAX2Handler
 * This class is a combination of an AttributesHandler and a
 * GenericSAX2Handler allowing direct access to and handling of XML-tags
 * (elements) and attributes.
 * By now, almost every class that handles XML-data is derived from this
 * class.
 */
class AttributesReadingGenericSAX2Handler : public AttributesHandler,
                                            public GenericSAX2Handler {
public:
    /** constructor */
    AttributesReadingGenericSAX2Handler() { }

    /** parametrised constructor */
    AttributesReadingGenericSAX2Handler(GenericSAX2Handler::Tag *tags,
        int noTags, AttributesHandler::Attr *attrs, int noAttrs)
        : GenericSAX2Handler(tags, noTags),
        AttributesHandler(attrs, noAttrs) { }

    /** destructor */
    virtual ~AttributesReadingGenericSAX2Handler() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "AttributesReadingGenericSAX2Handler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

