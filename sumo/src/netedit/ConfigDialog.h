#ifndef _FXTESTDIALOG_H_
#define _FXTESTDIALOG_H_


#include "fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class FXTestDialog : public FXDialogBox {
  FXDECLARE(FXTestDialog)
protected:
  FXHorizontalFrame* contents;
  FXHorizontalFrame* buttons;
  FXMenuPane*        menu;
  FXMenuPane*        submenu;
  FXPopup*           pane;



//  FXMenuBar*         menubar;
//  FXMenuPane*        filemenu;
//  FXMenuPane*        tabmenu;
//  FXHorizontalFrame* contents;
  FXTabBook*         tabbook;
  FXTabItem*         tab1;
  FXTabItem*         tab2;
  FXTabItem*         tab3;
  FXHorizontalFrame* tab_frame1;
  FXHorizontalFrame* tab_frame2;
  FXVerticalFrame*   boxframe1;
  FXHorizontalFrame* boxframe2;
  FXHorizontalFrame* tab_frame3;
  FXList*            simplelist;
//  FXFileList*        filelist;
//  FXDirList*         dirlist;

public:

// Messages
  enum{
    ID_PANEL
    };

private:
  FXTestDialog(){}
public:
  FXTestDialog(FXWindow* owner);
  virtual ~FXTestDialog();
  long onCmdPanel(FXObject*,FXSelector,void* ptr);
  //void create();
  };


#endif
//_FXTESTDIALOG_H_