/***************************************************************************
                          OptionsParser.cpp
			  Parses the command line arguments
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2002/04/08 07:21:25  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:28  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:20  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "Option.h"
#include "OptionsCont.h"
#include "OptionsParser.h"
#include "UtilExceptions.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
bool OptionsParser::parse(OptionsCont *oc, int argc, char **argv) {
    bool ok = true;
    for(int i=1; i<argc; ) {
      int add;
      if(i<argc-1)
        add = check(oc, argv[i], argv[i+1]);
      else
        add = check(oc, argv[i]);
      if(add>0)
        i += add;
      else {
        i += 1;
        ok = false;
      }
    }
    return ok;
}

int OptionsParser::check(OptionsCont *oc, char *arg1) {
    // the last stand-alone argument should be a switch
    if(!checkParameter(arg1)) return -1;
    // check switch
    bool error = false;
    if(isAbbreviation(arg1)) {
      // set all switches when abbreviated
      for(int i=1; i<arg1[i]!=0; i++) {
        try {
          if(!oc->set(convert(arg1[i]), true)) {
            error = true;
          }
        } catch (InvalidArgument &e) {
          return -1;
        }
      }
    } else {
	  // set single switch
      try {
        if(!oc->set(convert(arg1+2), true)) {
          error = true;
        } 
      } catch(InvalidArgument &e) {
        return -1;
      }
    }
    if(error) return -1;
    return 1;
}

int OptionsParser::check(OptionsCont *oc, char *arg1, char *arg2) {
    // the first argument should be an option 
    // (only the second may be a free string)
    if(!checkParameter(arg1)) return -1;
    // process not abbreviated parameters
    if(!isAbbreviation(arg1)) {
      try {
        if(oc->isBool(convert(arg1+2))) {
          if(!oc->set(convert(arg1+2), true)) 
            return -1;
          return 1;
        }
        else {
          if(!oc->set(convert(arg1+2), convert(arg2))) 
            return -1;
          return 2;
        }
      } catch (InvalidArgument &e) {
        return -1;
      }
    } 
    // process abbreviated parameters
    else {
      bool error = false;
      char file = 0;
      try {
        for(int i=1; arg1[i]!=0; i++) {
          if(oc->isBool(convert(arg1[i]))) {
            if(!oc->set(convert(arg1[i]), true)) {
              error = true;
            }
          } else {
            if(file!=0) {
              cout << "Error: The current parameter '" << arg1[i] << "' and the parameter '" << file << "' do both need a value." << endl;
              error = true;
            }
            file = arg1[i];
          }
        }
        if(file!=0) {
          if(!oc->set(convert(file), convert(arg2))) {
            error = true;
          } else
            return 2;
        }
        if(error) return -1;
          return 1;
      } catch (InvalidArgument &e) {
        return -1;
      }
    }
}


bool OptionsParser::checkParameter(char *arg1) {
  if(arg1[0]!='-') {
      cerr << "The parameter " << arg1 << " is not allowed in this context" << endl;
      cerr << "Switch or parameter name expected." << endl;
      return false;
  } 
  return true;
}

bool OptionsParser::isAbbreviation(char *arg1) {
    return arg1[1]!='-';
}

string OptionsParser::convert(char *arg) {
    string s(arg);
    return s;
}

string OptionsParser::convert(char abbr) {
    char buf[2];
    buf[0] = abbr;
    buf[1] = 0;
    string s(buf);
    return buf;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "OptionsParser.icc"
//#endif

// Local Variables:
// mode:C++
// End:


