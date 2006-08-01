#ifndef PCTypeDefHandler_h
#define PCTypeDefHandler_h
/***************************************************************************
                          PCTypeDefHandler.h
    A handler for loading polygon type maps
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2006/08/01 07:52:47  dkrajzew
// polyconvert added
//
// Revision 1.3  2006/03/17 09:04:18  dksumo
// class-documentation added/patched
//
// Revision 1.2  2006/01/26 11:24:17  dksumo
// debugging building under linux
//
// Revision 1.1  2006/01/16 13:45:21  dksumo
// initial work on the dfrouter
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

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/options/OptionsCont.h>
#include "PCTypeDefHandler.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class PCTypeToDef;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class PCTypeDefHandler
 * @brief A handler for loading polygon type maps
 */
class PCTypeDefHandler : public SUMOSAXHandler {
public:
    /// Constructor
    PCTypeDefHandler(OptionsCont &oc, PCTypeMap &con);

    /// Destrcutor
    virtual ~PCTypeDefHandler();


protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);


protected:
    /// the options (program settings)
    OptionsCont &myOptions;

    /// The type map to fill
	PCTypeMap &myContainer;


private:
    /// invalidated copy constructor
    PCTypeDefHandler(const PCTypeDefHandler &src);

    /// invalidated assignment operator
    PCTypeDefHandler &operator=(const PCTypeDefHandler &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

