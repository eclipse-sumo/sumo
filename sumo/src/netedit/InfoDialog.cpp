//---------------------------------------------------------------------------//
//                        InfoDialog.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2005/10/07 11:38:33  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/09/09 12:51:48  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.2  2005/05/30 08:18:26  dksumo
// comments added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "InfoDialog.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

// Map
FXDEFMAP(InfoDialog) InfoDialogMap[]={
  FXMAPFUNC(SEL_COMMAND,  InfoDialog::ID_PANEL,                         InfoDialog::onCmdPanel),
  FXMAPFUNC(SEL_COMMAND,  InfoDialog::ID_OK,                             InfoDialog::onCmdOK),

  };



// ConfigDialog implementation
FXIMPLEMENT(InfoDialog,FXDialogBox,InfoDialogMap,ARRAYNUMBER(InfoDialogMap))


// Construct a dialog box
InfoDialog::InfoDialog(FXWindow* owner)
:FXDialogBox(owner,"Info Window",DECOR_TITLE|DECOR_BORDER), pane(0)
{

 // Contents
  contents=new FXVerticalFrame(this,LAYOUT_SIDE_TOP|FRAME_NONE|LAYOUT_FILL_X|LAYOUT_FILL_Y|PACK_UNIFORM_WIDTH,0,0,150,50, 0,0,0,0);

  // Switcher



 boxframe1=new FXVerticalFrame(contents,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT, 0,0,275,200, 0,0,0,0);
 new FXLabel(boxframe1,"Note : \n The chosen algorithm cannot be used \n on the actuall bitmap\n ",NULL,JUSTIFY_CENTER_X|LAYOUT_FILL_X);
 new FXHorizontalSeparator(boxframe1,SEPARATOR_GROOVE|LAYOUT_FILL_X);
 new FXLabel(boxframe1,"Info:\n You cannot use the extractStreets-algorithm \n on a scelettonized bitmap",NULL,JUSTIFY_CENTER_X|LAYOUT_FILL_X);




  buttonframe=new FXHorizontalFrame(contents,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  	new FXButton(buttonframe,"&OK \tOK.",
		NULL,this,ID_OK,
		BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_RIGHT,0,0,70,30,0,0,0,0);

}

// Must delete the menus
InfoDialog::~InfoDialog(){
  delete pane;
  }





// Active panel switched
long InfoDialog::onCmdPanel(FXObject*,FXSelector,void* ptr)
{
  FXTRACE((1,"Panel = %d\n",(FXint)(long)ptr));
  return 1;
}





long InfoDialog::onCmdOK(FXObject*,FXSelector,void* ptr)
{
  hide();
  return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



