//---------------------------------------------------------------------------//
//                        ConfigDialog.cpp -
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
// Revision 1.11  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/09/09 12:51:48  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.5  2005/05/30 08:18:26  dksumo
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include "ConfigDialog.h"
#include <cstdlib>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// Map
FXDEFMAP(ConfigDialog) ConfigDialogMap[]={
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_PANEL,                         ConfigDialog::onCmdPanel),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_ACCEPT_COLOR,                  ConfigDialog::onCmdAcceptColor),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_DELETE_COLOR,                  ConfigDialog::onCmdDeleteColor),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_DEL_ALL_COL,                   ConfigDialog::onCmdDeleteAllColors),
  FXMAPFUNC(SEL_CHANGED,  ConfigDialog::ID_DIL_SLIDER,                     ConfigDialog::onCmdDilSlider),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_DIL_SLIDER,                     ConfigDialog::onCmdDilSlider),
  FXMAPFUNC(SEL_CHANGED,  ConfigDialog::ID_ERO_SLIDER,                     ConfigDialog::onCmdEroSlider),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_ERO_SLIDER,                     ConfigDialog::onCmdEroSlider),
  FXMAPFUNC(SEL_CHANGED,  ConfigDialog::ID_ERA_SLIDER,                     ConfigDialog::onCmdEraSlider),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_ERA_SLIDER,                     ConfigDialog::onCmdEraSlider),
  FXMAPFUNC(SEL_CHANGED,  ConfigDialog::ID_NODE_SLIDER,                    ConfigDialog::onCmdNodeSlider),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_NODE_SLIDER,                    ConfigDialog::onCmdNodeSlider),
  FXMAPFUNC(SEL_CHANGED,  ConfigDialog::ID_EPSI_SLIDER,                    ConfigDialog::onCmdEpsiSlider),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_EPSI_SLIDER,                    ConfigDialog::onCmdEpsiSlider),
  ///Miguel!!!
  FXMAPFUNC(SEL_CHANGED,  ConfigDialog::ID_MAPSCALE_SLIDER,                    ConfigDialog::onCmdMapscaleSlider),
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_MAPSCALE_SLIDER,                    ConfigDialog::onCmdMapscaleSlider),
  ///Miguel!!!
  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_OK,                             ConfigDialog::onCmdOK),

  FXMAPFUNC(SEL_COMMAND,  ConfigDialog::ID_CANCEL,                         ConfigDialog::onCmdCancel),
  };

// ConfigDialog implementation
FXIMPLEMENT(ConfigDialog,FXDialogBox,ConfigDialogMap,ARRAYNUMBER(ConfigDialogMap))


// Construct a dialog box
ConfigDialog::ConfigDialog(FXWindow* owner)
    :FXDialogBox(owner,"Image/Graph Configuration",DECOR_TITLE|DECOR_BORDER),
    pane(0)
{

  // Contents
  contents=new FXVerticalFrame(this,LAYOUT_SIDE_TOP|FRAME_NONE|LAYOUT_FILL_X|LAYOUT_FILL_Y|PACK_UNIFORM_WIDTH,0,0,500,500, 0,0,0,0);

  // Switcher
  tabbook=new FXTabBook(contents,NULL,ID_PANEL,PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT);

  //First Tab
    tab1=new FXTabItem(tabbook,"&Street Extraction",NULL);
    tab_frame1=new FXHorizontalFrame(tabbook,FRAME_THICK|FRAME_RAISED);
        boxframe1=new FXVerticalFrame(tab_frame1,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT, 0,0,300,300, 0,0,0,0);

            new FXLabel(boxframe1,"Choose the colors belonging to the street areas.",NULL,JUSTIFY_CENTER_X|LAYOUT_FILL_X);
            new FXHorizontalSeparator(boxframe1,SEPARATOR_GROOVE|LAYOUT_FILL_X);

            boxframe2=new FXHorizontalFrame(boxframe1,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT, 0,0,300,225, 0,0,0,0);
                colorlist=new FXList(boxframe2,NULL,0,LIST_EXTENDEDSELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,0,0,150,220);
                boxframe3=new FXVerticalFrame(boxframe2,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
                    colorbox=new ColorSelector(boxframe3,this,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0);

                    new FXButton(boxframe3,"&Delete List \tDelete list.",
                        NULL,this,ID_DEL_ALL_COL,
                        BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
                    new FXButton(boxframe3,"&Delete Color \tDelete the marked color from the list.",
                        NULL,this,ID_DELETE_COLOR,
                          BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
                    new FXButton(boxframe3,"&Add Color \tAccept the chosen color.",
                        NULL,this,ID_ACCEPT_COLOR,
                        BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_BOTTOM,0,0,0,0,0,0,0,0);
            new FXHorizontalSeparator(boxframe1,SEPARATOR_GROOVE|LAYOUT_FILL_X);
            new FXLabel(boxframe1,"Note:Streetgraph extraction works best with\n unnoisy and sharply defined pictures.",NULL,JUSTIFY_CENTER_X|LAYOUT_FILL_X);

    //Second tab
    tab2=new FXTabItem(tabbook,"&Filter",NULL);
    tab_frame2=new FXHorizontalFrame(tabbook,FRAME_THICK|FRAME_RAISED);
        boxframe4=new FXVerticalFrame(tab_frame2,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT , 0,0,300,300, 0,0,0,0);

            new FXLabel(boxframe4,"Set Filter Parameter.",NULL,JUSTIFY_CENTER_X|LAYOUT_FILL_X);
            new FXHorizontalSeparator(boxframe4,SEPARATOR_GROOVE|LAYOUT_FILL_X);
            FXMatrix *myMatrix=new FXMatrix(boxframe4,3,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);

                new FXLabel(myMatrix,"&Dilatation:",NULL,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
                dilatationTextField =new FXTextField(myMatrix,4,this,ID_DIL_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
                dilatationSlider    =new FXSlider(myMatrix,this,ID_DIL_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15,0,0,0,0);
                dilatationSlider->setRange(1,10);
                dilatationSlider->setTickDelta(1);
                dilatationSlider->setValue(1);
                dilatationTextField->setEditable(false);
                dilatationTextField->setText(FXStringVal(dilatationSlider->getValue()));

                new FXLabel(myMatrix,"&Erosion:",NULL,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
                erosionTextField =new FXTextField(myMatrix,4,this,ID_ERO_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
                erosionSlider    =new FXSlider(myMatrix,this,ID_ERO_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15,0,0,0,0);
                erosionSlider->setRange(1,10);
                erosionSlider->setTickDelta(1);
                erosionSlider->setValue(1);
                erosionTextField->setEditable(false);
                erosionTextField->setText(FXStringVal(erosionSlider->getValue()));

                new FXLabel(myMatrix,"&Erase Stains:",NULL,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
                eraseStainsTextField =new FXTextField(myMatrix,4,this,ID_ERA_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
                eraseStainsSlider    =new FXSlider(myMatrix,this,ID_ERA_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15,0,0,0,0);
                eraseStainsSlider->setRange(1,15);
                eraseStainsSlider->setTickDelta(1);
                eraseStainsSlider->setValue(5);
                eraseStainsTextField->setEditable(false);
                eraseStainsTextField->setText(FXStringVal(eraseStainsSlider->getValue()));

                new FXLabel(myMatrix,"&Node Distance:",NULL,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
                nodeDistanceTextField =new FXTextField(myMatrix,4,this,ID_NODE_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
                nodeDistanceSlider    =new FXSlider(myMatrix,this,ID_NODE_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15,0,0,0,0);
                nodeDistanceSlider->setRange(5,50);
                nodeDistanceSlider->setTickDelta(1);
                nodeDistanceSlider->setValue(20);
                nodeDistanceTextField->setEditable(false);
                nodeDistanceTextField->setText(FXStringVal(nodeDistanceSlider->getValue()));

                new FXLabel(myMatrix,"&Epsilon:",NULL,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
                epsilonTextField =new FXTextField(myMatrix,4,this,ID_EPSI_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
                epsilonSlider    =new FXSlider(myMatrix,this,ID_EPSI_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15,0,0,0,0);
                epsilonSlider->setRange(1,20);
                epsilonSlider->setTickDelta(1);
                epsilonSlider->setValue(7);
                epsilonTextField->setEditable(false);
                epsilonTextField->setText(FXStringVal(epsilonSlider->getValue()));


  //Third Tab

  tab3=new FXTabItem(tabbook,"&Bitmap Properties",NULL);
  ///Miguel!!!

  tab_frame3=new FXHorizontalFrame(tabbook,FRAME_THICK|FRAME_RAISED);
        boxframe1=new FXVerticalFrame(tab_frame3,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT , 0,0,300,300, 0,0,0,0);

            new FXLabel(boxframe1,"Set the scale and the gauss-krueger koordinates.",NULL,JUSTIFY_CENTER_X|LAYOUT_FILL_X);
            new FXHorizontalSeparator(boxframe1,SEPARATOR_GROOVE|LAYOUT_FILL_X);
            FXMatrix *myMatrix2=new FXMatrix(boxframe1,3,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);
			FXMatrix *myMatrix3=new FXMatrix(boxframe1,3,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);
			FXMatrix *myMatrix4=new FXMatrix(boxframe1,3,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,0,0,0,0,10,10,10,10, 5,8);

			///Miguel!!!
  new FXLabel(myMatrix2,"&Map Scale(gk/100/pixel):",NULL,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_LEFT);
  mapscaleTextField =new FXTextField(myMatrix2,4,this,ID_MAPSCALE_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
  mapscaleSlider    =new FXSlider(myMatrix2,this,ID_MAPSCALE_SLIDER,LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FIX_HEIGHT|SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,0,0,0,15,15,0,0,0);
  mapscaleSlider->setRange(50,500);
  mapscaleSlider->setTickDelta(25);
  mapscaleSlider->setValue(200);
  mapscaleTextField->setEditable(false);
  mapscaleTextField->setText(FXStringVal(mapscaleSlider->getValue()));

  new FXLabel(myMatrix3,"&LL Corner GKR-Value:",NULL,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_LEFT);
  gkrTextField =new FXTextField(myMatrix3,10,this,ID_GKR_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,10,10,10,10, DEFAULT_PAD,DEFAULT_PAD,0,0);
  gkrTextField->setEditable(true);

  new FXLabel(myMatrix4,"&LL Corner GKH-Value:",NULL,LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
  gkhTextField =new FXTextField(myMatrix4,10,this,ID_GKH_TEXT,JUSTIFY_RIGHT|LAYOUT_FILL_ROW|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK,0,0,0,0, DEFAULT_PAD,DEFAULT_PAD,0,0);
  gkhTextField->setEditable(true);


  ///Miguel!!!


  buttonframe=new FXHorizontalFrame(contents,LAYOUT_FILL_X|LAYOUT_FILL_Y);
    new FXButton(buttonframe,"&OK \tOK.",
        NULL,this,ID_OK,
        BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_RIGHT,0,0,70,30,0,0,0,0);
    new FXButton(buttonframe,"&Cancel \tCancel.",
        NULL,this,ID_CANCEL,
        BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_RIGHT,0,0,70,30,0,0,0,0);


}

// Must delete the menus
ConfigDialog::~ConfigDialog(){
  delete pane;
  }



long ConfigDialog::onCmdDilSlider(FXObject*,FXSelector sel,void*){
  dilatationTextField->setText(FXStringVal(dilatationSlider->getValue()));
  dilatationSlider->setValue(dilatationSlider->getValue());
  return 1;
  }

long ConfigDialog::onCmdEroSlider(FXObject*,FXSelector sel,void*){
  erosionTextField->setText(FXStringVal(erosionSlider->getValue()));
  erosionSlider->setValue(erosionSlider->getValue());
  return 1;
  }

long ConfigDialog::onCmdEraSlider(FXObject*,FXSelector sel,void*){
  eraseStainsTextField->setText(FXStringVal(eraseStainsSlider->getValue()));
  eraseStainsSlider->setValue(eraseStainsSlider->getValue());
  return 1;
  }

long ConfigDialog::onCmdNodeSlider(FXObject*,FXSelector sel,void*){
  nodeDistanceTextField->setText(FXStringVal(nodeDistanceSlider->getValue()));
  nodeDistanceSlider->setValue(nodeDistanceSlider->getValue());
  return 1;
  }

long ConfigDialog::onCmdEpsiSlider(FXObject*,FXSelector sel,void*){
  epsilonTextField->setText(FXStringVal(epsilonSlider->getValue()));
  epsilonSlider->setValue(epsilonSlider->getValue());
  return 1;
  }
///Miguel!!!
long ConfigDialog::onCmdMapscaleSlider(FXObject*,FXSelector sel,void*){
  mapscaleTextField->setText(FXStringVal(mapscaleSlider->getValue()));
  mapscaleSlider->setValue(mapscaleSlider->getValue());
  return 1;
  }
///Miguel!!!

// Active panel switched
long ConfigDialog::onCmdPanel(FXObject*,FXSelector,void* ptr)
{
  FXTRACE((1,"Panel = %d\n",(FXint)(long)ptr));
  return 1;
}

long ConfigDialog::onCmdAcceptColor(FXObject*,FXSelector,void* ptr)
{
  calculateRGBValues(colorbox->getRGBA());
  FXString colorText;
  colorText="R: "+(FXStringVal(255.0f*rgba[0],0,FALSE))+
            "     G: "+(FXStringVal(255.0f*rgba[1],0,FALSE))+
            "     B: "+(FXStringVal(255.0f*rgba[2],0,FALSE));
  if((colorlist->findItem(colorText,-1,SEARCH_FORWARD)==-1)||(colorlist->getNumItems()==0))
    colorlist->appendItem(colorText);
  return 1;
}

long ConfigDialog::onCmdDeleteAllColors(FXObject*,FXSelector,void* ptr)
{
  if(colorlist->getNumItems()>0)
    colorlist->clearItems();
  return 1;
}

long ConfigDialog::onCmdCancel(FXObject*,FXSelector,void* ptr)
{
  hide();
  return 1;
}

long ConfigDialog::onCmdOK(FXObject*,FXSelector,void* ptr)
{
  hide();
  return 1;
}



long ConfigDialog::onCmdDeleteColor(FXObject*,FXSelector,void* ptr)
{
  if((colorlist->getNumItems()>0)&&(colorlist->isItemSelected(colorlist->getCurrentItem())))
      colorlist->removeItem(colorlist->getCurrentItem(),FALSE);
  return 1;
}


long ConfigDialog::calculateRGBValues(FXColor color){

  rgba[0]=0.003921568627f*FXREDVAL(color);
  rgba[1]=0.003921568627f*FXGREENVAL(color);
  rgba[2]=0.003921568627f*FXBLUEVAL(color);
  return 1;
  int i =10;
}


int ConfigDialog::getMergeTolerance(){
    return mergeSlider->getValue();
}


FXSlider* ConfigDialog::getDilSlider(){
    return dilatationSlider;
}


FXSlider* ConfigDialog::getEpsiSlider(){
    return epsilonSlider;
}

FXSlider* ConfigDialog::getEraSlider(){
    return eraseStainsSlider;
}


FXSlider* ConfigDialog::getEroSlider(){
    return erosionSlider;
}

FXSlider* ConfigDialog::getMergeSlider(){
    return mergeSlider;
}

FXSlider* ConfigDialog::getNodeSlider(){
    return nodeDistanceSlider;
}

///Miguel!!!
FXSlider* ConfigDialog::getMapscaleSlider(){
    return mapscaleSlider;
}

FXTextField* ConfigDialog::getGKRTextField(){
    return gkrTextField;
}

FXTextField* ConfigDialog::getGKHTextField(){
    return gkhTextField;
}
///Miguel!!!
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
