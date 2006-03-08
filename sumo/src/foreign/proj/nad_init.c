/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Load datum shift files into memory.
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
 * Revision 1.8  2003/03/17 18:56:01  warmerda
 * implement delayed loading of ctable format files
 *
 * Revision 1.7  2003/03/15 06:02:02  warmerda
 * preliminary NTv2 support, major restructure of datum shifting
 *
 * Revision 1.6  2002/07/08 02:32:05  warmerda
 * ensure clean C++ builds
 *
 * Revision 1.5  2002/04/30 16:26:07  warmerda
 * trip newlines of ctable id field
 *
 * Revision 1.4  2001/08/17 17:28:37  warmerda
 * removed use of emess()
 *
 * Revision 1.3  2001/04/05 19:31:54  warmerda
 * substantially reorganized and added NTv1 support
 *
 */

#define PJ_LIB__

#include <projects.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

/************************************************************************/
/*                          nad_ctable_load()                           */
/*                                                                      */
/*      Load the data portion of a ctable formatted grid.               */
/************************************************************************/

int nad_ctable_load( struct CTABLE *ct, FILE *fid )

{
    int  a_size;

    fseek( fid, sizeof(struct CTABLE), SEEK_SET );

    /* read all the actual shift values */
    a_size = ct->lim.lam * ct->lim.phi;
    ct->cvs = (FLP *) pj_malloc(sizeof(FLP) * a_size);
    if( ct->cvs == NULL
        || fread(ct->cvs, sizeof(FLP), a_size, fid) != a_size )
    {
        pj_errno = -38;
        return 0;
    }

    return 1;
}

/************************************************************************/
/*                          nad_ctable_init()                           */
/*                                                                      */
/*      Read the header portion of a "ctable" format grid.              */
/************************************************************************/

struct CTABLE *nad_ctable_init( FILE * fid )
{
    struct CTABLE *ct;
    int		id_end;

    /* read the table header */
    ct = (struct CTABLE *) pj_malloc(sizeof(struct CTABLE));
    if( ct == NULL
        || fread( ct, sizeof(struct CTABLE), 1, fid ) != 1 )
    {
        pj_errno = -38;
        return NULL;
    }

    /* do some minimal validation to ensure the structure isn't corrupt */
    if( ct->lim.lam < 1 || ct->lim.lam > 100000
        || ct->lim.phi < 1 || ct->lim.phi > 100000 )
    {
        pj_errno = -38;
        return NULL;
    }

    /* trim white space and newlines off id */
    for( id_end = strlen(ct->id)-1; id_end > 0; id_end-- )
    {
        if( ct->id[id_end] == '\n' || ct->id[id_end] == ' ' )
            ct->id[id_end] = '\0';
        else
            break;
    }

    ct->cvs = NULL;

    return ct;
}

/************************************************************************/
/*                              nad_init()                              */
/*                                                                      */
/*      Read a datum shift file in any of the supported binary formats. */
/************************************************************************/

struct CTABLE *nad_init(char *name)
{
    char 	fname[MAX_PATH_FILENAME+1];
    struct CTABLE *ct;
    FILE 	*fid;
    char	header[512];

    errno = pj_errno = 0;

/* -------------------------------------------------------------------- */
/*      Open the file using the usual search rules.                     */
/* -------------------------------------------------------------------- */
    strcpy(fname, name);
    if (!(fid = pj_open_lib(fname, "rb"))) {
        pj_errno = errno;
        return 0;
    }

    ct = nad_ctable_init( fid );
    if( ct != NULL )
    {
        if( !nad_ctable_load( ct, fid ) )
        {
            nad_free( ct );
            ct = NULL;
        }
    }

    fclose(fid);
    return ct;
}

/************************************************************************/
/*                              nad_free()                              */
/*                                                                      */
/*      Free a CTABLE grid shift structure produced by nad_init().      */
/************************************************************************/

void nad_free(struct CTABLE *ct)
{
    if (ct) {
        if( ct->cvs != NULL )
            pj_dalloc(ct->cvs);

        pj_dalloc(ct);
    }
}
