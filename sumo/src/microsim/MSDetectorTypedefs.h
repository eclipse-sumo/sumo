#ifndef MSDETECTORTYPEDEFS_H
#define MSDETECTORTYPEDEFS_H

#include "MSMeanDetector.h"
#include "MSSumDetector.h"
#include "MSE2Detector.h"

// conrete detectors
#include "MSDensity.h"


namespace Detector 
{
    typedef MSMeanDetector< MSE2Detector< MSDensity > > E2Density;
}

#endif // MSE2DETECTORTYPEDEFS_H

// Local Variables:
// mode:C++
// End:
