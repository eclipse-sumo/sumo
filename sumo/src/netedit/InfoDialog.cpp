#include "InfoDialog.h"

// Map
FXDEFMAP(InfoDialog) InfoDialogMap[]={
  FXMAPFUNC(SEL_COMMAND,  InfoDialog::ID_PANEL,                         InfoDialog::onCmdPanel),
  FXMAPFUNC(SEL_COMMAND,  InfoDialog::ID_OK,                             InfoDialog::onCmdOK),

  }; 



// ConfigDialog implementation
FXIMPLEMENT(InfoDialog,FXDialogBox,InfoDialogMap,ARRAYNUMBER(InfoDialogMap))


// Construct a dialog box
InfoDialog::InfoDialog(FXWindow* owner):FXDialogBox(owner,"Info Window",DECOR_TITLE|DECOR_BORDER)
{  

 // Contents
  contents=new FXVerticalFrame(this,LAYOUT_SIDE_TOP|FRAME_NONE|LAYOUT_FILL_X|LAYOUT_FILL_Y|PACK_UNIFORM_WIDTH,0,0,150,50, 0,0,0,0);

  // Switcher
  

	
 boxframe1=new FXVerticalFrame(contents,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT, 0,0,275,200, 0,0,0,0);
 new FXLabel(boxframe1,"Note : \n The chosen algorism cannot be used \n on the actuall bitmap\n ",NULL,JUSTIFY_CENTER_X|LAYOUT_FILL_X);
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






