#ifndef TOSTRING_H
#define TOSTRING_H

//---------------------------------------------------------------------------//
//                        ToString.h  -  Converter to std::string
//                           -------------------
//  begin                : Wed, 23 Sep 2002
//  copyright            : (C) 2002 by Christian Roessel
//  organisation         : DLR/IVF http://ivf.dlr.de
//  email                : roessel@zpr.uni-koeln.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Log$
// Revision 1.1  2002/09/25 16:57:31  roessel
// Initial commit.
//

#include <sstream>
#include <string>

template <class T>
inline std::string toString( const T& t ) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

#endif // TOSTRING_H


