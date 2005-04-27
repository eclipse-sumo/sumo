#ifndef NIXMLTypesHandler_h
#define NIXMLTypesHandler_h
/***************************************************************************
                          NIXMLTypesHandler.h
			  Used to parse the XML-descriptions of types given in a XML-format
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
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
// Revision 1.3  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/06/18 11:17:29  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.1  2003/02/07 11:16:30  dkrajzew
// names changed
//
// Revision 1.1  2002/10/16 15:45:36  dkrajzew
// initial commit for xml-importing classes
//
// Revision 1.4  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:57  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:13  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/sumoxml/SUMOSAXHandler.h>


class NBTypeCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NIXMLTypesHandler : public SUMOSAXHandler {
public:
    /// standard constructor
    NIXMLTypesHandler(NBTypeCont &tc);

    /// destructor
    ~NIXMLTypesHandler();

protected:
    /// called on the opening of a tag; inherited
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

    /// called after reading intermediate characters; inherited
    void myCharacters(int element, const std::string &name,
        const std::string &chars);

    /// called on the closing of a tag; inherited
    void myEndElement(int element, const std::string &name);

private:
	NBTypeCont &myTypeCont;

private:
    /** invalid copy constructor */
    NIXMLTypesHandler(const NIXMLTypesHandler &s);

    /** invalid assignment operator */
    NIXMLTypesHandler &operator=(const NIXMLTypesHandler &s);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
