
#ifndef _INFODIALOG_H_
#define _INFODIALOG_H_


#include "fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdlib.h"


#include <iostream>
#include <sstream>
#include <string>
using namespace std;


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


#endif
//_INFODIALOG_H_