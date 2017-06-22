/****************************************************************************/
/// @file    NBPTLineCont.cpp
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id: NBPTLineCont.cpp 24570 2017-06-07 06:54:55Z namdre $
///
// Container for NBPTLine during netbuild
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include <iostream>
#include "NBPTLineCont.h"



NBPTLineCont::NBPTLineCont() :
    myIdCnt(0) {}


NBPTLineCont::~NBPTLineCont() {
    for (std::vector<NBPTLine*>::iterator it = myPTLines.begin(); it != myPTLines.end(); it++){
        delete *it;
    }
    myPTLines.clear();
}

void 
NBPTLineCont::insert(NBPTLine* pLine) {
//    std::cout << "new line: " << pLine->getName() << " stops: " << pLine->getStops().size() << std::endl;
    pLine->setId(myIdCnt++);
    myPTLines.push_back(pLine);
}

