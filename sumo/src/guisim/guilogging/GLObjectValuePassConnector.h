/****************************************************************************/
/// @file    GLObjectValuePassConnector.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GLObjectValuePassConnector_h
#define GLObjectValuePassConnector_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/globjects/GUIGlObject.h>
#include <microsim/MSUpdateEachTimestep.h>
#include <utils/helpers/ValueSource.h>
#include <utils/helpers/ValueRetriever.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObject;

// ===========================================================================
// class definitions
// ===========================================================================
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

    virtual bool updateEachTimestep(void)
    {
        if (!myObject.active()) {
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


#endif

/****************************************************************************/

