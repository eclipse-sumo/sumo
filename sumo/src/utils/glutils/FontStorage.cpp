//---------------------------------------------------------------------------//
//                        FontStorage.cpp -
//  A temporary storage for fonts
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/06/18 11:24:29  dkrajzew
// parsing of character sets from char-arrays implemented
//
// Revision 1.2  2003/06/05 14:28:05  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <cassert>
#include <fstream>
#include "lfontrenderer.h"
#include "FontStorage.h"



/* =========================================================================
 * method definitions
 * ======================================================================= */
FontStorage::FontStorage()
{
}

FontStorage::~FontStorage()
{
    for(FontMap::iterator i=myFonts.begin(); i!=myFonts.end(); i++) {
        free ((*i).second.buf);
    }
}


void
FontStorage::add(const std::string &fontname, const std::string &filename)
{
    LFont font;
    uint id;
    uint t;
    uint i;
    float f;

    if ((filename == "") || (fontname == ""))
        throw "LFontRenderer::LoadFont - invalid filename or font name";
    std::ifstream file;
    file.open(filename.c_str(), std::ios::binary);
    if (!file.is_open())
        throw "LFontRenderer::LoadFont - error opening file";
    file.read((char*)&id, sizeof(id));
    if (id != 6666)
        throw "LFontRenderer::LoadFont - wrong file format";
    file.read((char*)&font.imageWidth, sizeof(uint));
    file.read((char*)&font.imageHeight, sizeof(uint));
    file.read((char*)&t, sizeof(uint));
    font.defaultHeight = t;
    for (i=0; i<256; i++)
    {
        file.read((char*)&t, sizeof(uint));
        font.chars[i].top = (float)t/(float)font.imageHeight;

        file.read((char*)&t, sizeof(uint));
        font.chars[i].left = (float)t/(float)font.imageWidth;

        file.read((char*)&t, sizeof(uint));
        font.chars[i].bottom = (float)t/(float)font.imageHeight;

        file.read((char*)&t, sizeof(uint));
        font.chars[i].right = (float)t/(float)font.imageWidth;

        file.read((char*)&t, sizeof(uint));
        font.chars[i].enabled = t != 0;

        file.read((char*)&f, sizeof(float));
        font.chars[i].widthFactor = f;
    }
    if ((file.eof()) || (file.fail()))
        throw "LFontRenderer::LoadFont - error reading from file";

    font.buf = (char*)malloc(font.imageHeight*font.imageWidth);
    if (font.buf == 0)
        throw "LFontRenderer::LoadFont - could not allocate memory";
    file.read(font.buf, font.imageWidth*font.imageHeight);
    if ((file.eof()) || (file.fail()))
    {
        free(font.buf);
        throw "LFontRenderer::LoadFont - error reading from file";
    }
    file.close();
    font.r = 1;
    font.g = 0;
    font.b = .5;
    font.height = font.defaultHeight;
    font.name = fontname;
    font.widthScale = 1;
    font.angle = 0;
    font.italic = false;

    myFonts[fontname] = font;
}


void
FontStorage::add(const std::string &fontname, const unsigned char * const characters)
{
    LFont font;
    uint id;
    uint t;
    uint i;
    float f;

    size_t off = 0;
    memcpy(&id, characters+off, sizeof(id)); off += sizeof(id);
    if (id != 6666)
        throw "LFontRenderer::LoadFont - wrong file format";
    memcpy((char*)&font.imageWidth, characters+off, sizeof(uint)); off +=sizeof(uint);
    memcpy((char*)&font.imageHeight, characters+off, sizeof(uint)); off +=sizeof(uint);
    memcpy((char*)&t, characters+off, sizeof(uint)); off +=sizeof(uint);
    font.defaultHeight = t;
    for (i=0; i<256; i++)
    {
        memcpy((char*)&t, characters+off, sizeof(uint)); off +=sizeof(uint);
        font.chars[i].top = (float)t/(float)font.imageHeight;

        memcpy((char*)&t, characters+off, sizeof(uint)); off +=sizeof(uint);
        font.chars[i].left = (float)t/(float)font.imageWidth;

        memcpy((char*)&t, characters+off, sizeof(uint)); off +=sizeof(uint);
        font.chars[i].bottom = (float)t/(float)font.imageHeight;

        memcpy((char*)&t, characters+off, sizeof(uint)); off +=sizeof(uint);
        font.chars[i].right = (float)t/(float)font.imageWidth;

        memcpy((char*)&t, characters+off, sizeof(uint)); off +=sizeof(uint);
        font.chars[i].enabled = t != 0;

        memcpy((char*)&f, characters+off, sizeof(float)); off +=sizeof(float);
        font.chars[i].widthFactor = f;
    }
    font.buf = (char*)malloc(font.imageHeight*font.imageWidth);
    if (font.buf == 0)
        throw "LFontRenderer::LoadFont - could not allocate memory";
    memcpy(font.buf, characters+off, font.imageWidth*font.imageHeight);
    font.r = 1;
    font.g = 0;
    font.b = .5;
    font.height = font.defaultHeight;
    font.name = fontname;
    font.widthScale = 1;
    font.angle = 0;
    font.italic = false;
    myFonts[fontname] = font;
}


LFont
FontStorage::get(const std::string &name) const
{
    FontMap::const_iterator i=myFonts.find(name);
    assert(myFonts.end()!=i);
    return (*i).second;
}


bool
FontStorage::has(const std::string &name) const
{
    return myFonts.find(name)!=myFonts.end();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "FontStorage.icc"
//#endif

// Local Variables:
// mode:C++
// End:


