#include "MSDetector.h"
#include <utils/logging/LoggedValue.h>
#include <utils/logging/LoggedValue_Single.h>
#include <utils/logging/LoggedValue_TimeFixed.h>
#include <utils/logging/LoggedValue_TimeFloating.h>


MSDetector::MSDetector( std::string id, OutputStyle style, 
                       std::ofstream* file, MSNet::Time sampleInterval,
                       bool floating) 
    : myID( id ), myStyle( style ), myFile( file ),
    mySampleInterval(sampleInterval), myAmTimeFloating(floating)
{
}


MSDetector::~MSDetector() 
{
}

