
#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_


#include "fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdlib.h"
#include "ColorSelector.h"

#include <iostream>
#include <sstream>
#include <string>
using namespace std;


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
  FXSlider*          dilatationSlider;
  FXSlider*          erosionSlider;
  FXSlider*          eraseStainsSlider;
  FXSlider*          nodeDistanceSlider;
  FXSlider*          epsilonSlider;
  FXSlider*			 mergeSlider;
  
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
  long onCmdMergeSlider(FXObject*,FXSelector sel,void*);
  long onCmdEpsiSlider(FXObject*,FXSelector sel,void*);
  
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

  
  //void create();
  };


#endif
//_CONFIGDIALOG_H_

