#ifndef OptionsIO_h
#define OptionsIO_h
/***************************************************************************
                          OptionsIO.h
			  Loads the configuration file using "OptionsLoader"
			  and parses the given command line arguments using
			  "OptionsParser"
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
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
// Revision 1.2  2002/06/11 13:43:37  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.1  2002/03/20 08:20:57  dkrajzew
// New configuration-default handling
//
// Revision 2.0  2002/02/14 14:43:28  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:19  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * OptionsIO
 * A wrapper for the complete IO of options containing the reading of a
 * configuration file and the parsing of the command line arguments.
 * The only accessable method returns fale, when something failed during the
 * process. This may happen when the configuration file is broken or its
 * or the command line arguments syntax is invalid. This may also happen
 * when the configuration or the command line arguments do specify a value
 * more than once (independent of each other as the command line arguments
 * may overwrite the configuration settings)
 */
class OptionsIO {
 public:
    /** loads the configuration and parses the command line arguments */
    static bool getOptions(OptionsCont *oc, int argv, char **argc);
 private:
    /** tries to find the configuration */
    static std::string getConfigurationPath(OptionsCont *oc, bool &ok);
    /** loads and parses the configuration */
    static bool loadConfiguration(OptionsCont *oc);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "OptionsIO.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


