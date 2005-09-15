/********************************************************************************
*                                                                               *
*               Lockable object base type (used in threading)                   *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003 by Mathew Robertson.   All Rights Reserved.                *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
********************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fxdefs.h>
using namespace FX;
#include "FXLockable.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG
using namespace FXEX;
namespace FXEX {

FXLockable::FXLockable() : lock_(0) {}

}

