#include <string>
#include <map>
#include <cassert>
#include <fstream>
#include "lfontrenderer.h"
#include "FontStorage.h"

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

