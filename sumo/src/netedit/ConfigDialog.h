
#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_
//---------------------------------------------------------------------------//
//                        CONFIGDIALOG.h -
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
// Revision 1.8  2005/10/07 11:38:33  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/09/09 12:51:48  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.5  2005/06/14 11:22:27  dksumo
// documentation added
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

#include "fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdlib.h"
#include "ColorSelector.h"

#include <iostream>
#include <sstream>
#include <string>

/* =========================================================================
 * used namespace
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class ConfigDialog : public FXDialogBox {
  FXDECLARE(ConfigDialog)
protected:
  FXVerticalFrame* contents;
  FXHorizontalFrame* buttonframe;
  FXPopup*           pane;

  FXTabBook*         tabbook;
  FXTabItem*         tab1;
  FXTabItem*         tab2;
  FXTabItem*         tab3;
  FXHorizontalFrame* tab_frame1;
  FXHorizontalFrame* tab_frame2;
  FXVerticalFrame*   boxframe1;
  FXHorizontalFrame* boxframe2;
  FXVerticalFrame*   boxframe3;
  FXVerticalFrame*   boxframe4;
  FXHorizontalFrame* boxframe5;
  FXHorizontalFrame* boxframe6;
  FXHorizontalFrame* boxframe7;
  FXHorizontalFrame* boxframe8;
  FXHorizontalFrame* tab_frame3;
  FXList*            colorlist;
  ColorSelector*     colorbox;
  FXColor            currentcolor;
  FXfloat            rgba[3];
  FXTextField*       dilatationTextField;
  FXTextField*       erosionTextField;
  FXTextField*       eraseStainsTextField;
  FXTextField*       nodeDistanceTextField;
  FXTextField*       epsilonTextField;
  FXTextField*		 mergeTextField;
  ///Miguel!!!
  FXTextField*		 mapscaleTextField;
  FXTextField*		 gkrTextField;
  FXTextField*		 gkhTextField;
  ///Miguel!!!
  FXSlider*          dilatationSlider;
  FXSlider*          erosionSlider;
  FXSlider*          eraseStainsSlider;
  FXSlider*          nodeDistanceSlider;
  FXSlider*          epsilonSlider;
  FXSlider*			 mergeSlider;
  ///Miguel!!!
  FXSlider*			 mapscaleSlider;
  ///Miguel!!!
public:


// Messages
  enum{
	ID_PANEL=FXMainWindow::ID_LAST,
	ID_MERGE_SLIDER,
	ID_MERGE_TEXT,
	ID_DIL_SLIDER,
	ID_DIL_TEXT,
	ID_ERA_SLIDER,
	ID_ERA_TEXT,
	ID_ERO_SLIDER,
	ID_ERO_TEXT,
	ID_NODE_SLIDER,
	ID_NODE_TEXT,
	ID_EPSI_SLIDER,
	ID_EPSI_TEXT,
	///Miguel!!!
	ID_MAPSCALE_SLIDER,
	ID_MAPSCALE_TEXT,
	ID_GKR_TEXT,
	ID_GKH_TEXT,
	///Miguel!!!
	ID_DEL_ALL_COL,
	ID_ACCEPT_COLOR,
	ID_DELETE_COLOR,
	ID_CANCEL,
	ID_OK,
  };

private:
  ConfigDialog(){}

public:
  ConfigDialog(FXWindow* owner);
  virtual ~ConfigDialog();

  long onCmdPanel(FXObject*,FXSelector,void* ptr);
  long onCmdAcceptColor(FXObject*,FXSelector,void* ptr);
  long onCmdDeleteColor(FXObject*,FXSelector,void* ptr);
  long onCmdDeleteAllColors(FXObject*,FXSelector,void* ptr);
  long calculateRGBValues(FXColor color);

  long onCmdDilSlider(FXObject*,FXSelector sel,void*);
  long onCmdEroSlider(FXObject*,FXSelector sel,void*);
  long onCmdEraSlider(FXObject*,FXSelector sel,void*);
  long onCmdNodeSlider(FXObject*,FXSelector sel,void*);

  long onCmdEpsiSlider(FXObject*,FXSelector sel,void*);
  ///Miguel!!!
  long onCmdMapscaleSlider(FXObject*,FXSelector sel,void*);
  ///Miguel!!!
  long onCmdCancel(FXObject*,FXSelector,void* ptr);
  long onCmdOK(FXObject*,FXSelector,void* ptr);

  long onUpdDilSlider(FXObject*,FXSelector sel,void*);

  int getMergeTolerance();

  FXSlider* getEroSlider();
  FXSlider* getDilSlider();
  FXSlider* getNodeSlider();
  FXSlider* getEpsiSlider();
  FXSlider* getEraSlider();
  FXSlider* getMergeSlider();
  ///Miguel!!!
  FXSlider* getMapscaleSlider();
  FXTextField* getGKRTextField();
  FXTextField* getGKHTextField();
  ///Miguel!!!
  //void create();
  };
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
