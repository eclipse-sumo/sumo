/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Built in datum list.
 * Author:   Frank Warmerdam, warmerda@home.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log$
 * Revision 1.1  2006/03/08 13:02:28  dkrajzew
 * some further work on converting geo-coordinates
 *
 * Revision 1.10  2004/10/28 16:07:55  fwarmerdam
 * added pj_get_*_ref() accessors
 *
 * Revision 1.9  2004/05/04 01:15:21  warmerda
 * Added NZGD49 datum - see bug 339
 *
 * Revision 1.8  2003/03/28 14:29:28  warmerda
 * Fixed ire65 definition ... use mod_airy, not modif_airy.
 *
 * Revision 1.7  2003/03/17 21:37:22  warmerda
 * make NAD27 grids optional, added alaska and ntv2
 *
 * Revision 1.6  2003/02/06 03:37:52  warmerda
 * Fixed typo in madrid prime meridian value.
 *
 * Revision 1.5  2003/01/15 14:34:55  warmerda
 * Added some datums as suggested by GRASS team
 *
 * Revision 1.4  2002/12/09 16:01:02  warmerda
 * added prime meridian support
 *
 * Revision 1.3  2002/07/08 02:32:05  warmerda
 * ensure clean C++ builds
 *
 * Revision 1.2  2001/04/05 19:32:41  warmerda
 * added ntv1_can.dat to NAD27 list
 *
 * Revision 1.1  2000/07/06 23:32:27  warmerda
 * New
 *
 */

#define PJ_DATUMS__

#include <projects.h>

/*
 * The ellipse code must match one from pj_ellps.c.  The datum id should
 * be kept to 12 characters or less if possible.  Use the official OGC
 * datum name for the comments if available.
 */

C_NAMESPACE struct PJ_DATUMS pj_datums[] = {
/* id       definition                               ellipse  comments */
/* --       ----------                               -------  -------- */
"WGS84",    "towgs84=0,0,0", 		             "WGS84", "",
"GGRS87",   "towgs84=-199.87,74.79,246.62",          "GRS80",
				"Greek_Geodetic_Reference_System_1987",
"NAD83",    "towgs84=0,0,0",                         "GRS80",
				"North_American_Datum_1983",
"NAD27",    "nadgrids=@conus,@alaska,@ntv2_0.gsb,@ntv1_can.dat",
                                                     "clrk66",
				"North_American_Datum_1927",
"potsdam",  "towgs84=606.0,23.0,413.0",  "bessel",  "Potsdam Rauenberg 1950 DHDN",
"carthage",  "towgs84=-263.0,6.0,431.0",  "clark80",  "Carthage 1934 Tunisia",
"hermannskogel", "towgs84=653.0,-212.0,449.0",  "bessel",  "Hermannskogel",
"ire65",  "towgs84=482.530,-130.596,564.557,-1.042,-0.214,-0.631,8.15",  "mod_airy",  "Ireland 1965",
"nzgd49",    "towgs84=59.47,-5.04,187.44,0.47,-0.1,1.024,-4.5993", "intl", "New Zealand Geodetic Datum 1949",
NULL,       NULL,                                    NULL,    NULL
};

struct PJ_DATUMS *pj_get_datums_ref()

{
    return pj_datums;
}

C_NAMESPACE struct PJ_PRIME_MERIDIANS pj_prime_meridians[] = {
    /* id        definition                         */
    /* --        ----------                         */
    "greenwich", "0dE",
    "lisbon",    "9d07'54.862\"W",
    "paris",     "2d20'14.025\"E",
    "bogota",    "74d04'51.3\"E",
    "madrid",    "3d41'16.58\"W",
    "rome",      "12d27'8.4\"E",
    "bern",      "7d26'22.5\"E",
    "jakarta",   "106d48'27.79\"E",
    "ferro",     "17d40'W",
    "brussels",  "4d22'4.71\"E",
    "stockholm", "18d3'29.8\"E",
    "athens",    "23d42'58.815\"E",
    "oslo",      "10d43'22.5\"E",
    NULL,        NULL
};

struct PJ_PRIME_MERIDIANS *pj_get_prime_meridians_ref()

{
    return pj_prime_meridians;
}

