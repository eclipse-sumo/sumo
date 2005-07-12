//---------------------------------------------------------------------------//
//                        GUIFrame.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2005/07/12 12:49:08  dkrajzew
// code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsParser.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/DevHelper.h>
#include "GUIFrame.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */

void
GUIFrame::fillInitOptions(OptionsCont &oc)
{
    oc.doRegister("max-gl-width", 'w', new Option_Integer(1280));
    oc.doRegister("max-gl-height", 'h', new Option_Integer(1024));
    oc.doRegister("quit-on-end", 'Q', new Option_Bool(false));
    oc.doRegister("surpress-end-info", 'S', new Option_Bool(false));
    oc.doRegister("help", '?', new Option_Bool(false));
    oc.doRegister("configuration", 'c', new Option_FileName());
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("allow-floating-aggregated-views", 'F', new Option_Bool(false));
    oc.doRegister("disable-aggregated-views", 'A', new Option_Bool(false));
    oc.doRegister("disable-textures", 'T', new Option_Bool(false));
    oc.doRegister("verbose", 'v', new Option_Bool(false)); // !!!
    DevHelper::insertDevOptions(oc);
}


bool
GUIFrame::checkInitOptions(OptionsCont &oc)
{
    // check whether the parameter are ok
    if(oc.getInt("w")<0||oc.getInt("h")<0) {
        MsgHandler::getErrorInstance()->inform(
            "Both the screen's width and the screen's height must be larger than zero.");
        return false;
    }
    return true;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

