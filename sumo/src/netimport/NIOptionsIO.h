#ifndef NIOptionsIO_h
#define NIOptionsIO_h
/***************************************************************************
                          NIOptionsIO.h
			  A class for the initialisation, input and veryfying of the
        programs options
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
// Revision 1.2  2005/04/27 12:24:24  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2003/12/11 06:16:37  dkrajzew
// NBOptionsIO moved to netimport NIOptionsIO where they belong to
//
// Revision 1.3  2003/06/24 08:09:29  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.4  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.2  2002/04/09 12:23:09  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * class definitions
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * NIOptionsIO
 * This class is used to build the container of possible commands
 * (OptionsCont-typed) and process reading of options from the command
 * line and the configuration file. The options are also veryfied inside
 * this class
 */
class NIOptionsIO
{
public:
    static void fillOptions(OptionsCont &oc);

    static bool checkOptions(OptionsCont &oc);

private:

    /** checks whether the given option settings are ok */
    static bool check(OptionsCont &oc);

    /** checks whether all needed informations are given */
    static bool checkCompleteDescription(OptionsCont &oc);

    /** checks whether the node file definition is given */
    static bool checkNodes(OptionsCont &oc);

    /** checks whether the edge file definition is given */
    static bool checkEdges(OptionsCont &oc);

    /** checks whether the output can be build */
    static bool checkOutput(OptionsCont &oc);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
