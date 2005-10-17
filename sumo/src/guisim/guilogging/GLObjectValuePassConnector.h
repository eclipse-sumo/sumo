#ifndef GLObjectValuePassConnector_h
#define GLObjectValuePassConnector_h
//---------------------------------------------------------------------------//
//                        GLObjectValuePassConnector.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.6  2005/10/17 08:54:37  dkrajzew
// memory leaks removed
//
// Revision 1.5  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:06:13  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:16:10  dkrajzew
// level 3 warnings removed; code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/gui/globjects/GUIGlObject.h>
#include <microsim/MSUpdateEachTimestep.h>
#include <utils/helpers/ValueSource.h>
#include <utils/helpers/ValueRetriever.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObject;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<typename T>
class GLObjectValuePassConnector :
    public MSUpdateEachTimestep<GLObjectValuePassConnector<T> >
{
public:
    GLObjectValuePassConnector(GUIGlObject &o,
        ValueSource<T> *source,
        ValueRetriever<T> *retriever)
        : myObject(o), mySource(source), myRetriever(retriever)
        { }

    virtual ~GLObjectValuePassConnector()
    {
        delete mySource;
    }

    virtual bool updateEachTimestep( void ) {
        if(!myObject.active()) {
            return false;
        }
        myRetriever->addValue(mySource->getValue());
        return true;
    }

private:
    GUIGlObject &myObject;
    ValueSource<T> *mySource;
    ValueRetriever<T> *myRetriever;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
