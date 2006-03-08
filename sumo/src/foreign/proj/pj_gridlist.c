/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Code to manage the list of currently loaded (cached) PJ_GRIDINFOs
 *           See pj_gridinfo.c for details of loading individual grids.
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
 * Revision 1.1  2006/03/08 13:02:28  dkrajzew
 * some further work on converting geo-coordinates
 *
 * Revision 1.3  2003/03/18 16:26:58  warmerda
 * clear error if missing file is not required
 *
 * Revision 1.2  2003/03/17 19:45:47  warmerda
 * support '@' marker for optional grids
 *
 * Revision 1.1  2003/03/15 06:01:18  warmerda
 * New
 *
 */

#define PJ_LIB__

#include <projects.h>
#include <string.h>
#include <math.h>
#include <assert.h>

static PJ_GRIDINFO *grid_list = NULL;

/* used only by pj_load_nadgrids() and pj_deallocate_grids() */

static int           last_nadgrids_max = 0;
static int           last_nadgrids_count = 0;
static PJ_GRIDINFO **last_nadgrids_list = NULL;
static char         *last_nadgrids = NULL;

/************************************************************************/
/*                        pj_deallocate_grids()                         */
/*                                                                      */
/*      Deallocate all loaded grids.                                    */
/************************************************************************/

void pj_deallocate_grids()

{
    while( grid_list != NULL )
    {
        PJ_GRIDINFO *item = grid_list;
        grid_list = grid_list->next;
        item->next = NULL;

        pj_gridinfo_free( item );
    }

    if( last_nadgrids != NULL )
    {
        pj_dalloc( last_nadgrids );
        last_nadgrids = NULL;

        pj_dalloc( last_nadgrids_list );
        last_nadgrids_list = NULL;

        last_nadgrids_count = 0;
        last_nadgrids_max = 0;
    }
}

/************************************************************************/
/*                       pj_gridlist_merge_grid()                       */
/*                                                                      */
/*      Find/load the named gridfile and merge it into the              */
/*      last_nadgrids_list.                                             */
/************************************************************************/

static int pj_gridlist_merge_gridfile( const char *gridname )

{
    int i, got_match=0;
    PJ_GRIDINFO *this_grid, *tail = NULL;

/* -------------------------------------------------------------------- */
/*      Try to find in the existing list of loaded grids.  Add all      */
/*      matching grids as with NTv2 we can get many grids from one      */
/*      file (one shared gridname).                                     */
/* -------------------------------------------------------------------- */
    for( this_grid = grid_list; this_grid != NULL; this_grid = this_grid->next)
    {
        if( strcmp(this_grid->gridname,gridname) == 0 )
        {
            got_match = 1;

            /* dont add to the list if it is invalid. */
            if( this_grid->ct == NULL )
                return 0;

            /* do we need to grow the list? */
            if( last_nadgrids_count >= last_nadgrids_max - 2 )
            {
                PJ_GRIDINFO **new_list;
                int new_max = last_nadgrids_max + 20;

                new_list = (PJ_GRIDINFO **) pj_malloc(sizeof(void*) * new_max);
                if( last_nadgrids_list != NULL )
                {
                    memcpy( new_list, last_nadgrids_list,
                            sizeof(void*) * last_nadgrids_max );
                    pj_dalloc( last_nadgrids_list );
                }

                last_nadgrids_list = new_list;
                last_nadgrids_max = new_max;
            }

            /* add to the list */
            last_nadgrids_list[last_nadgrids_count++] = this_grid;
            last_nadgrids_list[last_nadgrids_count] = NULL;
        }

        tail = this_grid;
    }

    if( got_match )
        return 1;

/* -------------------------------------------------------------------- */
/*      Try to load the named grid.                                     */
/* -------------------------------------------------------------------- */
    this_grid = pj_gridinfo_init( gridname );

    if( this_grid == NULL )
    {
        /* we should get at least a stub grid with a missing "ct" member */
        assert( FALSE );
        return 0;
    }

    if( tail != NULL )
        tail->next = this_grid;
    else
        grid_list = this_grid;

/* -------------------------------------------------------------------- */
/*      Recurse to add the grid now that it is loaded.                  */
/* -------------------------------------------------------------------- */
    return pj_gridlist_merge_gridfile( gridname );
}

/************************************************************************/
/*                     pj_gridlist_from_nadgrids()                      */
/*                                                                      */
/*      This functions loads the list of grids corresponding to a       */
/*      particular nadgrids string into a list, and returns it.  The    */
/*      list is kept around till a request is made with a different     */
/*      string in order to cut down on the string parsing cost, and     */
/*      the cost of building the list of tables each time.              */
/************************************************************************/

PJ_GRIDINFO **pj_gridlist_from_nadgrids( const char *nadgrids, int *grid_count)

{
    const char *s;

    pj_errno = 0;
    *grid_count = 0;

    if( last_nadgrids != NULL
        && strcmp(nadgrids,last_nadgrids) == 0 )
    {
        *grid_count = last_nadgrids_count;
        return last_nadgrids_list;
    }

/* -------------------------------------------------------------------- */
/*      Free old one, if any, and make space for new list.              */
/* -------------------------------------------------------------------- */
    if( last_nadgrids != NULL )
    {
        pj_dalloc(last_nadgrids);
    }

    last_nadgrids = (char *) pj_malloc(strlen(nadgrids)+1);
    strcpy( last_nadgrids, nadgrids );

    last_nadgrids_count = 0;

/* -------------------------------------------------------------------- */
/*      Loop processing names out of nadgrids one at a time.            */
/* -------------------------------------------------------------------- */
    for( s = nadgrids; *s != '\0'; )
    {
        int   end_char;
        int   required = 1;
        char  name[128];

        if( *s == '@' )
        {
            required = 0;
            s++;
        }

        for( end_char = 0;
             s[end_char] != '\0' && s[end_char] != ',';
             end_char++ ) {}

        if( end_char > sizeof(name) )
        {
            pj_errno = -38;
            return NULL;
        }

        strncpy( name, s, end_char );
        name[end_char] = '\0';

        s += end_char;
        if( *s == ',' )
            s++;

        if( !pj_gridlist_merge_gridfile( name ) && required )
        {
            pj_errno = -38;
            return NULL;
        }
        else
            pj_errno = 0;
    }

    if( last_nadgrids_count > 0 )
    {
        *grid_count = last_nadgrids_count;
        return last_nadgrids_list;
    }
    else
        return NULL;
}
