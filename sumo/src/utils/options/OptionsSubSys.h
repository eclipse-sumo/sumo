#ifndef OptionsSubSys_h
#define OptionsSubSys_h
//---------------------------------------------------------------------------//
//                        SystemFrame.h -
//  A set of actions common to all applications
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 23.06.2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.4  2005/05/04 09:28:01  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/11/23 10:36:02  dkrajzew
// debugging
//
// Revision 1.2  2003/10/27 10:55:10  dkrajzew
// problems on setting gui options patched - the configuration is not loaded directly any more
//
// Revision 1.1  2003/06/24 08:12:59  dkrajzew
// a subsystem for option handling common to most of the applications added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * abstract method definitions
 * ======================================================================= */
typedef void (fill_options)(OptionsCont &);
typedef bool (check_options)(OptionsCont &);


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OptionsSubSys
 * A class which holds the options for an application in a static container.
 * This shall not only allow an easier access to the command line options,
 * but also generalise the performing of option-related functions common
 * to all applications such as checking whether to print the help-screen,
 * validating the input etc.
 */
class OptionsSubSys {
public:
    /** @brief Initialises the options container
        fill_f is used to fill the given OptionsCont with values, check_f
        to validate them. This method performs the parsing of options and
        also other options-related function (printing the help screen etc.) */
    static bool init(bool loadConfig, int argc, char **argv,
        fill_options *fill_f, check_options *check_f,
        char *help[]);

    /** @brief Initialisation method for gui-based applications
        We may be sure that we set our options when performing gui-based
        options processes right (not via the command line), so checking
        is left out here; we also do not have to parse command line options
        or to print anything.
        The name will be inserted as the name of the configuration to load. */
    static bool guiInit(fill_options *fill_f,
        const std::string &conf);

    /** @brief Retrives the options
        They should be initialised before. */
    static OptionsCont &getOptions();

    /// Clears all previously set options
    static void close();

protected:
    /// The static options container used
    static OptionsCont myOptions;

protected:
    /// (No) constructor
    OptionsSubSys();

    /// (No) destructor
    ~OptionsSubSys();

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
