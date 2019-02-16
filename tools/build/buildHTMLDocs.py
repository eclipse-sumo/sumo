#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    buildHTMLDocs.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2011-10-20
# @version $Id$

"""
Converts wiki-documentation into HTML pages.

Determines what to convert, first: if a command line argument is given,
it is interpreted as the page to convert. Otherwise, "API:AllPages" is
used for obtaining the list of all pages which will be
converted in subsequent steps.

For each of the pages to convert, the HTML-representation of the
page is downloaded and stripped from wiki-header/footer, first.
Then, the image-links are extracted from the HTML page and stored
temporarily, the links themselves are patched to point to local pages/images
(if the page behind the link exists).
The page is saved into options.mirror/<PAGE_PATH>.

After parsing all pages, the images are downloaded and stored into
options.mirror/images.

After downloading all data, the title page is extracted and the content
included in this page is extracted. This content is embedded into "index.html"
between the <!-- nav begins --> / <!-- nav ends --> markers.
All pages downloaded earlier are loaded, and embedded into the index.html
between the <!-- content begins --> / <!-- content ends --> markers. Then,
the page is saved into options.output/<PAGE_PATH>. All images are copied
from options.mirror/images to options.output/images.
"""
from __future__ import absolute_import
from __future__ import print_function
try:
    from urllib import urlopen
except ImportError:
    from urllib.request import urlopen
import os
import sys
import shutil
import datetime
import pydoc
import types
from optparse import OptionParser

from mirrorWiki import readParsePage, readParseEditPage, getAllPages
TOOLDIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(TOOLDIR)
from sumolib.miscutils import working_dir  # noqa


def patchLinks(page, name):
    images = set()
    level = len(name.split("/")) - 1
    level = "../" * level
    b = page.find(" href=")
    while b >= 0:
        # images/files
        if page[b + 7:].startswith("/wiki/File:") or page[b + 7:].startswith("/wiki/Image:"):
            b2 = b
            b = page.find(":", b) + 1
            images.add(page[b2 + 7:page.find("\"", b)])
            page = page[:b2 + 7] + level + "images/" + page[b:]
        # pages (HTML)
        elif page[b + 7:].startswith("/wiki/"):
            e = b + 7 + 6
            e2 = page.find("\"", b + 7)
            link = page[e:e2]
            if "action=edit" not in link:
                if link.find("#") > 0:
                    link = level + link.replace("#", ".html#")
                elif link.find("#") < 0 and not link.endswith((".png", ".jpg", ".svg")):
                    link = level + link + ".html"
                page = page[:b + 7] + link + page[e2:]
        # pages (pydoc)
        else:
            lb = page.find('"', b) + 1
            le = page.find('"', lb)
            link = page[lb:le]
            p = link.find("daily/pydoc")
            if p >= 0:
                link = level + ".." + link[p + 5:]
                page = page[:lb] + link + page[le:]
        b = page.find(" href=", b + 1)
    return page, images, level


def patchImages(page, name):
    images = set()
    level = len(name.split("/")) - 1
    level = "../" * level
    b = page.find("<img ")
    b = page.find("src", b)
    while b >= 0:
        b += 5
        e = page.find("\"", b + 2)
        add = page[b:e]
        ls = add[add.rfind("/"):]
        if add.find("thumb") >= 0:
            ls = ls[ls.find("-") + 1:]
        images.add(add)
        page = page[:b] + level + "images/" + ls + page[e:]
        b = page.find("<img", b + 1)
        b = page.find("src", b)
    page = page.replace(".svg.png", ".svg")
    return page, images


def parseWikiLink(l):
    if l.find("[[") >= 0:
        # internal link
        b = l.find("[") + 2
        e = l.find("]", b)
        t = l[b:e]
        if t.find("|") < 0:
            link = t
            text = t
        else:
            link, text = t.split("|")
        link = link.replace(" ", "_")
        if link.find("#") >= 0:
            link = link.replace("#", ".html#")
        else:
            link = link + ".html"
        # external link
    elif l.find("[") >= 0:
        b = l.find("[") + 1
        e = l.find("]", b)
        t = l[b:e]
        link = t[:t.find(" ")]
        text = t[t.find(" ") + 1:]
    else:
        # text
        text = l[l.find(" ") + 1:]
        link = ""
    return text, link


def pydoc_recursive(module):
    pydoc.writedoc(module)
    for submod in module.__dict__.values():
        if type(submod) is types.ModuleType and submod.__name__.startswith(module.__name__):
            pydoc_recursive(submod)


def generate_pydoc(out_dir):
    os.mkdir(out_dir)
    import traci
    import sumolib
    with working_dir(out_dir):
        for module in (traci, sumolib):
            pydoc_recursive(module)


optParser = OptionParser()
optParser.add_option("-m", "--mirror", default="mirror", help="mirror folder")
optParser.add_option("-o", "--output", default="docs", help="output folder")
optParser.add_option("-p", "--pydoc-output", help="output folder for pydoc")
optParser.add_option("-i", "--index", default=os.path.join(os.path.dirname(
    __file__), "..", "..", "docs", "wiki", "index.html"), help="index template file")
optParser.add_option("-r", "--version", help="add version info")
optParser.add_option("-c", "--clean", action="store_true", default=False, help="remove output dirs")
(options, args) = optParser.parse_args()

if options.pydoc_output:
    if options.clean:
        shutil.rmtree(options.pydoc_output, ignore_errors=True)
    generate_pydoc(options.pydoc_output)
if options.clean:
    shutil.rmtree(options.mirror, ignore_errors=True)
    shutil.rmtree(options.output, ignore_errors=True)
try:
    os.mkdir(options.mirror)
except Exception:
    pass
try:
    os.mkdir(options.mirror + "/images")
except Exception:
    pass
images = set()
pages = getAllPages(args)
for name in pages:
    if name.endswith(".css"):
        print("Skipping css-file %s" % name)
        continue
    print("Fetching %s" % name)
    c = readParsePage(name)
    if name.find("/") > 0:
        try:
            os.makedirs(os.path.join(options.mirror, name[:name.rfind("/")]))
        except Exception:
            pass
    if True:  # name.find(".")<0:
        c, pi, level = patchLinks(c, name)
        for i in pi:
            images.add(i)
        c, pi = patchImages(c, name)
        for i in pi:
            images.add(i)
    name = name + ".html"
    fd = open(os.path.join(options.mirror, name), "wb")
    fd.write(c.encode("utf8"))
    fd.close()

imageFiles = []
for i in images:
    print("Fetching image %s" % i)
    if i.startswith("https://"):
        f = urlopen(i)
        i = i[i.rfind("/") + 1:]
    if i.find(":") >= 0:
        f = urlopen("https://sumo.dlr.de%s" % i)
        c = f.read()
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
    fd = open(os.path.join(options.mirror, "images", i), "wb")
    fd.write(f.read())
    fd.close()
    imageFiles.append(os.path.join("images", i))

# build navigation
nav = readParseEditPage("SUMO_User_Documentation")
lines = nav[nav.find("="):].split("\n")
level = 0
c = ""
hadHeader = False
for l in lines:
    if len(l) == 0:
        continue
    if l[0] == '=':
        text, link = parseWikiLink(" " + l.replace("=", ""))
        if hadHeader:
            if level > 0:
                c += "</ul></li>\n" * level
        spc = ' ' * (level + 1)
        c = c + spc + "<li>"
        if link != "":
            c = c + "<a href=\"" + link + "\">"
        c = c + text
        if link != "":
            c = c + "</a>"
        c = c + "</li>\n"
        hadHeader = True
        level = 0
        continue
    if l[0].find('*') < 0:
        continue
    text, link = parseWikiLink(l)
    nLevel = l.count('*')
    if level > nLevel:
        c = c + ("</ul></li>\n" * (level - nLevel))
    if level < nLevel:
        c = c + \
            ('<li style="list-style: none; display: inline"><ul>\n' *
             (nLevel - level))
    spc = ' ' * (nLevel + 1)
    # + str(level) + "-" + str(nLevel)
    c = c + spc + "<li>"
    if link != "":
        c = c + '<a href="' + link + '">' + text + '</a>'
    else:
        c = c + text
    c = c + "</li>\n"
    level = nLevel
if level > 0:
    c += "</ul></li>\n" * level

# get template and embed navigation
fd = open(options.index)
tpl = fd.read()
fd.close()
b = tpl.find("<!-- nav begins -->")
b = tpl.find(">", b) + 1
e = tpl.find("<!-- nav ends -->")
tpl = tpl[:b] + c + tpl[e:]

# build HTML pages
try:
    os.mkdir(options.output)
except Exception:
    pass
try:
    os.mkdir(options.output + "/images")
except Exception:
    pass
for name in pages:
    if name.endswith(".css"):
        print("Skipping css-file %s" % name)
        continue
    fromStr = 'generated on %s from <a href="https://sumo.dlr.de/wiki/%s">the wiki page for %s</a>' % (
        datetime.datetime.now(), name, name)
    name = name + ".html"
    t = os.path.join(options.output, name)
    fd = open(os.path.join(options.mirror, name), 'rb')
    c = fd.read().decode("utf8")
    if options.version:
        fromStr += " for SUMO %s" % options.version
    c = c.replace('<div id="siteSub" class="noprint">From Sumo</div>',
                  '<div id="siteSub" class="noprint">%s</div>' % fromStr)
    navLink = c.find('<a class="mw-jump-link" ')
    while navLink > 0:
        c = c[:navLink] + c[c.find('</a>', navLink)+4:]
        navLink = c.find('<a class="mw-jump-link" ')
    fd.close()
    #
    if name.find('/') >= 0:
        level = name.count("/")
    else:
        level = name.count("\\")
    level = "../" * level
    # title
    cc = tpl.replace("<title>", "<title>" + name[:name.rfind(".html")] + " - ")
    # css inclusion
    cc = cc.replace("sumo.css", level + "sumo.css")
    cc = cc.replace("logo.png", level + "logo.png")
    # links
    b = cc.find("<a href=")
    while b >= 0:
        b = cc.find('"', b)
        if not cc[b + 1:].startswith("http"):
            cc = cc[:b + 1] + level + cc[b + 1:]
        b = cc.find("<a href=", b)
    # content
    b = cc.find("<!-- content begins -->")
    e = cc.find("<!-- content ends -->", b)
    e = cc.find("<", e + 1) - 1
    cc = cc[:b] + c + cc[e:]

    try:
        os.makedirs(os.path.split(t)[0])
    except Exception:
        pass
    fd = open(t, "wb")
    fd.write(cc.encode("utf8"))
    fd.close()
for i in imageFiles:
    shutil.copy(
        os.path.join(options.mirror, i), os.path.join(options.output, i))
if os.path.exists(os.path.join(options.output, 'SUMO_User_Documentation.html')):
    shutil.copy(os.path.join(options.output, 'SUMO_User_Documentation.html'),
                os.path.join(options.output, 'index.html'))
