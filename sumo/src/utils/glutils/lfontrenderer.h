/* ----------------------------------------------------------------------------
Copyright (c) 2002, Lev Povalahev
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * The name of the author may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

#ifndef LFONTRENDERER_H
#define LFONTRENDERER_H
//--------------------------------------------------

#include <string>
#include <vector>
#include <qgl.h> // !!! not really a good solution; where to get openGl-defs from?
#ifdef _WIN32
#include <windows.h>
//#include <glut.h>
#endif // _WIN32

//--------------------------------------------------

typedef unsigned int uint;
typedef unsigned char byte;

#define DEFAULT_HEIGHT                  99999

class LCharCoords
{
public:
    float left;
    float top;
    float right;
    float bottom;
    bool enabled;
    float widthFactor;
};

class LFont
{
public:
    std::string name;
    uint textureId;
    uint defaultHeight;
    uint height;
    uint imageWidth;
    uint imageHeight;
    float widthScale;
    bool italic;
    float angle;
    float r, g, b;
    LCharCoords chars[256];
    char *buf;

};

class LStringData
{
public:
    std::string str;
    uint font;  // the font to be used
    uint height; // the height of the font
    float r, g, b; // the color
    float angle; // the rotation angle
    float x, y;  // the position
    float widthScale; // width scale
    bool italic; // true if the font is italic
};

//--------------------------------------------------
class LFontRenderer
{
public:
    // default constructor, initializes the object
    LFontRenderer();
    // the destructor, automatically called by delete
    virtual ~LFontRenderer();
    // returns the number of fonts loaded
    uint GetFontCount();
    // returns the name of the given font, index _must_ be smaller than GetFontCount()
    const std::string& GetFontName(uint index);
    // sets the active font, this function is also available in a version that takes the index as parameter
    void SetActiveFont(const std::string& name);
    // sets the active font, this function is also available in a version that takes the name
    // of the font as parameter
    void SetActiveFont(uint index);
    // returns the index of the active font, throws an exception if no fonts are loaded
    uint GetActiveFont();
    // this function loads a font from a file and adds it to the list of fonts it returns the index of
    // the new font. The font name paramater is a name for the font that can be used with
    // SetActiveFont() etc.
    uint LoadFont(const std::string& fontname, const std::string& filename);
    // removes all fonts and clears any memory allocated
    void Clear();
    // sets the height of the current font
    void SetHeight(uint height);
    // sets the color of the active font
    void SetColor(float red, float green, float blue);
    // returns teh height of the active font
    uint GetHeight();
    // call this to get the color of the active font
    void GetColor(float &red, float &green, float &blue);
    // call thos method to output a string
    void StringOut(float x, float y, const std::string &string);
    // this method draws all the strings
    void Draw();
    // this method draws all the strings rescaling the viewport to the given size
    void Draw(int x, int y);
    // this methods sets the width scale of the active font
    void SetWidthScale(float scale);
    // this method returns the width scale of the active font
    float GetWidthScale();
    // this method sets the rotation angle for the active font
    void SetRotation(float angle);
    // this method returns the rotation angle for the active font
    float GetRotation();
    // this function is used to set either the active font should be italic or not
    void SetItalic(bool value);
    // this function returns true if the active font is italic
    bool GetItalic();
    // this function returns the width of the string. font metrics of the active font
    // are used
    uint GetStringWidth(const std::string &string);


    void add(LFont font);
protected:
    void myDraw(int x, int y, int w, int h);

protected:
    // used internally
    uint m_strCount;
    // the index of the active font
    uint m_activeFont;
    // the array of fonts
    std::vector<LFont> m_fonts;
    // the strings to be rendered
    std::vector<LStringData> m_strings;
    // the size of the viewport
    uint m_viewTop;
    uint m_viewLeft;
    uint m_viewWidth;
    uint m_viewHeight;
#ifdef GL_VERSION_1_3
    // used internally, the number of supported texture units;
    uint m_textureUnits;
#endif // GL_VERSION_1_3
};

//--------------------------------------------------
#endif // LFONTRENDERER_H

