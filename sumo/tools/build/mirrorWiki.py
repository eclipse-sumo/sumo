#!/usr/bin/env python
"""
@file    getWikiPages.py
@author  Daniel.Krajzewicz@dlr.de
@date    2011-10-20
@version $Id$

Mirrors wiki-documentation.

Determines what to mirror, first: if a command line argument is given,
it is interpreted as the page to mirror. Otherwise, "Special:AllPages" is
downloaded and parsed for obtaining the list of all pages which will be
converted in subsequent steps.

For each of the pages to mirror, the page is downloaded as for
being edited and is stripped from wiki-header/footer, first.
Then, the image-links are extracted from the HTML page and stored 
temporarily.
The page is saved into MIRROR_FOLDER/<PAGE_PATH>. 

After mirroring all pages, the images are downloaded and stored into 
MIRROR_FOLDER/images.

Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import urllib, os, sys, shutil

def readParsePage(page):
    f = urllib.urlopen("http://sourceforge.net/apps/mediawiki/sumo/index.php?title=%s" % page)
    c = f.read()
    b = c.find("This page was last modified on");
    e = c.find("<", b)
    lastMod = c[b:e]
    b = c.find("globalWrapper")
    b = c.find('<a name="top"', b)
    e = c.find("<div class=\"printfooter\">")
    c = c[b:e]
    c = c.replace("<h3 id=\"siteSub\">From sumo</h3>", "")
    b = c.find("<div id=\"jump-to-nav\">")
    e = c.find("</div>", b)+6
    c = c[:b] + c[e:]
    c = c + '</div><hr/><div id="lastmod">' + lastMod + '</div>'
    return c
    
def readParseEditPage(page):
    f = urllib.urlopen("http://sourceforge.net/apps/mediawiki/sumo/index.php?title=%s&action=edit" % page)
    c = f.read()
    b = c.find("wpTextbox1")
    b = c.find('>', b) + 1
    e = c.find("</textarea>")
    return c[b:e]
    
def getImagesFromLinks(page, name):
    images = set()
    b = page.find("<a href")
    while b>=0:
        # images
        if page[b+9:].startswith("File:") or page[b+9:].startswith("Image:"):
            images.add(page[b+9:page.find("\"",b+9)])
            e = page.find(":", b+9)+1
        # images/files
        elif page[b+9:].startswith("/apps/mediawiki/sumo/index.php?title=File:") or page[b+9:].startswith("/apps/mediawiki/sumo/index.php?title=Image:"):
            b2 = b
            b = page.find("title=", b)+6
            images.add(page[b:page.find("\"",b)])
            e = page.find(":", b)+1
        # pages (HTML)
        elif page[b+9:].startswith("/apps/mediawiki/sumo/index.php"):
            e = page.find("?", b+9)+7
        b = page.find("<a href", b+1)
    return images

def getImages(page, name):
    images = set()
    b = page.find("<img ")
    b = page.find("src", b)
    while b>=0:
        b= b + 5	    
        e = page.find("\"", b+2)
        add = page[b:e]
        images.add(add)
    return images


MIRROR_FOLDER = "wiki"

try: os.mkdir(MIRROR_FOLDER)
except: pass
try: os.mkdir(MIRROR_FOLDER + "/images")
except: pass
images = set()
if len(sys.argv)<2:
    p = readParsePage("Special:AllPages")
    p = p[p.find("<input type=\"submit\" value=\"Go\" />"):]
    p = p[p.find("<table "):]
    pages = p.split("<a ")
else:
    pages = ["href=?title=" + sys.argv[1] + "\""]
for p in pages:
    if(not p.startswith("href")):
        continue
    b = p.find("?title=")
    e = p.find("\"", b)
    name = p[b+7:e]
    print "Fetching %s" % name
    c = readParseEditPage(name)
    if name.find("/")>0:
        try: 
            os.makedirs(os.path.join(MIRROR_FOLDER, name[:name.rfind("/")]))
        except: pass
    if True:
        pi = getImagesFromLinks(c, name)
        for i in pi:
            images.add(i)
        pi = getImages(c, name)
        for i in pi:
            images.add(i)
    name = name + ".txt"
    fd = open(os.path.join(MIRROR_FOLDER, name), "w")
    fd.write(c)
    fd.close()

imageFiles = []
for i in images:
    print "Fetching image %s" % i
    if i.find(":")>=0:
        f = urllib.urlopen("http://sourceforge.net/apps/mediawiki/sumo/index.php?title=%s" % i)
        c = f.read()
        b = c.find("<div class=\"fullImageLink\" id=\"file\">")
        b = c.find("href=", b)+6
        e = c.find("\"", b+1)
        f = urllib.urlopen("http://sourceforge.net/%s" % c[b:e])
        i = i[i.find(":")+1:]
    else:
        f = urllib.urlopen("http://sourceforge.net/%s" % i)
        i = i[i.rfind("/")+1:]
    if i.find("px-")>=0:
        i = i[:i.find('-')+1]
    fd = open(os.path.join(MIRROR_FOLDER, "images", i), "wb")
    fd.write(f.read())
    fd.close()
    imageFiles.append(os.path.join("images", i))
