#ifndef GUIGradients_h
#define GUIGradients_h

#include <vector>
#include <utils/gfx/RGBColor.h>
#include <utils/gui/drawer/GUIGradientStorage.h>

/// The gradient used
extern std::vector<RGBColor> myDensityGradient;

/// A storage for available gradients
extern GUIGradientStorage *gGradients;


#endif
