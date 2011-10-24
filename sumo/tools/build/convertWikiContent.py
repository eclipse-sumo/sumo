#!/usr/bin/env python
"""
@file    convertWikiContent.py
@author  Daniel.Krajzewicz@dlr.de
@date    2011-10-20
@version $Id$

Converts the content list stored in wiki-format in "content_user.txt" into
a HTML-list and writes it into "out.html".
Has then to be put into "index.html".

Copyright (C) 2010-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

def parseLink(l):
    if l.find("[[")>=0:
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
    elif l.find("[")>=0:
        b = l.find("[")+1
        e = l.find("]", b)
        t = l[b:e]
        link = t[:t.find(" ")]
        text = t[t.find(" ")+1:]
    else:
        text = l[l.find(" ")+1:]
        link = ""
    return text, link

fd = open("content_user.txt")
lines = fd.readlines()
fd.close()

level = 0
c = "<ul>\n";
hadHeader = False
for l in lines:
    if l[0]=='=':
        text, link = parseLink(" " + l[1:-2])
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
    text, link = parseLink(l)
    nLevel = l.count('*')
    if level>nLevel:
        c = c + ("</ul>" * (level-nLevel))
    if level<nLevel:
        c = c + ("<ul>" * (nLevel-level))
    spc = ' ' * (nLevel+1)
    #+ str(level) + "-" + str(nLevel) 
    c = c + spc + "<li>";
    if link !="":
        c = c + '<a href="' + link + '">' + text + '</a>';
    else:
        c = c + text;
    c = c + "</li>\n";
    level = nLevel

fd = open("out.html", "w")
fd.write(c)
fd.close()
