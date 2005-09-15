
#ifndef _INFODIALOG_H_
#define _INFODIALOG_H_
//---------------------------------------------------------------------------//
//                        InfoDialog.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                :
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
// Revision 1.4  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/09/09 12:51:48  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.2  2005/05/30 08:18:26  dksumo
// comments added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdlib.h"


#include <iostream>
#include <sstream>
#include <string>
using namespace std;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class InfoDialog : public FXDialogBox {
  FXDECLARE(InfoDialog)
protected:
  FXVerticalFrame* contents;
  FXHorizontalFrame* buttonframe;
  FXPopup*           pane;
  FXVerticalFrame*   boxframe1;


public:


// Messages
  enum{
	ID_PANEL=FXMainWindow::ID_LAST,

	ID_OK,
  };

private:
  InfoDialog(){}

public:
  InfoDialog(FXWindow* owner);
  virtual ~InfoDialog();

  long onCmdPanel(FXObject*,FXSelector,void* ptr);
  long onCmdOK(FXObject*,FXSelector,void* ptr);






  //void create();
  };

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif
// Local Variables:
// mode:C++
// End:
