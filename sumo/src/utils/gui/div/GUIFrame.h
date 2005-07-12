#ifndef GUIFrame_h
#define GUIFrame_h
//---------------------------------------------------------------------------//
//                         GUIFrame.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.04.2005
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
// Revision 1.2  2005/07/12 12:49:08  dkrajzew
// code style adapted
//
// Revision 1.2  2005/05/30 08:21:04  dksumo
// comments added
//


class OptionsCont;

/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIFrame {
public:
	static void fillInitOptions(OptionsCont &oc);

	static bool checkInitOptions(OptionsCont &oc);

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
