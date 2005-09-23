#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H
//---------------------------------------------------------------------------//
//                        COLORSELECTOR.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
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
// Revision 1.4  2005/09/23 06:01:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/09/09 12:51:48  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.2  2005/06/14 11:22:27  dksumo
// documentation added
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifndef FXPACKER_H
#include "FXPacker.h"
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H


/* =========================================================================
 * namespace definitions
 * ======================================================================= */
namespace FX {

/* =========================================================================
 * class declarations
 * ======================================================================= */
class FXColorWell;
//class FXButton;
class FXIcon;

/// Color selection widget
class  ColorSelector : public FXPacker {
  FXDECLARE(ColorSelector)
protected:
  FXColorWell  *well;                 // Main well
//  FXXPMIcon    *eyedropicon;          // Icon for eye dropper
  FXfloat       rgba[4];              // Accurate RGBA color

protected:
  ColorSelector(){}
  void updateWell();
private:
  ColorSelector(const ColorSelector&);
  ColorSelector &operator=(const ColorSelector&);
public:
  long onCmdColor(FXObject*,FXSelector,void*);
  long onChgColor(FXObject*,FXSelector,void*);
  long onCmdSetValue(FXObject*,FXSelector,void*);
  long onCmdColorPick(FXObject*,FXSelector,void*);
  long onCmdWell(FXObject*,FXSelector,void*);
  long onChgWell(FXObject*,FXSelector,void*);
public:
  enum {
    ID_COLOR,
	ID_COLORPICK,
	ID_WELL_CHANGED,
    ID_LAST
    };
public:

  /// Construct a new ColorSelector
  ColorSelector(FXComposite *p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  /// Create the ColorSelector
  virtual void create();

  /// Set the selected color
  void setRGBA(FXColor clr);

  /// Get the selected color
  FXColor getRGBA() const;

  /// Destructor
  virtual ~ColorSelector();
  };

}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
