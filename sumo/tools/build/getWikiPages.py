#!/usr/bin/env python
"""
@file    getWikiPages.py
@author  Daniel Krajzewicz
@date    2011-10-20
@version $Id$

Mirrors the pages by scanning "Special:AllPages" first.
The downloaded and stripped pages are written into DEST_FOLDER.
Images are written into DEST_FOLDER/images.

Copyright (C) 2010-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import urllib, os, sys

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
    
def patchLinks(page, name):
    images = set()
    level = len(name.split("/"))-1
    level = "../" * level
    b = page.find("<a href")
    while b>=0:
        if page[b+9:].startswith("File:") or page[b+9:].startswith("Image:"):
            images.add(page[b+9:page.find("\"",b+9)])
            e = page.find(":", b+9)+1
            page = page[:b] + level + "images/" + page[e:]
        elif page[b+9:].startswith("/apps/mediawiki/sumo/index.php?title=File:") or page[b+9:].startswith("/apps/mediawiki/sumo/index.php?title=Image:"):
            b2 = b
            b = page.find("title=", b)+6
            images.add(page[b:page.find("\"",b)])
            e = page.find(":", b)+1
            page = page[:b2+9] + level + "images/" + page[e:]
        elif page[b+9:].startswith("/apps/mediawiki/sumo/index.php"):
            e = page.find("?", b+9)+7
            e2 = page.find("\"", b+9)
            link = page[e:e2]
            if link.find("action=edit")<0:
                if link.find("#")>0:
                    link = level + link.replace("#", ".html#")
                elif link.find("#")<0 and not (link.endswith(".png") or link.endswith(".jpg") or link.endswith(".svg")):
                    link = level + link + ".html"
                page = page[:b+9] + link + page[e2:]
            else:
                page = page[:b+9] + "http://sourceforge.net/" + page[b+10:]
        b = page.find("<a href", b+1)
    return page, images, level

def patchImages(page, name):
    images = set()
    level = len(name.split("/"))-1
    level = "../" * level
    b = page.find("<img ")
    b = page.find("src", b)
    while b>=0:
        b= b + 5	    
        e = page.find("\"", b+2)
        add = page[b:e]
        l = add[add.rfind("/"):]
        if add.find("thumb")>=0:
            l = l[l.find("-")+1:]
        images.add(add)
        page = page[:b] + level + "images/" + l + page[e:]
        b = page.find("<img", b+1)
        b = page.find("src", b)
    page = page.replace(".svg.png", ".svg")
    return page, images

DEST_FOLDER = "mirror"

try: os.mkdir(DEST_FOLDER)
except: pass
try: os.mkdir(DEST_FOLDER + "/images")
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
    if name.endswith(".css"):
        print "Skipping css-file %s" % name
    print "Fetching %s" % name
    c = readParsePage(name)
    if name.find("/")>0:
        try: 
            os.makedirs(os.path.join(DEST_FOLDER, name[:name.rfind("/")]))
        except: pass
    if True:#name.find(".")<0:
        c, pi, level = patchLinks(c, name)
        for i in pi:
            images.add(i)
        c, pi = patchImages(c, name)
        for i in pi:
            images.add(i)
    name = name + ".html"
    fd = open(os.path.join(DEST_FOLDER, name), "w")
    fd.write(c)
    fd.close()

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
    fd = open(os.path.join(DEST_FOLDER, "images", i), "wb")
    fd.write(f.read())
    fd.close()





