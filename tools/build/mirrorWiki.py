#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    mirrorWiki.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2011-10-20
# @version $Id$

"""
Mirrors wiki-documentation.

Determines what to mirror, first: if a command line argument is given,
it is interpreted as the page to mirror. Otherwise, "API:AllPages" is
used for obtaining the list of all pages which will be
converted in subsequent steps.

For each of the pages to mirror, the page is downloaded as for
being edited and is stripped from wiki-header/footer, first.
Then, the image-links are extracted from the HTML page and stored
temporarily.
The page is saved into MIRROR_FOLDER/<PAGE_PATH>.

After mirroring all pages, the images are downloaded and stored into
MIRROR_FOLDER/images.
"""
from __future__ import absolute_import
from __future__ import print_function
try:
    from urllib import urlopen
except ImportError:
    from urllib.request import urlopen
import os
import json
import io
from optparse import OptionParser


def getAllPages(args):
    if len(args) == 0:
        f = urlopen("https://sumo.dlr.de/w/api.php?action=query&list=allpages&aplimit=500&format=json")
        result = json.loads(f.read().decode('utf8'))
        return [entry["title"].replace(" ", "_") for entry in result["query"]["allpages"]]
    return [a.replace(" ", "_") for a in args]


def readParsePage(page):
    f = urlopen("https://sumo.dlr.de/wiki/%s" % page)
    c = f.read().decode('utf8')
    b = c.find("This page was last modified on")
    e = c.find("<", b)
    lastMod = c[b:e]
    b = c.find('<a id="top"')
    e = c.find('<div class="printfooter">')
    c = c[b:e]
    c = c.replace('<h3 id="siteSub">From sumo</h3>', '')
    b = c.find('<div id="jump-to-nav"')
    e = c.find('</div>', b) + 6
    return c[:b] + c[e:] + '</div><hr/><div id="lastmod">' + lastMod + '</div>'


def readParseEditPage(page):
    f = urlopen(
        "https://sumo.dlr.de/w/index.php?title=%s&action=edit" % page)
    c = f.read().decode('utf8')
    b = c.find("wpTextbox1")
    b = c.find('>', b) + 1
    e = c.find("</textarea>")
    return c[b:e]


def getImages(page):
    images = set()
    for t in ["Image:", "File:"]:
        b = page.find(t)
        while b >= 0:
            e = len(page)
            for ch in ["|", "\n", "]"]:
                pos = page.find(ch, b)
                if pos >= 0 and pos < e:
                    e = pos
            images.add(page[b:e].strip())
            b = page.find(t, b + 1)
    return images


if __name__ == "__main__":
    optParser = OptionParser()
    optParser.add_option(
        "-o", "--output", default="wiki", help="output folder")
    (options, args) = optParser.parse_args()

    try:
        os.makedirs(os.path.join(options.output, "images"))
    except Exception:
        pass
    images = set()
    for name in getAllPages(args):
        print("Fetching %s" % name)
        c = readParseEditPage(name)
        if name.find("/") > 0:
            try:
                os.makedirs(
                    os.path.join(options.output, name[:name.rfind("/")]))
            except Exception:
                pass
            images.update(getImages(c))
        name = name + ".txt"
        fd = io.open(os.path.join(options.output, name), "w", encoding="utf8")
        fd.write(c)
        fd.close()

    for i in images:
        print("Fetching image %s" % i)
        if i.find(":") >= 0:
            f = urlopen("https://sumo.dlr.de/wiki/%s" % i)
            c = f.read().decode('utf8')
            b = c.find("<div class=\"fullImageLink\" id=\"file\">")
            b = c.find("href=", b) + 6
            e = c.find("\"", b + 1)
            f = urlopen("https://sumo.dlr.de/%s" % c[b:e])
            i = i[i.find(":") + 1:]
        else:
            f = urlopen("https://sumo.dlr.de/%s" % i)
            i = i[i.rfind("/") + 1:]
        if i.find("px-") >= 0:
            i = i[:i.find('-') + 1]
        fd = open(os.path.join(options.output, "images", i), "wb")
        fd.write(f.read())
        fd.close()
