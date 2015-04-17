// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:

/*
 * Copyright (C) 2013 Wu Yongwei <adah at users dot sourceforge dot net>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software.  If you use this
 *    software in a product, an acknowledgement in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must
 *    not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 * This file is part of Stones of Nvwa:
 *      http://sourceforge.net/projects/nvwa
 *
 */

/**
 * @file  _nvwa.h
 *
 * Common definitions for preprocessing.
 *
 * @date  2013-04-22
 */

#ifndef NVWA_NVWA_H
#define NVWA_NVWA_H

#ifndef NVWA_USE_NAMESPACE
#define NVWA_USE_NAMESPACE 1
#endif // NVWA_USE_NAMESPACE

#if NVWA_USE_NAMESPACE
#define NVWA_NAMESPACE_BEGIN namespace nvwa {
#define NVWA_NAMESPACE_END }
#define NVWA nvwa
#else  // NVWA_USE_NAMESPACE
#define NVWA_NAMESPACE_BEGIN
#define NVWA_NAMESPACE_END
#define NVWA
#endif // NVWA_USE_NAMESPACE

#endif // NVWA_NVWA_H
