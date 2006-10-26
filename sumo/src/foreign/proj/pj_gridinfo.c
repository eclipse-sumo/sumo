/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Functions for handling individual PJ_GRIDINFO's.  Includes
 *           loaders for all formats but CTABLE (in nad_init.c).
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam <warmerdam@pobox.com>
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
 * Revision 1.2  2006/10/26 10:26:22  dkrajzew
 * proj 4.5.0 added
 *
 * Revision 1.7  2005/07/07 00:16:03  fwarmerdam
 * Fixed debug fprintf syntax per:
 * http://bugzilla.remotesensing.org/show_bug.cgi?id=886
 *
 * Revision 1.6  2004/10/30 04:03:03  fwarmerdam
 * fixed reported information in ctable debug message
 *
 * Revision 1.5  2003/08/20 13:23:58  warmerda
 * Avoid unsigned char / char casting issues for VC++.
 *
 * Revision 1.4  2003/03/19 03:36:41  warmerda
 * Fixed so swap_words() works when it should.
 *
 * Revision 1.3  2003/03/17 19:44:45  warmerda
 * improved debugging, reduce header read size
 *
 * Revision 1.2  2003/03/17 18:56:34  warmerda
 * implement heirarchical NTv2 gridinfos
 *
 * Revision 1.1  2003/03/15 06:01:18  warmerda
 * New
 *
 */

#define PJ_LIB__

#include <projects.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

/************************************************************************/
/*                             swap_words()                             */
/*                                                                      */
/*      Convert the byte order of the given word(s) in place.           */
/************************************************************************/

static int  byte_order_test = 1;
#define IS_LSB	(((unsigned char *) (&byte_order_test))[0] == 1)

static void swap_words( unsigned char *data, int word_size, int word_count )

{
    int	word;

    for( word = 0; word < word_count; word++ )
    {
        int	i;

        for( i = 0; i < word_size/2; i++ )
        {
            int	t;

            t = data[i];
            data[i] = data[word_size-i-1];
            data[word_size-i-1] = t;
        }

        data += word_size;
    }
}

/************************************************************************/
/*                          pj_gridinfo_free()                          */
/************************************************************************/

void pj_gridinfo_free( PJ_GRIDINFO *gi )

{
    if( gi == NULL )
        return;

    if( gi->child != NULL )
    {
        PJ_GRIDINFO *child, *next;

        for( child = gi->child; child != NULL; child=next)
        {
            next=child->next;
            pj_gridinfo_free( child );
        }
    }

    if( gi->ct != NULL )
        nad_free( gi->ct );

    free( gi->gridname );
    if( gi->filename != NULL )
        free( gi->filename );

    pj_dalloc( gi );
}

/************************************************************************/
/*                          pj_gridinfo_load()                          */
/*                                                                      */
/*      This function is intended to implement delayed loading of       */
/*      the data contents of a grid file.  The header and related       */
/*      stuff are loaded by pj_gridinfo_init().                         */
/************************************************************************/

int pj_gridinfo_load( PJ_GRIDINFO *gi )

{
    if( gi == NULL || gi->ct == NULL )
        return 0;

/* -------------------------------------------------------------------- */
/*      ctable is currently loaded on initialization though there is    */
/*      no real reason not to support delayed loading for it as well.   */
/* -------------------------------------------------------------------- */
    if( strcmp(gi->format,"ctable") == 0 )
    {
        FILE *fid;
        int result;

        fid = pj_open_lib( gi->filename, "rb" );

        if( fid == NULL )
        {
            pj_errno = -38;
            return 0;
        }

        result = nad_ctable_load( gi->ct, fid );

        fclose( fid );

        return result;
    }

/* -------------------------------------------------------------------- */
/*      NTv1 format.                                                    */
/*      We process one line at a time.  Note that the array storage     */
/*      direction (e-w) is different in the NTv1 file and what          */
/*      the CTABLE is supposed to have.  The phi/lam are also           */
/*      reversed, and we have to be aware of byte swapping.             */
/* -------------------------------------------------------------------- */
    else if( strcmp(gi->format,"ntv1") == 0 )
    {
        double	*row_buf;
        int	row;
        FILE *fid;

        fid = pj_open_lib( gi->filename, "rb" );

        if( fid == NULL )
        {
            pj_errno = -38;
            return 0;
        }

        fseek( fid, gi->grid_offset, SEEK_SET );

        row_buf = (double *) pj_malloc(gi->ct->lim.lam * sizeof(double) * 2);
        gi->ct->cvs = (FLP *) pj_malloc(gi->ct->lim.lam*gi->ct->lim.phi*sizeof(FLP));
        if( row_buf == NULL || gi->ct->cvs == NULL )
        {
            pj_errno = -38;
            return 0;
        }

        for( row = 0; row < gi->ct->lim.phi; row++ )
        {
            int	    i;
            FLP     *cvs;
            double  *diff_seconds;

            if( fread( row_buf, sizeof(double), gi->ct->lim.lam * 2, fid )
                != 2 * gi->ct->lim.lam )
            {
                pj_dalloc( row_buf );
                pj_dalloc( gi->ct->cvs );
                pj_errno = -38;
                return 0;
            }

            if( IS_LSB )
                swap_words( (unsigned char *) row_buf, 8, gi->ct->lim.lam*2 );

            /* convert seconds to radians */
            diff_seconds = row_buf;

            for( i = 0; i < gi->ct->lim.lam; i++ )
            {
                cvs = gi->ct->cvs + (row) * gi->ct->lim.lam
                    + (gi->ct->lim.lam - i - 1);

                cvs->phi = *(diff_seconds++) * ((PI/180.0) / 3600.0);
                cvs->lam = *(diff_seconds++) * ((PI/180.0) / 3600.0);
            }
        }

        pj_dalloc( row_buf );

        fclose( fid );

        return 1;
    }

/* -------------------------------------------------------------------- */
/*      NTv2 format.                                                    */
/*      We process one line at a time.  Note that the array storage     */
/*      direction (e-w) is different in the NTv2 file and what          */
/*      the CTABLE is supposed to have.  The phi/lam are also           */
/*      reversed, and we have to be aware of byte swapping.             */
/* -------------------------------------------------------------------- */
    else if( strcmp(gi->format,"ntv2") == 0 )
    {
        float	*row_buf;
        int	row;
        FILE *fid;

        if( getenv("PROJ_DEBUG") != NULL )
        {
            fprintf( stderr, "NTv2 - loading grid %s\n", gi->ct->id );
        }

        fid = pj_open_lib( gi->filename, "rb" );

        if( fid == NULL )
        {
            pj_errno = -38;
            return 0;
        }

        fseek( fid, gi->grid_offset, SEEK_SET );

        row_buf = (float *) pj_malloc(gi->ct->lim.lam * sizeof(float) * 4);
        gi->ct->cvs = (FLP *) pj_malloc(gi->ct->lim.lam*gi->ct->lim.phi*sizeof(FLP));
        if( row_buf == NULL || gi->ct->cvs == NULL )
        {
            pj_errno = -38;
            return 0;
        }

        for( row = 0; row < gi->ct->lim.phi; row++ )
        {
            int	    i;
            FLP     *cvs;
            float   *diff_seconds;

            if( fread( row_buf, sizeof(float), gi->ct->lim.lam*4, fid )
                != 4 * gi->ct->lim.lam )
            {
                pj_dalloc( row_buf );
                pj_dalloc( gi->ct->cvs );
                gi->ct->cvs = NULL;
                pj_errno = -38;
                return 0;
            }

            if( !IS_LSB )
                swap_words( (unsigned char *) row_buf, 4,
                            gi->ct->lim.lam*4 );

            /* convert seconds to radians */
            diff_seconds = row_buf;

            for( i = 0; i < gi->ct->lim.lam; i++ )
            {
                cvs = gi->ct->cvs + (row) * gi->ct->lim.lam
                    + (gi->ct->lim.lam - i - 1);

                cvs->phi = *(diff_seconds++) * ((PI/180.0) / 3600.0);
                cvs->lam = *(diff_seconds++) * ((PI/180.0) / 3600.0);
                diff_seconds += 2; /* skip accuracy values */
            }
        }

        pj_dalloc( row_buf );

        fclose( fid );

        return 1;
    }

    else
    {
        return 0;
    }
}

/************************************************************************/
/*                       pj_gridinfo_init_ntv2()                        */
/*                                                                      */
/*      Load a ntv2 (.gsb) file.                                        */
/************************************************************************/

static int pj_gridinfo_init_ntv2( FILE *fid, PJ_GRIDINFO *gilist )

{
    unsigned char header[11*16];
    int num_subfiles, subfile;

    assert( sizeof(int) == 4 );
    assert( sizeof(double) == 8 );
    if( sizeof(int) != 4 || sizeof(double) != 8 )
    {
        fprintf( stderr,
                 "basic types of inappropraiate size in pj_gridinfo_init_ntv2()\n" );
        pj_errno = -38;
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Read the overview header.                                       */
/* -------------------------------------------------------------------- */
    if( fread( header, sizeof(header), 1, fid ) != 1 )
    {
        pj_errno = -38;
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Byte swap interesting fields if needed.                         */
/* -------------------------------------------------------------------- */
    if( !IS_LSB )
    {
        swap_words( header+8, 4, 1 );
        swap_words( header+8+16, 4, 1 );
        swap_words( header+8+32, 4, 1 );
        swap_words( header+8+7*16, 8, 1 );
        swap_words( header+8+8*16, 8, 1 );
        swap_words( header+8+9*16, 8, 1 );
        swap_words( header+8+10*16, 8, 1 );
    }

/* -------------------------------------------------------------------- */
/*      Get the subfile count out ... all we really use for now.        */
/* -------------------------------------------------------------------- */
    memcpy( &num_subfiles, header+8+32, 4 );

/* ==================================================================== */
/*      Step through the subfiles, creating a PJ_GRIDINFO for each.     */
/* ==================================================================== */
    for( subfile = 0; subfile < num_subfiles; subfile++ )
    {
        struct CTABLE *ct;
        LP ur;
        int gs_count;
        PJ_GRIDINFO *gi;

/* -------------------------------------------------------------------- */
/*      Read header.                                                    */
/* -------------------------------------------------------------------- */
        if( fread( header, sizeof(header), 1, fid ) != 1 )
        {
            pj_errno = -38;
            return 0;
        }

        if( strncmp((const char *) header,"SUB_NAME",8) != 0 )
        {
            pj_errno = -38;
            return 0;
        }

/* -------------------------------------------------------------------- */
/*      Byte swap interesting fields if needed.                         */
/* -------------------------------------------------------------------- */
        if( !IS_LSB )
        {
            swap_words( header+8+16*4, 8, 1 );
            swap_words( header+8+16*5, 8, 1 );
            swap_words( header+8+16*6, 8, 1 );
            swap_words( header+8+16*7, 8, 1 );
            swap_words( header+8+16*8, 8, 1 );
            swap_words( header+8+16*9, 8, 1 );
            swap_words( header+8+16*10, 4, 1 );
        }

/* -------------------------------------------------------------------- */
/*      Initialize a corresponding "ct" structure.                      */
/* -------------------------------------------------------------------- */
        ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
        strncpy( ct->id, (const char *) header + 8, 8 );
        ct->id[8] = '\0';

        ct->ll.lam = - *((double *) (header+7*16+8)); /* W_LONG */
        ct->ll.phi = *((double *) (header+4*16+8));   /* S_LAT */

        ur.lam = - *((double *) (header+6*16+8));     /* E_LONG */
        ur.phi = *((double *) (header+5*16+8));       /* N_LAT */

        ct->del.lam = *((double *) (header+9*16+8));
        ct->del.phi = *((double *) (header+8*16+8));

        ct->lim.lam = (int) (fabs(ur.lam-ct->ll.lam)/ct->del.lam + 0.5) + 1;
        ct->lim.phi = (int) (fabs(ur.phi-ct->ll.phi)/ct->del.phi + 0.5) + 1;

        if( getenv("PROJ_DEBUG") != NULL )
            fprintf( stderr,
                     "NTv2 %s %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)\n",
                     ct->id,
                     ct->lim.lam, ct->lim.phi,
                     ct->ll.lam/3600.0, ct->ll.phi/3600.0,
                     ur.lam/3600.0, ur.phi/3600.0 );

        ct->ll.lam *= DEG_TO_RAD/3600.0;
        ct->ll.phi *= DEG_TO_RAD/3600.0;
        ct->del.lam *= DEG_TO_RAD/3600.0;
        ct->del.phi *= DEG_TO_RAD/3600.0;

        memcpy( &gs_count, header + 8 + 16*10, 4 );
        if( gs_count != ct->lim.lam * ct->lim.phi )
        {
            fprintf( stderr,
                     "GS_COUNT(%d) does not match expected cells (%dx%d=%d)\n",
                     gs_count, ct->lim.lam, ct->lim.phi,
                     ct->lim.lam * ct->lim.phi );
            pj_errno = -38;
            return 0;
        }

        ct->cvs = NULL;

/* -------------------------------------------------------------------- */
/*      Create a new gridinfo for this if we aren't processing the      */
/*      1st subfile, and initialize our grid info.                      */
/* -------------------------------------------------------------------- */
        if( subfile == 0 )
            gi = gilist;
        else
        {
            gi = (PJ_GRIDINFO *) pj_malloc(sizeof(PJ_GRIDINFO));
            memset( gi, 0, sizeof(PJ_GRIDINFO) );

            gi->gridname = strdup( gilist->gridname );
            gi->filename = strdup( gilist->filename );
            gi->next = NULL;
        }

        gi->ct = ct;
        gi->format = "ntv2";
        gi->grid_offset = ftell( fid );

/* -------------------------------------------------------------------- */
/*      Attach to the correct list or sublist.                          */
/* -------------------------------------------------------------------- */
        if( strncmp((const char *)header+24,"NONE",4) == 0 )
        {
            if( gi != gilist )
            {
                PJ_GRIDINFO *lnk;

                for( lnk = gilist; lnk->next != NULL; lnk = lnk->next ) {}
                lnk->next = gi;
            }
        }

        else
        {
            PJ_GRIDINFO *lnk;
            PJ_GRIDINFO *gp = gilist;

            while( gp != NULL
                   && strncmp(gp->ct->id,(const char*)header+24,8) != 0 )
                gp = gp->next;

            if( gp == NULL )
            {
                if( getenv("PROJ_DEBUG") != NULL )
                    fprintf( stderr, "pj_gridinfo_init_ntv2(): "
                             "failed to find parent %8.8s for %s.\n",
                             (const char *) header+24, gi->ct->id );

                for( lnk = gp; lnk->next != NULL; lnk = lnk->next ) {}
                lnk->next = gi;
            }
            else if( gp->child == NULL )
            {
                gp->child = gi;
            }
            else
            {
                for( lnk = gp->child; lnk->next != NULL; lnk = lnk->next ) {}
                lnk->next = gi;
            }
        }

/* -------------------------------------------------------------------- */
/*      Seek past the data.                                             */
/* -------------------------------------------------------------------- */
        fseek( fid, gs_count * 16, SEEK_CUR );
    }

    return 1;
}

/************************************************************************/
/*                       pj_gridinfo_init_ntv1()                        */
/*                                                                      */
/*      Load an NTv1 style Canadian grid shift file.                    */
/************************************************************************/

static int pj_gridinfo_init_ntv1( FILE * fid, PJ_GRIDINFO *gi )

{
    unsigned char header[176];
    struct CTABLE *ct;
    LP		ur;

    assert( sizeof(int) == 4 );
    assert( sizeof(double) == 8 );
    if( sizeof(int) != 4 || sizeof(double) != 8 )
    {
        fprintf( stderr,
                 "basic types of inappropraiate size in nad_load_ntv1()\n" );
        pj_errno = -38;
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Read the header.                                                */
/* -------------------------------------------------------------------- */
    if( fread( header, sizeof(header), 1, fid ) != 1 )
    {
        pj_errno = -38;
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Regularize fields of interest.                                  */
/* -------------------------------------------------------------------- */
    if( IS_LSB )
    {
        swap_words( header+8, 4, 1 );
        swap_words( header+24, 8, 1 );
        swap_words( header+40, 8, 1 );
        swap_words( header+56, 8, 1 );
        swap_words( header+72, 8, 1 );
        swap_words( header+88, 8, 1 );
        swap_words( header+104, 8, 1 );
    }

    if( *((int *) (header+8)) != 12 )
    {
        pj_errno = -38;
        printf("NTv1 grid shift file has wrong record count, corrupt?\n");
        return 0;
    }

/* -------------------------------------------------------------------- */
/*      Fill in CTABLE structure.                                       */
/* -------------------------------------------------------------------- */
    ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
    strcpy( ct->id, "NTv1 Grid Shift File" );

    ct->ll.lam = - *((double *) (header+72));
    ct->ll.phi = *((double *) (header+24));
    ur.lam = - *((double *) (header+56));
    ur.phi = *((double *) (header+40));
    ct->del.lam = *((double *) (header+104));
    ct->del.phi = *((double *) (header+88));
    ct->lim.lam = (int) (fabs(ur.lam-ct->ll.lam)/ct->del.lam + 0.5) + 1;
    ct->lim.phi = (int) (fabs(ur.phi-ct->ll.phi)/ct->del.phi + 0.5) + 1;

    if( getenv("PROJ_DEBUG") != NULL )
        fprintf( stderr,
                 "NTv1 %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)\n",
                 ct->lim.lam, ct->lim.phi,
                 ct->ll.lam, ct->ll.phi, ur.lam, ur.phi );

    ct->ll.lam *= DEG_TO_RAD;
    ct->ll.phi *= DEG_TO_RAD;
    ct->del.lam *= DEG_TO_RAD;
    ct->del.phi *= DEG_TO_RAD;
    ct->cvs = NULL;

    gi->ct = ct;
    gi->grid_offset = ftell( fid );
    gi->format = "ntv1";

    return 1;
}

/************************************************************************/
/*                          pj_gridinfo_init()                          */
/*                                                                      */
/*      Open and parse header details from a datum gridshift file       */
/*      returning a list of PJ_GRIDINFOs for the grids in that          */
/*      file.  This superceeds use of nad_init() for modern             */
/*      applications.                                                   */
/************************************************************************/

PJ_GRIDINFO *pj_gridinfo_init( const char *gridname )

{
    char 	fname[MAX_PATH_FILENAME+1];
    PJ_GRIDINFO *gilist;
    FILE 	*fp;
    char	header[160];

    errno = pj_errno = 0;

/* -------------------------------------------------------------------- */
/*      Initialize a GRIDINFO with stub info we would use if it         */
/*      cannot be loaded.                                               */
/* -------------------------------------------------------------------- */
    gilist = (PJ_GRIDINFO *) pj_malloc(sizeof(PJ_GRIDINFO));
    memset( gilist, 0, sizeof(PJ_GRIDINFO) );

    gilist->gridname = strdup( gridname );
    gilist->filename = NULL;
    gilist->format = "missing";
    gilist->grid_offset = 0;
    gilist->ct = NULL;
    gilist->next = NULL;

/* -------------------------------------------------------------------- */
/*      Open the file using the usual search rules.                     */
/* -------------------------------------------------------------------- */
    strcpy(fname, gridname);
    if (!(fp = pj_open_lib(fname, "rb"))) {
        pj_errno = errno;
        return gilist;
    }

    gilist->filename = strdup(fname);

/* -------------------------------------------------------------------- */
/*      Load a header, to determine the file type.                      */
/* -------------------------------------------------------------------- */
    if( fread( header, sizeof(header), 1, fp ) != 1 )
    {
        fclose( fp );
        pj_errno = -38;
        return gilist;
    }

    fseek( fp, SEEK_SET, 0 );

/* -------------------------------------------------------------------- */
/*      Determine file type.                                            */
/* -------------------------------------------------------------------- */
    if( strncmp(header + 0, "HEADER", 6) == 0
        && strncmp(header + 96, "W GRID", 6) == 0
        && strncmp(header + 144, "TO      NAD83   ", 16) == 0 )
    {
        pj_gridinfo_init_ntv1( fp, gilist );
    }

    else if( strncmp(header + 0, "NUM_OREC", 8) == 0
             && strncmp(header + 48, "GS_TYPE", 7) == 0 )
    {
        pj_gridinfo_init_ntv2( fp, gilist );
    }

    else
    {
        struct CTABLE *ct = nad_ctable_init( fp );

        gilist->format = "ctable";
        gilist->ct = ct;

        if( getenv("PROJ_DEBUG") != NULL )
            fprintf( stderr,
                     "Ctable %s %dx%d: LL=(%.9g,%.9g) UR=(%.9g,%.9g)\n",
                     ct->id,
                     ct->lim.lam, ct->lim.phi,
                     ct->ll.lam * RAD_TO_DEG, ct->ll.phi * RAD_TO_DEG,
                     (ct->ll.lam + (ct->lim.lam-1)*ct->del.lam) * RAD_TO_DEG,
                     (ct->ll.phi + (ct->lim.phi-1)*ct->del.phi) * RAD_TO_DEG );
    }

    fclose(fp);

    return gilist;
}
