/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Perform overall coordinate system to coordinate system
 *           transformations (pj_transform() function) including reprojection
 *           and datum shifting.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
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
 * Revision 1.13  2004/10/25 15:34:36  fwarmerdam
 * make names of geodetic funcs from geotrans unique
 *
 * Revision 1.12  2004/05/03 19:45:23  warmerda
 * Altered so that raw ellpses are treated as a essentially having a
 * +towgs84=0,0,0 specification so ellpisoid shifts are applied.
 * Fixed so that prime meridian shifts are applied if the coordinate system is
 * not lat/long (ie. if it is projected).  This fixes:
 * http://bugzilla.remotesensing.org/show_bug.cgi?id=510
 *
 * Revision 1.11  2004/01/24 09:37:19  warmerda
 * pj_transform() will no longer return an error code if any of the points are
 * transformable.  In this case the application is expected to check for
 * HUGE_VAL to identify failed points.
 * As part of the implementation, I added a list of pj_errno values that
 * are transient (ie per-point) rather than indicating global failure for the
 * coordinate system definition.  We use this in deciding which pj_fwd and
 * pj_inv error codes are really fatal and should be reported.
 *
 * Revision 1.10  2003/08/21 02:09:06  warmerda
 * added a bunch of HUGE_VAL checking
 *
 * Revision 1.9  2003/03/26 16:52:30  warmerda
 * added check that an inverse transformation func exists
 *
 * Revision 1.8  2002/12/14 20:35:43  warmerda
 * implement units support for geocentric coordinates
 *
 * Revision 1.7  2002/12/14 20:14:35  warmerda
 * added geocentric support
 *
 * Revision 1.6  2002/12/09 16:01:02  warmerda
 * added prime meridian support
 *
 * Revision 1.5  2002/12/01 19:25:26  warmerda
 * applied fix for 7 param shift in pj_geocentric_from_wgs84, see bug 194
 *
 * Revision 1.4  2002/02/15 14:30:36  warmerda
 * provide default Z array if none passed in in pj_datum_transform()
 *
 * Revision 1.3  2001/04/04 21:13:21  warmerda
 * do arcsecond/radian and ppm datum parm transformation in pj_set_datum()
 *
 * Revision 1.2  2001/04/04 16:08:08  warmerda
 * rewrote 7 param datum shift to match EPSG:9606, now works with example
 *
 * Revision 1.1  2000/07/06 23:32:27  warmerda
 * New
 *
 */

#include <projects.h>
#include <string.h>
#include <math.h>
#include "geocent.h"

PJ_CVSID("$Id$");

#ifndef SRS_WGS84_SEMIMAJOR
#define SRS_WGS84_SEMIMAJOR 6378137.0
#endif

#ifndef SRS_WGS84_ESQUARED
#define SRS_WGS84_ESQUARED 0.006694379990
#endif

#define Dx_BF (defn->datum_params[0])
#define Dy_BF (defn->datum_params[1])
#define Dz_BF (defn->datum_params[2])
#define Rx_BF (defn->datum_params[3])
#define Ry_BF (defn->datum_params[4])
#define Rz_BF (defn->datum_params[5])
#define M_BF  (defn->datum_params[6])

/*
** This table is intended to indicate for any given error code in
** the range 0 to -44, whether that error will occur for all locations (ie.
** it is a problem with the coordinate system as a whole) in which case the
** value would be 0, or if the problem is with the point being transformed
** in which case the value is 1.
**
** At some point we might want to move this array in with the error message
** list or something, but while experimenting with it this should be fine.
*/

static int transient_error[45] = {
    /*             0  1  2  3  4  5  6  7  8  9   */
    /* 0 to 9 */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 10 to 19 */ 0, 0, 0, 0, 1, 1, 0, 1, 1, 1,
    /* 20 to 29 */ 1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    /* 30 to 39 */ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    /* 40 to 44 */ 0, 0, 0, 0, 0 };

/************************************************************************/
/*                            pj_transform()                            */
/*                                                                      */
/*      Currently this function doesn't recognise if two projections    */
/*      are identical (to short circuit reprojection) because it is     */
/*      difficult to compare PJ structures (since there are some        */
/*      projection specific components).                                */
/************************************************************************/

int pj_transform( PJ *srcdefn, PJ *dstdefn, long point_count, int point_offset,
                  double *x, double *y, double *z )

{
    long      i;
    int       need_datum_shift;

    pj_errno = 0;

    if( point_offset == 0 )
        point_offset = 1;

/* -------------------------------------------------------------------- */
/*      Transform geocentric source coordinates to lat/long.            */
/* -------------------------------------------------------------------- */
    if( srcdefn->is_geocent )
    {
        if( z == NULL )
        {
            pj_errno = PJD_ERR_GEOCENTRIC;
            return PJD_ERR_GEOCENTRIC;
        }

        if( srcdefn->to_meter != 1.0 )
        {
            for( i = 0; i < point_count; i++ )
            {
                x[point_offset*i] *= srcdefn->to_meter;
                y[point_offset*i] *= srcdefn->to_meter;
            }
        }

        if( pj_geocentric_to_geodetic( srcdefn->a, srcdefn->es,
                                       point_count, point_offset,
                                       x, y, z ) != 0)
            return pj_errno;
    }

/* -------------------------------------------------------------------- */
/*      Transform source points to lat/long, if they aren't             */
/*      already.                                                        */
/* -------------------------------------------------------------------- */
    else if( !srcdefn->is_latlong )
    {
        if( srcdefn->inv == NULL )
        {
            pj_errno = -17; /* this isn't correct, we need a no inverse err */
            if( getenv( "PROJ_DEBUG" ) != NULL )
            {
                fprintf( stderr,
                       "pj_transform(): source projection not invertable\n" );
            }
            return pj_errno;
        }

        for( i = 0; i < point_count; i++ )
        {
            XY         projected_loc;
            LP	       geodetic_loc;

            projected_loc.u = x[point_offset*i];
            projected_loc.v = y[point_offset*i];

            if( projected_loc.u == HUGE_VAL )
                continue;

            geodetic_loc = pj_inv( projected_loc, srcdefn );
            if( pj_errno != 0 )
            {
                if( pj_errno > 0 || pj_errno < -44 || point_count == 1
                    || transient_error[-pj_errno] == 0 )
                    return pj_errno;
                else
                {
                    geodetic_loc.u = HUGE_VAL;
                    geodetic_loc.v = HUGE_VAL;
                }
            }

            x[point_offset*i] = geodetic_loc.u;
            y[point_offset*i] = geodetic_loc.v;
        }
    }
/* -------------------------------------------------------------------- */
/*      But if they are already lat long, adjust for the prime          */
/*      meridian if there is one in effect.                             */
/* -------------------------------------------------------------------- */
    if( srcdefn->from_greenwich != 0.0 )
    {
        for( i = 0; i < point_count; i++ )
        {
            if( x[point_offset*i] != HUGE_VAL )
                x[point_offset*i] += srcdefn->from_greenwich;
        }
    }

/* -------------------------------------------------------------------- */
/*      Convert datums if needed, and possible.                         */
/* -------------------------------------------------------------------- */
    if( pj_datum_transform( srcdefn, dstdefn, point_count, point_offset,
                            x, y, z ) != 0 )
        return pj_errno;

/* -------------------------------------------------------------------- */
/*      But if they are staying lat long, adjust for the prime          */
/*      meridian if there is one in effect.                             */
/* -------------------------------------------------------------------- */
    if( dstdefn->from_greenwich != 0.0 )
    {
        for( i = 0; i < point_count; i++ )
        {
            if( x[point_offset*i] != HUGE_VAL )
                x[point_offset*i] -= dstdefn->from_greenwich;
        }
    }


/* -------------------------------------------------------------------- */
/*      Transform destination latlong to geocentric if required.        */
/* -------------------------------------------------------------------- */
    if( dstdefn->is_geocent )
    {
        if( z == NULL )
        {
            pj_errno = PJD_ERR_GEOCENTRIC;
            return PJD_ERR_GEOCENTRIC;
        }

        pj_geodetic_to_geocentric( dstdefn->a, dstdefn->es,
                                   point_count, point_offset, x, y, z );

        if( dstdefn->fr_meter != 1.0 )
        {
            for( i = 0; i < point_count; i++ )
            {
                if( x[point_offset*i] != HUGE_VAL )
                {
                    x[point_offset*i] *= dstdefn->fr_meter;
                    y[point_offset*i] *= dstdefn->fr_meter;
                }
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Transform destination points to projection coordinates, if      */
/*      desired.                                                        */
/* -------------------------------------------------------------------- */
    else if( !dstdefn->is_latlong )
    {
        for( i = 0; i < point_count; i++ )
        {
            XY         projected_loc;
            LP	       geodetic_loc;

            geodetic_loc.u = x[point_offset*i];
            geodetic_loc.v = y[point_offset*i];

            if( geodetic_loc.u == HUGE_VAL )
                continue;

            projected_loc = pj_fwd( geodetic_loc, dstdefn );
            if( pj_errno != 0 )
            {
                if( pj_errno > 0 || pj_errno < -44 || point_count == 1
                    || transient_error[-pj_errno] == 0 )
                    return pj_errno;
                else
                {
                    projected_loc.u = HUGE_VAL;
                    projected_loc.v = HUGE_VAL;
                }
            }

            x[point_offset*i] = projected_loc.u;
            y[point_offset*i] = projected_loc.v;
        }
    }

    return 0;
}

/************************************************************************/
/*                     pj_geodetic_to_geocentric()                      */
/************************************************************************/

int pj_geodetic_to_geocentric( double a, double es,
                               long point_count, int point_offset,
                               double *x, double *y, double *z )

{
    double b;
    int    i;

    if( es == 0.0 )
        b = a;
    else
        b = a * sqrt(1-es);

    if( pj_Set_Geocentric_Parameters( a, b ) != 0 )
    {
        pj_errno = PJD_ERR_GEOCENTRIC;
        return pj_errno;
    }

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;

        if( pj_Convert_Geodetic_To_Geocentric( y[io], x[io], z[io],
                                            x+io, y+io, z+io ) != 0 )
        {
            pj_errno = PJD_ERR_GEOCENTRIC;
            return PJD_ERR_GEOCENTRIC;
        }
    }

    return 0;
}

/************************************************************************/
/*                     pj_geodetic_to_geocentric()                      */
/************************************************************************/

int pj_geocentric_to_geodetic( double a, double es,
                               long point_count, int point_offset,
                               double *x, double *y, double *z )

{
    double b;
    int    i;

    if( es == 0.0 )
        b = a;
    else
        b = a * sqrt(1-es);

    if( pj_Set_Geocentric_Parameters( a, b ) != 0 )
    {
        pj_errno = PJD_ERR_GEOCENTRIC;
        return pj_errno;
    }

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;

        if( x[io] == HUGE_VAL )
            continue;

        pj_Convert_Geocentric_To_Geodetic( x[io], y[io], z[io],
                                        y+io, x+io, z+io );
    }

    return 0;
}

/************************************************************************/
/*                         pj_compare_datums()                          */
/*                                                                      */
/*      Returns TRUE if the two datums are identical, otherwise         */
/*      FALSE.                                                          */
/************************************************************************/

int pj_compare_datums( PJ *srcdefn, PJ *dstdefn )

{
    if( srcdefn->datum_type != dstdefn->datum_type )
    {
        return 0;
    }
    else if( srcdefn->a != dstdefn->a
             || ABS(srcdefn->es - dstdefn->es) > 0.000000000050 )
    {
        /* the tolerence for es is to ensure that GRS80 and WGS84 are
           considered identical */
        return 0;
    }
    else if( srcdefn->datum_type == PJD_3PARAM )
    {
        return (srcdefn->datum_params[0] == dstdefn->datum_params[0]
                && srcdefn->datum_params[1] == dstdefn->datum_params[1]
                && srcdefn->datum_params[2] == dstdefn->datum_params[2]);
    }
    else if( srcdefn->datum_type == PJD_7PARAM )
    {
        return (srcdefn->datum_params[0] == dstdefn->datum_params[0]
                && srcdefn->datum_params[1] == dstdefn->datum_params[1]
                && srcdefn->datum_params[2] == dstdefn->datum_params[2]
                && srcdefn->datum_params[3] == dstdefn->datum_params[3]
                && srcdefn->datum_params[4] == dstdefn->datum_params[4]
                && srcdefn->datum_params[5] == dstdefn->datum_params[5]
                && srcdefn->datum_params[6] == dstdefn->datum_params[6]);
    }
    else if( srcdefn->datum_type == PJD_GRIDSHIFT )
    {
        return strcmp( pj_param(srcdefn->params,"snadgrids").s,
                       pj_param(dstdefn->params,"snadgrids").s ) == 0;
    }
    else
        return 1;
}

/************************************************************************/
/*                       pj_geocentic_to_wgs84()                        */
/************************************************************************/

int pj_geocentric_to_wgs84( PJ *defn,
                            long point_count, int point_offset,
                            double *x, double *y, double *z )

{
    int       i;

    pj_errno = 0;

    if( defn->datum_type == PJD_3PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;

            if( x[io] == HUGE_VAL )
                continue;

            x[io] = x[io] + defn->datum_params[0];
            y[io] = y[io] + defn->datum_params[1];
            z[io] = z[io] + defn->datum_params[2];
        }
    }
    else if( defn->datum_type == PJD_7PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;
            double x_out, y_out, z_out;

            if( x[io] == HUGE_VAL )
                continue;

            x_out = M_BF*(       x[io] - Rz_BF*y[io] + Ry_BF*z[io]) + Dx_BF;
            y_out = M_BF*( Rz_BF*x[io] +       y[io] - Rx_BF*z[io]) + Dy_BF;
            z_out = M_BF*(-Ry_BF*x[io] + Rx_BF*y[io] +       z[io]) + Dz_BF;

            x[io] = x_out;
            y[io] = y_out;
            z[io] = z_out;
        }
    }

    return 0;
}

/************************************************************************/
/*                      pj_geocentic_from_wgs84()                       */
/************************************************************************/

int pj_geocentric_from_wgs84( PJ *defn,
                              long point_count, int point_offset,
                              double *x, double *y, double *z )

{
    int       i;

    pj_errno = 0;

    if( defn->datum_type == PJD_3PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;

            if( x[io] == HUGE_VAL )
                continue;

            x[io] = x[io] - defn->datum_params[0];
            y[io] = y[io] - defn->datum_params[1];
            z[io] = z[io] - defn->datum_params[2];
        }
    }
    else if( defn->datum_type == PJD_7PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;
            double x_tmp, y_tmp, z_tmp;

            if( x[io] == HUGE_VAL )
                continue;

            x_tmp = (x[io] - Dx_BF) / M_BF;
            y_tmp = (y[io] - Dy_BF) / M_BF;
            z_tmp = (z[io] - Dz_BF) / M_BF;

            x[io] =        x_tmp + Rz_BF*y_tmp - Ry_BF*z_tmp;
            y[io] = -Rz_BF*x_tmp +       y_tmp + Rx_BF*z_tmp;
            z[io] =  Ry_BF*x_tmp - Rx_BF*y_tmp +       z_tmp;
        }
    }

    return 0;
}

/************************************************************************/
/*                         pj_datum_transform()                         */
/************************************************************************/

int pj_datum_transform( PJ *srcdefn, PJ *dstdefn,
                        long point_count, int point_offset,
                        double *x, double *y, double *z )

{
    double      src_a, src_es, dst_a, dst_es;
    int         z_is_temp = FALSE;

    pj_errno = 0;

/* -------------------------------------------------------------------- */
/*      Short cut if the datums are identical.                          */
/* -------------------------------------------------------------------- */
    if( pj_compare_datums( srcdefn, dstdefn ) )
        return 0;

    src_a = srcdefn->a;
    src_es = srcdefn->es;

    dst_a = dstdefn->a;
    dst_es = dstdefn->es;

/* -------------------------------------------------------------------- */
/*      Create a temporary Z array if one is not provided.              */
/* -------------------------------------------------------------------- */
    if( z == NULL )
    {
        int	bytes = sizeof(double) * point_count * point_offset;
        z = (double *) pj_malloc(bytes);
        memset( z, 0, bytes );
        z_is_temp = TRUE;
    }

#define CHECK_RETURN {if( pj_errno != 0 ) { if( z_is_temp ) pj_dalloc(z); return pj_errno; }}

/* -------------------------------------------------------------------- */
/*	If this datum requires grid shifts, then apply it to geodetic   */
/*      coordinates.                                                    */
/* -------------------------------------------------------------------- */
    if( srcdefn->datum_type == PJD_GRIDSHIFT )
    {
        pj_apply_gridshift( pj_param(srcdefn->params,"snadgrids").s, 0,
                            point_count, point_offset, x, y, z );
        CHECK_RETURN;

        src_a = SRS_WGS84_SEMIMAJOR;
        src_es = 0.006694379990;
    }

    if( dstdefn->datum_type == PJD_GRIDSHIFT )
    {
        dst_a = SRS_WGS84_SEMIMAJOR;
        dst_es = 0.006694379990;
    }

/* ==================================================================== */
/*      Do we need to go through geocentric coordinates?                */
/* ==================================================================== */
    if( src_es != dst_es || src_a != dst_a
        || srcdefn->datum_type == PJD_3PARAM
        || srcdefn->datum_type == PJD_7PARAM
        || dstdefn->datum_type == PJD_3PARAM
        || dstdefn->datum_type == PJD_7PARAM)
    {
/* -------------------------------------------------------------------- */
/*      Convert to geocentric coordinates.                              */
/* -------------------------------------------------------------------- */
        pj_geodetic_to_geocentric( src_a, src_es,
                                   point_count, point_offset, x, y, z );
        CHECK_RETURN;

/* -------------------------------------------------------------------- */
/*      Convert between datums.                                         */
/* -------------------------------------------------------------------- */
        if( srcdefn->datum_type == PJD_3PARAM
            || srcdefn->datum_type == PJD_7PARAM )
        {
            pj_geocentric_to_wgs84( srcdefn, point_count, point_offset,x,y,z);
            CHECK_RETURN;
        }

        if( dstdefn->datum_type == PJD_3PARAM
            || dstdefn->datum_type == PJD_7PARAM )
        {
            pj_geocentric_from_wgs84( dstdefn, point_count,point_offset,x,y,z);
            CHECK_RETURN;
        }

/* -------------------------------------------------------------------- */
/*      Convert back to geodetic coordinates.                           */
/* -------------------------------------------------------------------- */
        pj_geocentric_to_geodetic( dst_a, dst_es,
                                   point_count, point_offset, x, y, z );
        CHECK_RETURN;
    }

/* -------------------------------------------------------------------- */
/*      Apply grid shift to destination if required.                    */
/* -------------------------------------------------------------------- */
    if( dstdefn->datum_type == PJD_GRIDSHIFT )
    {
        pj_apply_gridshift( pj_param(dstdefn->params,"snadgrids").s, 1,
                            point_count, point_offset, x, y, z );
        CHECK_RETURN;
    }

    if( z_is_temp )
        pj_dalloc( z );

    return 0;
}

