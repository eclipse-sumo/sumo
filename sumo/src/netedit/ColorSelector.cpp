#include <fx.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXRegistry.h"
#include "FXHash.h"
#include "FXApp.h"
#include "ColorSelector.h"


using namespace FX;

namespace FX {

// Map
FXDEFMAP(ColorSelector) ColorSelectorMap[]={
  FXMAPFUNC(SEL_CHANGED,ColorSelector::ID_COLOR,            ColorSelector::onChgColor),
  FXMAPFUNC(SEL_COMMAND,ColorSelector::ID_COLOR,            ColorSelector::onCmdColor),
  FXMAPFUNC(SEL_COMMAND,ColorSelector::ID_COLORPICK,        ColorSelector::onCmdColorPick),
  FXMAPFUNC(SEL_COMMAND,ColorSelector::ID_WELL_CHANGED,     ColorSelector::onCmdWell),
  FXMAPFUNC(SEL_CHANGED,ColorSelector::ID_WELL_CHANGED,     ColorSelector::onChgWell),
  };


// Object implementation
FXIMPLEMENT(ColorSelector,FXPacker,ColorSelectorMap, ARRAYNUMBER(ColorSelectorMap))

// Separator item
ColorSelector::ColorSelector(FXComposite *p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):
  FXPacker(p,opts,x,y,w,h){
//  FXLabel *label;
  target=tgt;
  message=sel;

  //Icon
 // eyedropicon=new FXXPMIcon(getApp(),eyedrop_xpm);

  FXVerticalFrame *wellframe=new FXVerticalFrame(this,LAYOUT_NORMAL,0,0,0,0, 0,0,10,0,0,2);

/*
new FXButton(myIMGToolBar,"\t\tOpen Bitmap Configuration Dialog.",
        GUIIconSubSys::getIcon(ICON_OPEN_BMP_DIALOG), this, MID_OPEN_BMP_DIALOG,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
*/


  // Color sucker
  new FXPicker(wellframe,"\tPick color",NULL,this,ID_COLORPICK,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|LAYOUT_FILL_X|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_TOP,0,0,120,35,0,0,0,0);
  //new FXPicker(wellframe,"\tPick color",GUIIconSubSys::getIcon(ICON_EYEDROP),this,ID_COLORPICK,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_CENTER_X|LAYOUT_FILL_X|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_TOP,0,0,120,35,0,0,0,0);

  // Main color well
  well=new FXColorWell(wellframe,FXRGBA(255,255,255,255),this,ID_WELL_CHANGED,COLORWELL_SOURCEONLY|LAYOUT_FILL_X|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT, 0,0,130,90,0,0,0,0);
  }

// Init the panel
void ColorSelector::create(){
  FXPacker::create();
  }

// Color picker
long ColorSelector::onCmdColorPick(FXObject*,FXSelector,void* ptr){
  FXPoint *point=(FXPoint*)ptr;
  FXDCWindow dc(getRoot());
  setRGBA(dc.readPixel(point->x,point->y));
  handle(this,FXSEL(SEL_COMMAND,ID_COLOR),(void*)(FXuval)well->getRGBA());
  return 1;
  }

// Forward to target
long ColorSelector::onCmdColor(FXObject*,FXSelector,void*){
  if(target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)well->getRGBA());
  return 1;
  }

// Forward to target
long ColorSelector::onChgColor(FXObject*,FXSelector,void*){
  if(target) target->handle(this,FXSEL(SEL_CHANGED,message),(void*)(FXuval)well->getRGBA());
  return 1;
  }

// Set color
long ColorSelector::onCmdSetValue(FXObject*,FXSelector,void* ptr){
  setRGBA((FXColor)(FXuval)ptr);
  return 1;
  }

long ColorSelector::onCmdWell(FXObject*,FXSelector,void*){
  handle(this,FXSEL(SEL_COMMAND,ID_COLOR),(void*)(FXuval)well->getRGBA());
  return 1;
  }

long ColorSelector::onChgWell(FXObject*,FXSelector,void* ptr){
  FXColor color=(FXColor)(FXuval)ptr;
  rgba[0]=0.003921568627f*FXREDVAL(color);
  rgba[1]=0.003921568627f*FXGREENVAL(color);
  rgba[2]=0.003921568627f*FXBLUEVAL(color);
  rgba[3]=0.003921568627f*FXALPHAVAL(color);
  return 1;
  }

// Change RGBA color
void ColorSelector::setRGBA(FXColor clr){
  if(clr!=well->getRGBA()){
    rgba[0]=0.003921568627f*FXREDVAL(clr);
    rgba[1]=0.003921568627f*FXGREENVAL(clr);
    rgba[2]=0.003921568627f*FXBLUEVAL(clr);
    rgba[3]=0.003921568627f*FXALPHAVAL(clr);
    well->setRGBA(clr);
    }
  }

// Retrieve RGBA color
FXColor ColorSelector::getRGBA() const {
  return well->getRGBA();
  }
/*
// Save data
void ColorSelector::save(FXStream& store) const {

  FXPacker::save(store);
  store << well;
  store << accept;
  store << cancel;

  store.save(rgba,4);
  store.save(hsva,4);

  }

// Load data
void ColorSelector::load(FXStream& store){
/*  FXPacker::load(store);
  store >> well;
  store >> accept;
  store >> cancel;

  store.load(rgba,4);
  store.load(hsva,4);

  }
*/

// Cleanup; icons must be explicitly deleted
ColorSelector::~ColorSelector(){
//  delete eyedropicon;
  well=(FXColorWell*)-1L;
  }

}
