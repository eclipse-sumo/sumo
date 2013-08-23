#!/usr/bin/env python
"""
@file    buildHTMLDocs.py
@author  Daniel Krajzewicz
@date    2011-10-20
@version $Id$

Converts wiki-documentation into HTML pages.

Determines what to convert, first: if a command line argument is given,
it is interpreted as the page to convert. Otherwise, "Special:AllPages" is
downloaded and parsed for obtaining the list of all pages which will be
converted in subsequent steps.

For each of the pages to convert, the HTML-representation of the
page is downloaded and stripped from wiki-header/footer, first.
Then, the image-links are extracted from the HTML page and stored 
temporarily, the links themselves are patched to point to local pages/images
(if the page behind the link exists).
The page is saved into MIRROR_FOLDER/<PAGE_PATH>. 

After parsing all pages, the images are downloaded and stored into 
MIRROR_FOLDER/images.

After downloading all data, the title page is extracted and the content
included in this page is extracted. This content is embedded into "index.html"
between the <!-- nav begins --> / <!-- nav ends --> markers. 
All pages downloaded earlier are loaded, and embedded into the index.html
between the <!-- content begins --> / <!-- content ends --> markers. Then,
the page is saved into HTML_FOLDER/<PAGE_PATH>. All images are copied
from MIRROR_FOLDER/images to HTML_FOLDER/images.

Copyright (C) 2011-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import urllib, os, sys, shutil
from mirrorWiki import readParsePage, readParseEditPage

def patchLinks(page, name):
    images = set()
    level = len(name.split("/"))-1
    level = "../" * level
    b = page.find("<a href")
    while b>=0:
        # images/files
        if page[b+9:].startswith("/wiki/File:") or page[b+9:].startswith("/wiki/Image:"):
            b2 = b
            b = page.find(":", b)+1
            images.add(page[b2+9:page.find("\"",b)])
            page = page[:b2+9] + level + "images/" + page[b:]
        # pages (HTML)
        elif page[b+9:].startswith("/wiki/"):
            e = b+9+6
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
        b += 5
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

def parseWikiLink(l):
    if l.find("[[")>=0:
        # internal link
        b = l.find("[")+2
        e = l.find("]", b)
        t = l[b:e]
        if t.find("|")<0:
            link = t
            text = t
        else:
            link, text = t.split("|")
        link = link.replace(" ", "_")
        if link.find("#")>=0:
            link = link.replace("#", ".html#")
        else:
            link = link + ".html"
        # external link
    elif l.find("[")>=0:
        b = l.find("[")+1
        e = l.find("]", b)
        t = l[b:e]
        link = t[:t.find(" ")]
        text = t[t.find(" ")+1:]
    else:
        # text
        text = l[l.find(" ")+1:]
        link = ""
    return text, link

MIRROR_FOLDER = "mirror"
HTML_FOLDER = "docs"

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
    if not p.startswith("href"):
        continue
    b = p.find("/wiki/")
    e = p.find("\"", b)
    name = p[b+6:e]
    if name.endswith(".css"):
        print "Skipping css-file %s" % name
        continue
    print "Fetching %s" % name
    c = readParsePage(name)
    if name.find("/")>0:
        try: 
            os.makedirs(os.path.join(MIRROR_FOLDER, name[:name.rfind("/")]))
        except: pass
    if True:#name.find(".")<0:
        c, pi, level = patchLinks(c, name)
        for i in pi:
            images.add(i)
        c, pi = patchImages(c, name)
        for i in pi:
            images.add(i)
    name = name + ".html"
    fd = open(os.path.join(MIRROR_FOLDER, name), "w")
    fd.write(c)
    fd.close()

imageFiles = []
for i in images:
    print "Fetching image %s" % i
    if i.find(":")>=0:
        f = urllib.urlopen("http://sumo-sim.org%s" % i)
        c = f.read()
        b = c.find("<div class=\"fullImageLink\" id=\"file\">")
        b = c.find("href=", b)+6
        e = c.find("\"", b+1)
        f = urllib.urlopen("http://sumo-sim.org/%s" % c[b:e])
        i = i[i.find(":")+1:]
    else:
        f = urllib.urlopen("http://sumo-sim.org/%s" % i)
        i = i[i.rfind("/")+1:]
    if i.find("px-")>=0:
        i = i[:i.find('-')+1]
    fd = open(os.path.join(MIRROR_FOLDER, "images", i), "wb")
    fd.write(f.read())
    fd.close()
    imageFiles.append(os.path.join("images", i))

# build navigation
nav = readParseEditPage("SUMO_User_Documentation")
lines = nav[nav.find("="):].split("\n")
level = 0
c = "<ul>\n";
hadHeader = False
for l in lines:
    if len(l)==0:
        continue
    if l[0]=='=':
        text, link = parseWikiLink(" " + l.replace("=", ""))
        if hadHeader:
            c = c + "</ul>\n";
        spc = ' ' * (level+1)
        c = c + spc + "<li>";	
        if link!="":
            c = c + "<a href=\"" + link + "\">";	
        c = c + text;
        if link!="":
            c = c + "</a>";
        c = c + "</li>\n";
        hadHeader = True
        level = 0
        continue
    if l[0].find('*')<0:
        continue
    text, link = parseWikiLink(l)
    nLevel = l.count('*')
    if level>nLevel:
        c = c + ("</ul>\n" * (level-nLevel))
    if level<nLevel:
        c = c + ("<ul>\n" * (nLevel-level))
    spc = ' ' * (nLevel+1)
    #+ str(level) + "-" + str(nLevel) 
    c = c + spc + "<li>";
    if link !="":
        c = c + '<a href="' + link + '">' + text + '</a>';
    else:
        c = c + text;
    c = c + "</li>\n";
    level = nLevel

# get template and embed navigation
fd = open("index.html")
tpl = fd.read()
fd.close()
b = tpl.find("<!-- nav begins -->")
b = tpl.find(">", b)+1
e = tpl.find("<!-- nav ends -->")
tpl = tpl[:b] + c + tpl[e:]
    
# build HTML pages
try: os.mkdir(HTML_FOLDER)
except: pass
try: os.mkdir(HTML_FOLDER + "/images")
except: pass
for p in pages:
    if not p.startswith("href"):
        continue
    b = p.find("/wiki/")
    e = p.find("\"", b)
    name = p[b+6:e]
    if name.endswith(".css"):
        print "Skipping css-file %s" % name
        continue
    name = name + ".html"
    t = os.path.join(HTML_FOLDER, name)
    fd = open(os.path.join(MIRROR_FOLDER, name))
    c = fd.read()
    fd.close()
    #
    if name.find('/')>=0:
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
    while b>=0:
        b = cc.find('"', b)
        if not cc[b+1:].startswith("http"):
            cc = cc[:b+1] + level + cc[b+1:]
        b = cc.find("<a href=", b)
    # content
    b = cc.find("<!-- content begins -->")
    e = cc.find("<!-- content ends -->", b)
    e = cc.find("<", e+1) - 1
    cc = cc[:b] + c + cc[e:]

    try: os.makedirs(os.path.split(t)[0])
    except: pass
    fd = open(t, "w")
    fd.write(cc)
    fd.close()
for i in imageFiles:
    shutil.copy(os.path.join(MIRROR_FOLDER, i), os.path.join(HTML_FOLDER, i))
shutil.copy(os.path.join(HTML_FOLDER, 'SUMO_User_Documentation.html'), os.path.join(HTML_FOLDER, 'index.html'))
