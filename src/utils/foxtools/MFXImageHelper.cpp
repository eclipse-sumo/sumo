/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2005-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MFXImageHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    2005-05-04
///
// missing_desc
/****************************************************************************/
#include <config.h>

#include "fxheader.h"
#include <FXPNGImage.h>
#include <FXJPGImage.h>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244 4242) // do not warn about integer conversions
#endif
#include <FXTIFImage.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <utils/common/ToString.h>
#include "MFXImageHelper.h"


void
MFXImageHelper::checkSupported(FXString ext) {
    if (comparecase(ext, "png") == 0) {
        if (!FXPNGImage::supported) {
            throw InvalidArgument("Fox was compiled without png support!");
        }
    } else if (comparecase(ext, "jpg") == 0 || comparecase(ext, "jpeg") == 0) {
        if (!FXJPGImage::supported) {
            throw InvalidArgument("Fox was compiled without jpg support!");
        }
    } else if (comparecase(ext, "tif") == 0 || comparecase(ext, "tiff") == 0) {
        if (!FXTIFImage::supported) {
            throw InvalidArgument("Fox was compiled without tif support!");
        }
    }
}


FXImage*
MFXImageHelper::loadImage(FXApp* a, const std::string& file) {
    FXString ext = FXPath::extension(file.c_str());
    checkSupported(ext);
    FXImage* img = nullptr;
    if (comparecase(ext, "gif") == 0) {
        img = new FXGIFImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "bmp") == 0) {
        img = new FXBMPImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "xpm") == 0) {
        img = new FXXPMImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "pcx") == 0) {
        img = new FXPCXImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "ico") == 0 || comparecase(ext, "cur") == 0) {
        img = new FXICOImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "tga") == 0) {
        img = new FXTGAImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "rgb") == 0) {
        img = new FXRGBImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "xbm") == 0) {
        img = new FXXBMImage(a, nullptr, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "png") == 0) {
        img = new FXPNGImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "jpg") == 0 || comparecase(ext, "jpeg") == 0) {
        img = new FXJPGImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else if (comparecase(ext, "tif") == 0 || comparecase(ext, "tiff") == 0) {
        img = new FXTIFImage(a, nullptr, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
    } else {
        throw InvalidArgument("Unknown file extension '" + toString(ext.text()) + "' for image '" + file + "'!");
    }

    FXFileStream stream;
    if (img != nullptr && stream.open(file.c_str(), FXStreamLoad)) {
        a->beginWaitCursor();
        img->loadPixels(stream);
        stream.close();

        img->create();
        a->endWaitCursor();
    } else {
        delete img;
        throw InvalidArgument("Loading failed!");
    }
    return img;
}


FXbool
MFXImageHelper::scalePower2(FXImage* image, const int maxSize) {
    FXint newHeight = 0;
    for (FXint exp = 30; exp >= 0; exp--) {
        newHeight = 2 << exp;
        if (newHeight <= maxSize && (image->getHeight() & newHeight)) {
            break;
        }
    }
    if (2 * newHeight <= maxSize && (2 * newHeight - image->getHeight() < image->getHeight() - newHeight)) {
        newHeight *= 2;
    }
    FXint newWidth = 0;
    for (FXint exp = 30; exp >= 0; exp--) {
        newWidth = 2 << exp;
        if (newWidth <= maxSize && (image->getWidth() & newWidth)) {
            break;
        }
    }
    if (2 * newWidth <= maxSize && (2 * newWidth - image->getWidth() < image->getWidth() - newWidth)) {
        newWidth *= 2;
    }
    if (newHeight == image->getHeight() && newWidth == image->getWidth()) {
        return false;
    }
    image->scale(newWidth, newHeight);
    return true;
}


// smell: yellow (the save functions may have additional options, not regarded)
// Save file
FXbool
MFXImageHelper::saveImage(const std::string& file,
                          int width, int height, FXColor* data) {
    FXString ext = FXPath::extension(file.c_str());
    checkSupported(ext);
    enum class ImageFormat {
        GIF, BMP, XPM, PCX, ICO, TGA, RGB, XBM, PNG, JPG, TIF
    };
    ImageFormat format;
    if (comparecase(ext, "gif") == 0) {
        format = ImageFormat::GIF;
    } else if (comparecase(ext, "bmp") == 0) {
        format = ImageFormat::BMP;
    } else if (comparecase(ext, "xpm") == 0) {
        format = ImageFormat::XPM;
    } else if (comparecase(ext, "pcx") == 0) {
        format = ImageFormat::PCX;
    } else if (comparecase(ext, "ico") == 0 || comparecase(ext, "cur") == 0) {
        format = ImageFormat::ICO;
    } else if (comparecase(ext, "tga") == 0) {
        format = ImageFormat::TGA;
    } else if (comparecase(ext, "rgb") == 0) {
        format = ImageFormat::RGB;
    } else if (comparecase(ext, "xbm") == 0) {
        format = ImageFormat::XBM;
    } else if (comparecase(ext, "png") == 0) {
        format = ImageFormat::PNG;
    } else if (comparecase(ext, "jpg") == 0 || comparecase(ext, "jpeg") == 0) {
        format = ImageFormat::JPG;
    } else if (comparecase(ext, "tif") == 0 || comparecase(ext, "tiff") == 0) {
        format = ImageFormat::TIF;
    } else {
        throw InvalidArgument("Unknown file extension for image!");
    }
    FXFileStream stream;
    if (!stream.open(file.c_str(), FXStreamSave)) {
        throw InvalidArgument("Could not open file for writing!");
    }
    switch (format) {
        case ImageFormat::GIF:
            return fxsaveGIF(stream, data, width, height, false /* !!! "fast" */);
        case ImageFormat::BMP:
            return fxsaveBMP(stream, data, width, height);
        case ImageFormat::XPM:
            return fxsaveXPM(stream, data, width, height);
        case ImageFormat::PCX:
            return fxsavePCX(stream, data, width, height);
        case ImageFormat::ICO:
            return fxsaveICO(stream, data, width, height);
        case ImageFormat::TGA:
            return fxsaveTGA(stream, data, width, height);
        case ImageFormat::RGB:
            return fxsaveRGB(stream, data, width, height);
        case ImageFormat::XBM:
            return fxsaveXBM(stream, data, width, height);
        case ImageFormat::PNG:
            return fxsavePNG(stream, data, width, height);
        case ImageFormat::JPG:
            return fxsaveJPG(stream, data, width, height, 75);
        case ImageFormat::TIF:
            return fxsaveTIF(stream, data, width, height, 0);
        default:
            return false;
    }
}


/****************************************************************************/
