#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#ifndef FXPACKER_H
#include "FXPacker.h"
#endif

namespace FX {

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
#endif
// COLORSELECTOR_H
