#ifndef SLogging_h
#define SLogging_h
/***************************************************************************
                          SLogging.h  -  container for strings that shall be
			  presented to the user
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
// Revision 1.3  2002/06/11 13:44:35  dkrajzew
// Windows eol removed
//
// Revision 1.2  2002/04/24 10:38:45  dkrajzew
// Strings are now passed as references
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:26  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/14 14:37:22  croessel
// GPL-notice added.
//
// Revision 1.4  2002/02/13 15:40:47  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:13  traffic
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

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * SLogging
 * SLogging receives all messages that shall be passed to the user.
 * The method that receives such messages from other program-components is
 * static to avoid complicated passing of pointers to all message-delivarating
 * methods.
 */
class SLogging {
private:
    /// definition of the container for saved messages
    typedef std::vector<std::string> Messages;
    /// the list of messages got
    static Messages    m_Messages;
public:
    /// standard constructor
    SLogging();
    /// destructor
    ~SLogging();
    /// adds a message to the list of messages to present to the user
    static void add(const std::string &msg, bool report=true, bool newLine=true);
    static void add(char *msg, bool report=true, bool newLine=true);
private:
    /** invalid copy constructor */
    SLogging(const SLogging &s);
    /** invalid assignment operator */
    SLogging &operator=(const SLogging &s);
};

#endif

// Local Variables:
// mode:C++
// End:
