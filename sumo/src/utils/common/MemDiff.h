#ifndef MemDiff_H
#define MemDiff_H
/***************************************************************************
                          MemDiff.h
			  A class that wraps the MS - memory counting methods
                             -------------------
    project              : SUMO
    begin                : Thu, 06 Jul 2002
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : ? + Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.1  2002/10/16 15:09:09  dkrajzew
// initial commit for some utility classes common to most propgrams of the sumo-package
//
// Revision 1.4  2002/06/11 15:58:24  dkrajzew
// windows eol removed
//
// Revision 1.3  2002/06/07 14:58:46  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
//
#ifdef _DEBUG
#ifdef WIN32

/*#if _MSC_VER > 1000
   #pragma once
#endif*/ // _MSC_VER > 1000

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <afx.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
class CMemDiff : CMemoryState
{
// Construction/Destruction
public:
	CMemDiff();
	~CMemDiff();
};

#endif
#endif

#endif
